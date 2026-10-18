// 1.13c Storm.dll RVAs 0x1F30..0x23E0: TSFixedArray<RECTF>/<_FOUNDPARAM> setters and
// destructors, and the TSHashTableReuse<RGN> object, chunk and TSList primitives they sit on.
// Retail is 32-bit MSVC; this port keeps ptr-sized fields but the same link algebra:
// Link(ptr) = ptr + m_linkoffset, and a link's {prevlink,next} pair unlinks through
// NextLink(-1). Allocation names/line numbers passed to SMem* are not carried by our seam.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace {

constexpr uintptr_t kStormVtblHashReuseRgn = 0x6fc36ae4u;
constexpr uintptr_t kStormVtblHashFoundation = 0x6fc36ad4u;
constexpr uintptr_t kStormVtblHashBase = 0x6fc36b04u;
constexpr intptr_t kStormLinkOffsetDefault = (intptr_t)(int32_t)0xddddddddu;

// 1.13c Storm list layout: m_linkoffset then the terminator's {prevlink,next}.
struct StormLink {
    uintptr_t prevlink;
    uintptr_t next;
};
struct StormList {
    intptr_t linkoffset;
    StormLink term;
};

// TSFixedArray: retail {alloc,count,data,pad} - 16 bytes per array object.
struct StormFixedArray {
    uintptr_t alloc;
    uintptr_t count;
    void* data;
    uintptr_t pad;
};

// RGN node as used by the reuse hash table (retail stride 0x60).
struct StormElement {
    StormList list;             // retail +0x00
    StormLink link;             // retail +0x0c (fulllist link)
    uintptr_t id;               // retail +0x14 (ctor 0x2410 leaves it)
    StormFixedArray source;     // retail +0x18 (data +0x20)
    StormFixedArray rectf;      // retail +0x28 (data +0x30)
    StormFixedArray foundparam; // retail +0x38 (data +0x40)
    uintptr_t tail[6];          // retail +0x48
};

// TSHashObjectChunk<RGN,H>: retail {f0,count,data,fc,link@0x10}, extras after 0x18.
struct StormChunk {
    uintptr_t f0;
    uintptr_t count;
    StormElement* data;
    uintptr_t fc;
    StormLink link;
};

struct StormGrowableArray {
    uintptr_t cap;
    uintptr_t count;
    StormList* data;
    uintptr_t quantum;
};

// TSExplicitList<TSLink,RGN> base + TSHashTableReuse<RGN> derived fields.
struct StormHashTable {
    void* vtable;               // retail +0x00
    StormList fulllist;         // retail +0x04, linkoffset 0x0c
    uintptr_t field10;          // retail +0x10
    StormGrowableArray buckets; // retail +0x14
    uintptr_t bucketMask;       // retail +0x24
    StormList reuseList;        // retail +0x28, linkoffset 0x04
    uintptr_t chunkSize;        // retail +0x34
    StormList chunkList;        // retail +0x38, linkoffset 0x10
    uintptr_t field44;          // retail +0x44
    uintptr_t field48;          // retail +0x48 (0x2020)
    uintptr_t field4c;          // retail +0x4c (0x2020)
};

inline void* StormAlloc(uintptr_t size) { return std::malloc(size ? size : 1); }
inline void* StormRealloc(void* p, uintptr_t size) {
    void* r = std::realloc(p, size ? size : 1);
    return r;
}
inline void StormFree(void* p) { std::free(p); }

// 1.13c Storm+0x4C20/0x2160 unlink step: resolve NextLink(-1), splice, clear.
void StormUnlink(StormLink* link) {
    uintptr_t prev = link->prevlink;
    if (!prev) return;
    uintptr_t next = link->next;
    uintptr_t target;
    if ((intptr_t)next < 0) {
        target = ~next;
    } else {
        target = next + ((uintptr_t)link - *(uintptr_t*)(prev + sizeof(uintptr_t)));
    }
    *(uintptr_t*)target = prev;
    *(uintptr_t*)(prev + sizeof(uintptr_t)) = next;
    link->prevlink = 0;
    link->next = 0;
}

void StormListInitialize(StormList* list, intptr_t linkoffset) {
    list->linkoffset = linkoffset;
    list->term.prevlink = (uintptr_t)&list->term;
    list->term.next = ~(uintptr_t)&list->term;
}
void StormListInitializeDefault(StormList* list) { StormListInitialize(list, kStormLinkOffsetDefault); }

void StormListUnlinkAll(StormList* list) {
    while ((intptr_t)list->term.next > 0) {
        uintptr_t node = list->term.next;
        StormUnlink((StormLink*)(node + (uintptr_t)list->linkoffset));
    }
}

// 1.13c Storm+0x22D0: unlink every node, then unlink the terminator itself.
void StormListDestroy(StormList* list) {
    StormListUnlinkAll(list);
    StormUnlink(&list->term);
}

// 1.13c Storm+0x32E0 change-link-offset pattern (inline in 0x2AF0/0x2560).
void StormListChangeLinkOffset(StormList* list, intptr_t linkoffset) {
    if (list->linkoffset == linkoffset) return;
    StormListUnlinkAll(list);
    StormListInitialize(list, linkoffset);
}

// 1.13c Storm+0x2160: link node (or the terminator when node==0) by linktype; 1=after, else=before.
void StormListLinkNode(StormList* list, uintptr_t node, uint32_t linktype, uintptr_t existing) {
    StormLink* f = (StormLink*)(node ? node + (uintptr_t)list->linkoffset : (uintptr_t)&list->term);
    if (f->prevlink) StormUnlink(f);
    StormLink* ef = existing ? (StormLink*)(existing + (uintptr_t)list->linkoffset) : &list->term;
    if (linktype != 1) {
        uintptr_t prev = ef->prevlink;
        f->prevlink = prev;
        f->next = ((StormLink*)prev)->next;
        ((StormLink*)prev)->next = node;
        ef->prevlink = (uintptr_t)f;
    } else {
        f->prevlink = (uintptr_t)ef;
        uintptr_t next = ef->next;
        f->next = next;
        if ((intptr_t)next < 0) {
            *(uintptr_t*)(~next) = (uintptr_t)f;
        } else {
            uintptr_t target = next;
            intptr_t offset = (intptr_t)list->linkoffset;
            if (offset < 0) target += (uintptr_t)ef - ((StormLink*)next)->prevlink;
            *(uintptr_t*)target = (uintptr_t)f;
        }
        ef->next = node;
    }
}

// 1.13c Storm+0x1CF0: unlink the element's own TSLink, then its embedded list terminator.
void StormElementUnlink(StormElement* e) {
    StormUnlink(&e->link);
    StormUnlink(&e->list.term);
}

// 1.13c Storm+0x2740: destroy every RGN in the chunk, then free the array.
void StormChunkDestroy(StormChunk* c) {
    for (uintptr_t i = 0; i < c->count; i++) {
        StormElement* e = &c->data[i];
        if (e->foundparam.data) StormFree(e->foundparam.data);
        if (e->rectf.data) StormFree(e->rectf.data);
        if (e->source.data) StormFree(e->source.data);
        StormElementUnlink(e);
    }
    if (c->data) StormFree(c->data);
}

// 1.13c Storm+0x27D0: unlink the chunk from its list, then destroy it.
void StormChunkDelete(StormChunk* c) {
    StormUnlink(&c->link);
    StormChunkDestroy(c);
}

// 1.13c Storm+0x2080: drain the chunk list (freeing chars) then the reuse list (freeing RGNs).
void StormHashTableDestroyChunks(StormHashTable* h) {
    while ((intptr_t)h->chunkList.term.next > 0) {
        StormChunk* c = (StormChunk*)h->chunkList.term.next;
        StormChunkDelete(c);
        StormFree(c);
    }
    while ((intptr_t)h->reuseList.term.next > 0) {
        StormElement* e = (StormElement*)h->reuseList.term.next;
        if (e->foundparam.data) StormFree(e->foundparam.data);
        if (e->rectf.data) StormFree(e->rectf.data);
        if (e->source.data) StormFree(e->source.data);
        StormElementUnlink(e);
        StormFree(e);
    }
}

// 1.13c Storm+0x17F0 (vtable slot 0): unlink the node from fulllist and reuseList, relink to reuse tail.
void StormHashTableInternalDelete(StormHashTable* h, uintptr_t node) {
    StormUnlink((StormLink*)(node + (uintptr_t)h->fulllist.linkoffset));
    StormUnlink((StormLink*)(node + (uintptr_t)h->reuseList.linkoffset));
    StormListLinkNode(&h->reuseList, node, 1, 0);
}

// 1.13c Storm+0x33C0: rounding quantum - largest power of two <= wanted, clamped at 0x15.
uintptr_t StormArrayQuantum(uintptr_t wanted, StormGrowableArray* a) {
    if (wanted < 0x15) {
        uintptr_t v = (wanted - 1) & wanted;
        while (v) {
            wanted = v;
            v = (wanted - 1) & wanted;
        }
        return wanted < 1 ? 1 : wanted;
    }
    a->quantum = 0x15;
    return 0x15;
}

// 1.13c Storm+0x3400: destroy the tail, then realloc; on failure alloc+copy+destroy+free.
void StormArrayRealloc(StormGrowableArray* a, uintptr_t newcap) {
    StormList* olddata = a->data;
    uintptr_t oldcount = a->count;
    if (newcap < oldcount) {
        for (uintptr_t i = newcap; i < oldcount; i++) StormListDestroy(&olddata[i]);
    }
    a->cap = newcap;
    StormList* data = (StormList*)StormRealloc(olddata, newcap * sizeof(StormList));
    a->data = data;
    if (!data) {
        data = (StormList*)StormAlloc(newcap * sizeof(StormList));
        a->data = data;
        if (olddata) {
            uintptr_t n = newcap < oldcount ? newcap : oldcount;
            for (uintptr_t i = 0; i < n; i++) {
                StormListInitialize(&data[i], olddata[i].linkoffset);
                StormListDestroy(&olddata[i]);
            }
            StormFree(olddata);
        }
    }
}

// 1.13c Storm+0x32E0: grow (quantum-rounded realloc + default-init) or shrink (destroy) the array.
void StormArrayResize(StormGrowableArray* a, uintptr_t wanted) {
    if (wanted > a->count) {
        if (wanted > a->cap) {
            if (a->quantum == 0) a->quantum = StormArrayQuantum(wanted, a);
            uintptr_t q = a->quantum;
            uintptr_t rounded = wanted;
            if (wanted % q) rounded = wanted + (q - wanted % q);
            StormArrayRealloc(a, rounded);
        }
        for (uintptr_t i = a->count; i < wanted; i++) StormListInitializeDefault(&a->data[i]);
        a->count = wanted;
        return;
    }
    if (wanted < a->count) {
        for (uintptr_t i = wanted; i < a->count; i++) StormListDestroy(&a->data[i]);
    }
    a->count = wanted;
}

// 1.13c Storm+0x2290: TSGrowableArray<TSList> destructor - destroy elements, free data.
void StormListArrayDestroy(StormGrowableArray* a) {
    for (uintptr_t i = 0; i < a->count; i++) StormListDestroy(&a->data[i]);
    if (a->data) StormFree(a->data);
}

// 1.13c Storm+0x2AF0: TSHashTable base ctor - fulllist (0x0c), zero buckets, 4 buckets at offset 4.
void StormHashTableBaseInit(StormHashTable* h) {
    h->vtable = (void*)kStormVtblHashBase;
    StormListInitializeDefault(&h->fulllist);
    h->field10 = 0;
    std::memset(&h->buckets, 0, sizeof(h->buckets));
    StormListChangeLinkOffset(&h->fulllist, 0x0c);
    h->bucketMask = 3;
    StormArrayResize(&h->buckets, 4);
    for (uintptr_t i = 0; i <= h->bucketMask; i++) {
        StormListChangeLinkOffset(&h->buckets.data[i], 4);
    }
}

// 1.13c Storm+0x2560: reuse-table ctor - reuseList (4) and chunkList (0x10), chunk size 0x10.
void StormHashTableReuseInit(StormHashTable* h) {
    StormHashTableBaseInit(h);
    h->vtable = (void*)kStormVtblHashFoundation;
    StormListInitializeDefault(&h->reuseList);
    StormListInitialize(&h->chunkList, 0x10);
    h->chunkSize = 0x10;
    StormListChangeLinkOffset(&h->reuseList, 4);
}

// 1.13c Storm+0x20C0: clear fulllist and every bucket; warn==0 deletes via vtable slot 0.
void StormHashTableClear(StormHashTable* h, uint32_t warn) {
    h->field10 = 0;
    StormListUnlinkAll(&h->fulllist);
    for (uintptr_t i = 0; i < h->buckets.count; i++) {
        StormList* bucket = &h->buckets.data[i];
        while ((intptr_t)bucket->term.next > 0) {
            uintptr_t node = bucket->term.next;
            if (warn == 0) {
                StormHashTableInternalDelete(h, node);
            } else {
                StormUnlink((StormLink*)(node + (uintptr_t)bucket->linkoffset));
            }
        }
    }
}

// 1.13c Storm+0x1A70: hash base dtor.
void StormHashTableBaseDestroy(StormHashTable* h) {
    h->vtable = (void*)kStormVtblHashBase;
    StormHashTableClear(h, 1);
    StormListArrayDestroy(&h->buckets);
    StormListDestroy(&h->fulllist);
}

// 1.13c Storm+0x1990: foundation dtor - chunk/reuse lists, then base.
void StormHashTableFoundationDestroy(StormHashTable* h) {
    h->vtable = (void*)kStormVtblHashFoundation;
    StormHashTableDestroyChunks(h);
    StormListDestroy(&h->chunkList);
    StormListDestroy(&h->reuseList);
    StormHashTableBaseDestroy(h);
}

// 1.13c Storm+0x16B0: RGN reuse-table dtor (no deallocation of the object itself).
void StormHashTableReuseDestroy(StormHashTable* h) {
    StormHashTableFoundationDestroy(h);
}

// 1.13c Storm+0x1F30: TSFixedArray<RECTF>::Set - 16-byte elements.
void StormFixedArraySetRectF(StormFixedArray* a, uintptr_t count, const void* src) {
    a->alloc = count;
    if (a->data || count) {
        a->data = StormRealloc(a->data, count * 16);
    }
    for (uintptr_t i = 0; i < count; i++) {
        uint8_t* d = (uint8_t*)a->data + i * 16;
        if (d) std::memcpy(d, (const uint8_t*)src + i * 16, 16);
    }
    a->count = count;
}

// 1.13c Storm+0x1FC0: TSFixedArray<_FOUNDPARAM>::Set - 8-byte elements.
void StormFixedArraySetFoundParam(StormFixedArray* a, uintptr_t count, const void* src) {
    a->alloc = count;
    if (a->data || count) {
        a->data = StormRealloc(a->data, count * 8);
    }
    for (uintptr_t i = 0; i < count; i++) {
        uint8_t* d = (uint8_t*)a->data + i * 8;
        if (d) std::memcpy(d, (const uint8_t*)src + i * 8, 8);
    }
    a->count = count;
}

// 1.13c Storm+0x1FA0: TSFixedArray destructor - free data only.
void StormFixedArrayDestroy(StormFixedArray* a) {
    if (a->data) StormFree(a->data);
}

// 1.13c Storm+0x2410: RGN ctor - zero every field (retail leaves +0 and +0x14).
StormElement* StormElementInit(StormElement* e) {
    std::memset(e, 0, sizeof(StormElement));
    return e;
}

// 1.13c Storm+0x2210: list NewNode - allocate RGN (extrabytes + size), ctor, link by location.
void* StormHashTableInternalNew(StormHashTable* h, uintptr_t location, uintptr_t extrabytes, uint32_t flags) {
    (void)flags;
    void* mem = StormAlloc(extrabytes + sizeof(StormElement));
    StormElement* e = mem ? StormElementInit((StormElement*)mem) : nullptr;
    if (location) {
        StormListLinkNode(&h->fulllist, (uintptr_t)e, (uint32_t)location, 0);
    }
    return e;
}

// 1.13c Storm+0x2350: list NewNode for TSHashObjectChunk - header zeroed, link by location.
void* StormHashTableChunkNew(StormHashTable* h, uintptr_t location, uintptr_t extrabytes, uint32_t flags) {
    (void)flags;
    StormChunk* c = (StormChunk*)StormAlloc(extrabytes + sizeof(StormChunk));
    if (c) std::memset(c, 0, sizeof(StormChunk));
    if (location) {
        StormListLinkNode(&h->fulllist, (uintptr_t)c, (uint32_t)location, 0);
    }
    return c;
}

}  // namespace

#define P(T, i) ((T)(uintptr_t)r[i])

// 1.13c Storm+0x1F30: TSFixedArray<RECTF>::Set(esi=this, edi=count, [esp+4]=src)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1f30(uint32_t* r) {
    StormFixedArraySetRectF(P(StormFixedArray*, 0), (uintptr_t)r[1], P(const void*, 2));
    r[4] = 0;
}

// 1.13c Storm+0x1FA0: TSFixedArray destructor(eax=this)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1fa0(uint32_t* r) {
    StormFixedArrayDestroy(P(StormFixedArray*, 0));
    r[4] = 0;
}

// 1.13c Storm+0x1FC0: TSFixedArray<_FOUNDPARAM>::Set(edi=this, esi=count, eax=src)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1fc0(uint32_t* r) {
    StormFixedArraySetFoundParam(P(StormFixedArray*, 0), (uintptr_t)r[1], P(const void*, 2));
    r[4] = 0;
}

// 1.13c Storm+0x2020: TSHashTableReuse<RGN> ctor([esp+4]=this) -> this
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2020(uint32_t* r) {
    StormHashTable* h = P(StormHashTable*, 0);
    StormHashTableReuseInit(h);
    h->vtable = (void*)kStormVtblHashReuseRgn;
    h->field48 = (~(uintptr_t)h) & 0x0fffffffu;
    h->field4c = 0;
    r[4] = (uint32_t)(uintptr_t)h;
}

// 1.13c Storm+0x2080: TSHashTable dtor body(edi=this)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2080(uint32_t* r) {
    StormHashTableDestroyChunks(P(StormHashTable*, 0));
    r[4] = 0;
}

// 1.13c Storm+0x20C0: TSHashTable::Clear(ebx=this, [esp+4]=warn)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_20c0(uint32_t* r) {
    StormHashTableClear(P(StormHashTable*, 0), (uint32_t)r[1]);
    r[4] = 0;
}

// 1.13c Storm+0x2160: TSList::LinkNode(eax=list, ebx=node, [esp+4]=linktype, [esp+8]=existing)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2160(uint32_t* r) {
    StormListLinkNode(P(StormList*, 0), (uintptr_t)r[1], (uint32_t)r[2], (uintptr_t)r[3]);
    r[4] = 0;
}

// 1.13c Storm+0x2210: NewNode(esi=location, [esp+4]=list, [esp+8]=extrabytes, [esp+12]=flags)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2210(uint32_t* r) {
    void* p = StormHashTableInternalNew(P(StormHashTable*, 1), (uintptr_t)r[0], (uintptr_t)r[2], (uint32_t)r[3]);
    r[4] = (uint32_t)(uintptr_t)p;
}

// 1.13c Storm+0x2290: TSGrowableArray<TSList> dtor(eax=array)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2290(uint32_t* r) {
    StormListArrayDestroy(P(StormGrowableArray*, 0));
    r[4] = 0;
}

// 1.13c Storm+0x22D0: TSList dtor([esp+4]=this)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_22d0(uint32_t* r) {
    StormListDestroy(P(StormList*, 0));
    r[4] = 0;
}

// 1.13c Storm+0x2350: chunk NewNode(esi=location, [esp+4]=list, [esp+8]=extrabytes, [esp+12]=flags)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2350(uint32_t* r) {
    void* p = StormHashTableChunkNew(P(StormHashTable*, 1), (uintptr_t)r[0], (uintptr_t)r[2], (uint32_t)r[3]);
    r[4] = (uint32_t)(uintptr_t)p;
}

// 1.13c Storm+0x23E0: TSHashTableReuse<RGN> deleting dtor(ecx=this, [esp+4]=flags) -> this
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_23e0(uint32_t* r) {
    StormHashTable* h = P(StormHashTable*, 0);
    StormHashTableReuseDestroy(h);
    if ((r[1] & 1) && h) StormFree(h);
    r[4] = (uint32_t)(uintptr_t)h;
}

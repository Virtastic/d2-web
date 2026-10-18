// src/core/gfx/storm_batch_1520.cpp -- 12 1.13c Storm.dll internal rows (0x1520..0x1AF0)
// reimplemented from the decompiles in build-wasm/ghidra-storm/decomp/0x<RVA>.c (Stage A,
// RE-DONE; no spec, no vector yet).
//
// These are Storm's internal region/chunk bookkeeping classes, not exports: the AURGN allocation
// records (0x1560/0x15E0 allocate a named record plus two list dwords) and the relative-list
// containers behind them (0x17F0/0x18B0 splice nodes, 0x1990/0x1A70/0x1A30 are destructors).
// Retail stores the list links as two 32-bit dwords -- {link, tag}, where a negative tag is the
// bitwise complement of the previous-link address and a non-negative tag is the byte offset to it
// -- and the same unlink snippet recurs in every body below; it is the one model here
// (storm_unlink), written with the same 32-bit truncation as src/core/fn/storm_leaves_b23.cpp
// (Storm+0x4C20). StormLib replaces Storm in our build and nothing in the engine calls these
// internals, so the pool seams (storm_mem_alloc/storm_mem_free, the chunk allocator) are host
// calloc/free with the retail argument order; the virtual-slot call in 0x20C0 and the chunk grow
// in 0x2C60 are marked where they are modeled rather than recovered.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host kernel state, a no-op under wasm

namespace {

// ---- pool seams --------------------------------------------------------------------------------
// 0x1C580 SMemAlloc(size, tag, -2, flags): deterministic host allocation, tag/flags kept for
// provenance. 0x1B960 SMemFree(ptr, tag, -2) is its pair.
void* storm_mem_alloc(uint32_t size) { return std::calloc(1, size ? size : 1); }
void storm_mem_free(void* p, const char* tag, int32_t line) {
    (void)tag; (void)line;
    std::free(p);
}

// The {link, tag} relative-list node unlink, byte-for-byte the snippet at Storm+0x4C20 and
// repeated in 0x4B80/0x17F0/0x18B0/0x22D0. Retail is 32-bit; ours keeps the same truncation.
struct StormLink { uint32_t link, tag; };
static_assert(offsetof(StormLink, tag) == 4, "tag at +4");

void storm_unlink(uint32_t* node) {
    uint32_t link = node[0];
    if (link == 0) return;
    uint32_t tag = node[1];
    uint32_t* slot = (int32_t)tag < 0 ? (uint32_t*)(uintptr_t)~tag
                                      : (uint32_t*)((uintptr_t)node + (uint32_t)(tag - *(uint32_t*)((uintptr_t)link + 4)));
    *slot = link;
    *(uint32_t*)((uintptr_t)link + 4) = tag;
    node[0] = 0;
    node[1] = 0;
}

// 0x4B80: two consecutive unlinks on the same node (the retail body repeats the snippet twice;
// the second is a no-op once the first clears the node).
void storm_sub_4b80(uint32_t* node) { storm_unlink(node); storm_unlink(node); }

// 0x4390: insert `node` into the relative list whose anchor object is `list`; mode 1 links it as
// the tail (tag = previous link address), any other mode links it after the head. Retail carries
// the anchor in EAX and a default `this` in EAX+4 -- both are explicit here.
void storm_link(uint32_t* list, uint32_t* node, uint32_t mode) {
    storm_unlink(node);
    uint32_t* anchor = list + 1;
    if (mode != 1) {
        uint32_t head = anchor[0];
        node[0] = head;
        node[1] = *(uint32_t*)((uintptr_t)head + 4);
        *(uint32_t*)((uintptr_t)head + 4) = (uint32_t)(uintptr_t)node;
        anchor[0] = (uint32_t)(uintptr_t)node;
        return;
    }
    node[0] = (uint32_t)(uintptr_t)anchor;
    node[1] = anchor[1];
    uint32_t tag = anchor[1];
    uint32_t base = list[0];
    if ((int32_t)tag < 0) {
        *(uint32_t**)(uintptr_t)~tag = node;
        anchor[1] = (uint32_t)(uintptr_t)node;
        return;
    }
    if ((int32_t)base < 0)
        base = (uint32_t)(uintptr_t)anchor - *(uint32_t*)((uintptr_t)*(uint32_t*)(uintptr_t)anchor + 4);
    *(uint32_t**)(uintptr_t)(tag + base) = node;
    anchor[1] = (uint32_t)(uintptr_t)node;
}

// 0x2780: drain the node at obj[0] + obj[2] while obj[2] is positive.
void storm_sub_2780(uint32_t* obj) {
    int32_t off;
    while ((off = (int32_t)obj[2]) > 0) storm_unlink((uint32_t*)(uintptr_t)(obj[0] + (uint32_t)off));
}

// 0x1CF0: unlink the two embedded nodes at +0xc and +4.
void storm_sub_1cf0(uint32_t* p) {
    storm_unlink(p + 3);
    storm_unlink(p + 1);
}

// 0x2740: free each 0x60-byte element in the array at +8 (count at +4) through 0x1C30, then the
// array itself.
void storm_sub_1c30(uint32_t* p);   // 0x1C30, below
void storm_sub_2740(uint32_t* obj) {
    uint32_t n = obj[1];
    for (uint32_t i = 0; i < n; ++i)
        storm_sub_1c30((uint32_t*)(uintptr_t)(obj[2] + i * 0x60));
    if (obj[2] != 0) storm_mem_free((void*)(uintptr_t)obj[2], "__AURGN__", -2);
}

// 0x1C30: release a region record's three named buffers at +0x40/+0x30/+0x20, then 0x1CF0.
void storm_sub_1c30(uint32_t* p) {
    if (p[0x10] != 0) storm_mem_free((void*)(uintptr_t)p[0x10], "__AU_FOUNDPARAM__", -2);
    if (p[0x0c] != 0) storm_mem_free((void*)(uintptr_t)p[0x0c], "__AURECTF__", -2);
    if (p[0x08] != 0) storm_mem_free((void*)(uintptr_t)p[0x08], "__AU_SOURCE__", -2);
    storm_sub_1cf0(p);
}

// 0x22D0: drain the list at p, then unlink the embedded node at p+4.
void storm_sub_22d0(uint32_t* p) { storm_sub_2780(p); storm_unlink(p + 1); }

// 0x1B80: unlink the embedded node at p+4.
void storm_sub_1b80(uint32_t* p) { storm_sub_22d0(p); }

// 0x2290: destroy each 0xc-byte record in the array at +8 (count at +4), then the array.
void storm_sub_2290(uint32_t* obj) {
    uint32_t n = obj[1];
    for (uint32_t i = 0; i < n; ++i)
        storm_sub_1b80((uint32_t*)(uintptr_t)(obj[2] + i * 0xc));
    if (obj[2] != 0)
        storm_mem_free((void*)(uintptr_t)obj[2], "__AV__TSExplicitList(URGN(0,CC))", -2);
}

// 0x20C0: reset slot +0x10, drain the list at obj, then walk the obj[6] tables of 0xc-byte
// records at obj[7]: param_1 == 0 calls the record through the object's first virtual slot,
// otherwise the record node at base + offset is unlinked. The retail vtable slot is image state,
// so that one call is a seam.
void storm_call_vslot0(uint32_t* self, uint32_t record) { (void)self; (void)record; }
void storm_sub_20c0(uint32_t* self, uint32_t param_1) {
    self[4] = 0;
    storm_sub_2780(self);
    for (uint32_t i = 0; i < self[6]; ++i) {
        for (;;) {
            uint32_t base = *(uint32_t*)(uintptr_t)(self[7] + i * 0xc);
            int32_t off = (int32_t)*(uint32_t*)(uintptr_t)(self[7] + i * 0xc + 8);
            if (off < 1) break;
            if (param_1 == 0) storm_call_vslot0(self, (uint32_t)off);
            else {
                uint32_t* node = (uint32_t*)(uintptr_t)(base + (uint32_t)off);
                if (*node != 0) storm_unlink(node);
            }
        }
    }
}

// 0x2350: allocate one chunk header (param_2 + 0x18 bytes) and zero its six dwords.
uint32_t* storm_sub_2350(uint32_t param_2, uint32_t param_3) {
    uint32_t* p = (uint32_t*)storm_mem_alloc(param_2 + 0x18);
    if (p != nullptr) { p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0; }
    (void)param_3;
    return p;
}

// 0x2C20: round a capacity up to a power of two, minimum 8.
uint32_t storm_sub_2c20(uint32_t n, uint32_t* chunk) {
    if (n >= 8) { chunk[3] = 8; return 8; }
    uint32_t p = 1;   // retail's n & (n-1) walk leaves the highest power of two <= n, 1 for 0
    while ((p << 1) != 0 && (p << 1) <= n) p <<= 1;
    return p == 0 ? 1 : p;
}

// 0x2C60: resize the 0x60-byte element array to `count` entries, preserving the first entries.
void storm_sub_2c60(uint32_t* chunk, uint32_t count) {
    if (count == 0) count = 1;   // model: a zero grow step cannot make progress
    uint32_t keep = chunk[1] < count ? chunk[1] : count;
    uint32_t* fresh = (uint32_t*)storm_mem_alloc(count * 0x60);
    if (fresh != nullptr && chunk[2] != 0) {
        std::memcpy(fresh, (void*)(uintptr_t)chunk[2], (size_t)keep * 0x60);
    }
    if (chunk[2] != 0) storm_mem_free((void*)(uintptr_t)chunk[2], "__AURGN__", -2);
    chunk[2] = (uint32_t)(uintptr_t)fresh;
    chunk[0] = count;
}

// 0x26E0: take the next 0x60-byte element from `chunk`, growing it first when full; returns the
// element's base+offset (0 for the first element).
uint32_t storm_sub_26e0(uint32_t* chunk) {
    if (chunk[0] < chunk[1] + 1) {
        if (chunk[3] == 0) chunk[0] = storm_sub_2c20(chunk[0], chunk);
        storm_sub_2c60(chunk, chunk[0]);
    }
    uint32_t off = chunk[1] * 0x60 + chunk[2];
    chunk[1] += 1;
    return off;
}

// 0x27D0: unlink the embedded node at +0x10, then release the element array (0x2740).
void storm_sub_27d0(uint32_t* chunk) {
    storm_unlink(chunk + 4);
    storm_sub_2740(chunk);
}

// 0x2080: drain both chunk lists (+0x40 through 0x27D0, +0x30 through 0x1C30) and free each
// chunk with its tag. The list head at self+0x40 is re-read each pass, exactly as retail does;
// the unlink inside 0x27D0 is what advances it.
void storm_sub_2080(uint32_t* self) {
    uint32_t* p;
    while ((p = *(uint32_t**)(uintptr_t)(((uintptr_t)self) + 0x40)) != nullptr &&
           (intptr_t)p > 0) {
        storm_sub_27d0(p);
        storm_mem_free(p, "__AV__TSHashObjectChunk(URGN(VH))", -2);
    }
    while ((p = *(uint32_t**)(uintptr_t)(((uintptr_t)self) + 0x30)) != nullptr &&
           (intptr_t)p > 0) {
        storm_sub_1c30(p);
        storm_mem_free(p, "__AURGN__", -2);
    }
}

// 0x2020: base constructor. 0x2560 (not this batch) leaves the vtable plus the empty relative
// lists at +0x0c, +0x28 and +0x38; those sentinels are modelled here (init pattern of 0x2BC0:
// {0xDDDDDDDD, self, ~self}; 0x2D20 for the +0x38 one: {0x10, self, ~self}).
void storm_sub_2020(uint32_t* p) {
    p[1] = 0xc;
    p[2] = (uint32_t)(uintptr_t)(p + 2);
    p[3] = (uint32_t)~(uint32_t)(uintptr_t)(p + 2);
    p[4] = p[5] = p[6] = p[7] = p[8] = 0;
    p[9] = 3;
    p[10] = 0xdddddddd;
    p[11] = (uint32_t)(uintptr_t)(p + 11);
    p[12] = (uint32_t)~(uint32_t)(uintptr_t)(p + 11);
    p[13] = 0x10;
    p[14] = 0x10;
    p[15] = (uint32_t)(uintptr_t)(p + 15);
    p[16] = (uint32_t)~(uint32_t)(uintptr_t)(p + 15);
    p[0] = 0;
    p[0x12] = (uint32_t)~((uint32_t)(uintptr_t)p) & 0x0fffffffu;
    p[0x13] = 0;
}

// Retail vtables are image state (0x6FC36AD4/0x6FC36AE4/0x6FC36AF4/0x6FC36B04); ours are real
// distinct objects so a stored slot is a valid pointer.
uintptr_t g_vtable_36ad4;
uintptr_t g_vtable_36af4;
uintptr_t g_vtable_36b04;

// 0x1A70: install the vtable (retail 0x6FC36B04), reset the table part (0x20C0 mode 1), drain
// the explicit list (0x2290), then unlink the node at +4 (0x22D0).
void storm_sub_1a70(uint32_t* p) {
    p[0] = (uint32_t)(uintptr_t)&g_vtable_36b04;
    storm_sub_20c0(p, 1);
    storm_sub_2290(p);
    storm_sub_22d0(p + 1);
}

// 0x1990: install the base vtable (retail 0x6FC36AD4), drain the chunk lists (0x2080), unlink
// the two relative lists at +0x38 and +0x28 (0x22D0), then 0x1A70.
void storm_sub_1990(uint32_t* p) {
    p[0] = (uint32_t)(uintptr_t)&g_vtable_36ad4;
    storm_sub_2080(p);
    storm_sub_22d0((uint32_t*)((uintptr_t)p + 0x38));
    storm_sub_22d0((uint32_t*)((uintptr_t)p + 0x28));
    storm_sub_1a70(p);
}

}  // namespace

// 1.13c Storm+0x1520: free one AUBREAKCMD record; returns the record's +4 dword when positive
// (0 otherwise). ESI carries `this` in retail, EDI/param_1 is the fallback when it is null.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1520(uint32_t* r) {
    uint32_t thisp = r[0];
    uint32_t param_1 = r[1];
    uint32_t* fields = thisp != 0 ? (uint32_t*)(uintptr_t)thisp
                                  : (uint32_t*)(uintptr_t)(param_1 + 4);
    uint32_t uVar1 = fields[1];
    storm_sub_4b80((uint32_t*)(uintptr_t)thisp);
    storm_mem_free((void*)(uintptr_t)thisp, "__AUBREAKCMD__", -2);
    r[4] = (int32_t)uVar1 < 1 ? 0u : uVar1;
}

// 1.13c Storm+0x1560: allocate an AUBREAKCMD record of size+0xc, clear its two list dwords,
// and insert it (third register argument = insert mode) into the caller's list.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1560(uint32_t* r) {
    uint32_t* p = (uint32_t*)storm_mem_alloc(r[1] + 0xc);
    if (p != nullptr) {
        p[0] = 0;
        p[1] = 0;
    }
    if (r[3] != 0) storm_link((uint32_t*)(uintptr_t)r[0], p, r[3]);
    r[4] = (uint32_t)(uintptr_t)p;
}

// 1.13c Storm+0x15E0: the same constructor for an AUWNDREC record.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_15e0(uint32_t* r) {
    uint32_t* p = (uint32_t*)storm_mem_alloc(r[1] + 0xc);
    if (p != nullptr) {
        p[0] = 0;
        p[1] = 0;
    }
    if (r[3] != 0) storm_link((uint32_t*)(uintptr_t)r[0], p, r[3]);
    r[4] = (uint32_t)(uintptr_t)p;
}

// 1.13c Storm+0x1660: destroy the critical section at +0x50, then the base object (0x1990).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1660(uint32_t* r) {
    uint32_t* p = (uint32_t*)(uintptr_t)r[0];
    DeleteCriticalSection((LPCRITICAL_SECTION)((uintptr_t)p + 0x50));
    storm_sub_1990(p);
    r[4] = 0;
}

// 1.13c Storm+0x16B0: base-object destructor only.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_16b0(uint32_t* r) {
    storm_sub_1990((uint32_t*)(uintptr_t)r[0]);
    r[4] = 0;
}

// 1.13c Storm+0x1790: construct the base object, install the vtable (retail 0x6FC36AF4) and
// initialise the critical section at +0x50; returns the object.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1790(uint32_t* r) {
    uint32_t* p = (uint32_t*)(uintptr_t)r[0];
    storm_sub_2020(p);
    p[0] = (uint32_t)(uintptr_t)&g_vtable_36af4;
    InitializeCriticalSection((LPCRITICAL_SECTION)((uintptr_t)p + 0x50));
    r[4] = (uint32_t)(uintptr_t)p;
}

// 1.13c Storm+0x17F0: move the node at the offset `param_1` from the list at +8 (base at +4)
// and from the list at +0x2c (base at +0x28), then link the second anchor node back as the tail
// with the saved tag. The two unlink blocks are the shared 0x4C20 snippet.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_17f0(uint32_t* r) {
    uint8_t* thisp = (uint8_t*)(uintptr_t)r[0];
    uint32_t param_1 = r[1];
    uint32_t* piVar2;
    if (param_1 == 0) piVar2 = (uint32_t*)(thisp + 8);
    else piVar2 = (uint32_t*)(uintptr_t)(*(uint32_t*)(thisp + 4) + param_1);
    if (piVar2[0] != 0) storm_unlink(piVar2);
    if (param_1 == 0) piVar2 = (uint32_t*)(thisp + 0x2c);
    else piVar2 = (uint32_t*)(uintptr_t)(*(uint32_t*)(thisp + 0x28) + param_1);
    if (piVar2[0] != 0) storm_unlink(piVar2);
    uint32_t* piVar3 = (uint32_t*)(thisp + 0x2c);
    piVar2[0] = (uint32_t)(uintptr_t)piVar3;
    piVar2[1] = *(uint32_t*)(thisp + 0x30);
    uint32_t iVar4 = *(uint32_t*)(thisp + 0x28);
    uint32_t uVar1 = *(uint32_t*)(thisp + 0x30);
    if ((int32_t)uVar1 < 0) {
        *(uint32_t**)(uintptr_t)~uVar1 = piVar2;
        *(uint32_t*)(thisp + 0x30) = param_1;
        r[4] = 0;
        return;
    }
    if ((int32_t)iVar4 < 0)
        iVar4 = (uint32_t)(uintptr_t)piVar3 - *(uint32_t*)((uintptr_t)*(uint32_t*)(uintptr_t)piVar3 + 4);
    *(uint32_t**)(uintptr_t)(uVar1 + iVar4) = piVar2;
    *(uint32_t*)(thisp + 0x30) = param_1;
    r[4] = 0;
}

// 1.13c Storm+0x18B0: append a node to the relative list param_1. When the current chunk is
// absent or full, allocate one (0x2350), grow it (0x2C60) and double the object's +0x34 step;
// take the next element (0x26E0), replace whatever occupied it, and link it as the tail.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_18b0(uint32_t* r) {
    uint8_t* thisp = (uint8_t*)(uintptr_t)r[0];
    uint32_t* param_1 = (uint32_t*)(uintptr_t)r[1];
    int32_t iVar3 = *(int32_t*)(thisp + 0x30);
    uint32_t* chunk = nullptr;
    uint32_t* piVar4;
    if (iVar3 < 1) {
        for (;;) {
            chunk = *(uint32_t**)(thisp + 0x40);
            if ((intptr_t)chunk >= 1 && *chunk != chunk[1]) break;
            chunk = storm_sub_2350(0, 0);
            if (chunk == nullptr) break;   // model: retail would fault on a failed pool alloc
            if (*chunk < chunk[1] + *(uint32_t*)(thisp + 0x34))
                storm_sub_2c60(chunk, *(uint32_t*)(thisp + 0x34));
            *(uint32_t*)(thisp + 0x34) <<= 1;
            *(uint32_t**)(thisp + 0x40) = chunk;   // model: the new chunk becomes current
        }
        if (chunk != nullptr) {
            iVar3 = (int32_t)storm_sub_26e0(chunk);
            if (iVar3 == 0) piVar4 = param_1 + 1;
            else piVar4 = (uint32_t*)(uintptr_t)(*param_1 + (uint32_t)iVar3);
        } else {
            piVar4 = param_1 + 1;
        }
    } else {
        piVar4 = (uint32_t*)(uintptr_t)(*param_1 + (uint32_t)iVar3);
    }
    if (*piVar4 != 0) storm_unlink(piVar4);
    uint32_t* piVar5 = param_1 + 1;
    *piVar4 = (uint32_t)(uintptr_t)piVar5;
    piVar4[1] = param_1[2];
    uint32_t uVar1 = param_1[2];
    uint32_t iVar6 = *param_1;
    if ((int32_t)uVar1 < 0) {
        *(uint32_t**)(uintptr_t)~uVar1 = piVar4;
        param_1[2] = (uint32_t)iVar3;
        r[4] = 0;
        return;
    }
    if ((int32_t)iVar6 < 0)
        iVar6 = (uint32_t)(uintptr_t)piVar5 - *(uint32_t*)((uintptr_t)*(uint32_t*)(uintptr_t)piVar5 + 4);
    *(uint32_t**)(uintptr_t)(uVar1 + iVar6) = piVar4;
    param_1[2] = (uint32_t)iVar3;
    r[4] = 0;
}

// 1.13c Storm+0x1990: install the base vtable (retail 0x6FC36AD4), drain the chunk lists
// (0x2080), unlink the two relative lists at +0x38 and +0x28 (0x22D0), then 0x1A70.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1990(uint32_t* r) {
    storm_sub_1990((uint32_t*)(uintptr_t)r[0]);
    r[4] = 0;
}

// 1.13c Storm+0x1A30: destroy the region record (0x1C30), then SMemFree it with tag AURGN.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1a30(uint32_t* r) {
    uint32_t* p = (uint32_t*)(uintptr_t)r[0];
    storm_sub_1c30(p);
    storm_mem_free(p, "__AURGN__", -2);
    r[4] = 0;
}

// 1.13c Storm+0x1A70: install the vtable (retail 0x6FC36B04), reset the table part (0x20C0
// with mode 1), drain the explicit list (0x2290), then unlink the node at +4 (0x22D0).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1a70(uint32_t* r) {
    storm_sub_1a70((uint32_t*)(uintptr_t)r[0]);
    r[4] = 0;
}

// 1.13c Storm+0x1AF0: reset the container: blank the table part (0x20C0 mode 1), clear +0x10
// and +0x24, drain the explicit list (0x2290), then clear +0x14/+0x18/+0x1C.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1af0(uint32_t* r) {
    uint32_t* p = (uint32_t*)(uintptr_t)r[0];
    storm_sub_20c0(p, 1);
    p[4] = 0;
    p[9] = 0;
    storm_sub_2290(p);
    p[5] = 0;
    p[6] = 0;
    p[7] = 0;
    r[4] = 0;
}

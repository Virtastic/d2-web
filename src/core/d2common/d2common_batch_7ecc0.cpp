// 1.13c D2Common batch 7ecc0: +0x7ECC0, +0x42920, +0x68CD0, +0x68F00, +0x814C0, +0x81650, +0x81AD0.
// Naming-level implementations from the cached decompile; not called by the engine, no vectors.
// GUESSED INPUTS: every register-passed argument (in_EAX/unaff_EBX/EDI/ESI) is given as an explicit
// parameter; struct types are unknown, so they are raw byte views at the retail 32-bit offsets.
// Pointer-width fields are read at retail offsets with native width (ponytail: wrong on wasm64 until
// the owning structs are typed; the arithmetic and flag logic is the pinned part).
// Unresolved callees (0x42A70, 0x7EB40, 0x36C10, 0x816C0, 0x68A90, 0x68B90, Ordinal_10051) and the
// 0x6FDDACA0 table / 0x6FDEB244 callback are taken as parameters.
#include <cstdint>
#include <cstring>

namespace {
using B = uint8_t;
template <class T> T rd(const void* p, int o) { T v; std::memcpy(&v, (const B*)p + o, sizeof v); return v; }
template <class T> void wr(void* p, int o, T v) { std::memcpy((B*)p + o, &v, sizeof v); }
int32_t at32(const void* base, int32_t idx) { return rd<int32_t>(base, idx * 4); }
}

// 1.13c D2Common+0x7ECC0: for each of n entries, two (tile,flag) lookups -> a mapped value, sent to
// the 0x42A70 setter with ids 0x1a / 0x1b. tbl = 8-byte-stride table at 0x6FDDACA0.
static int32_t Map7ECC0(int32_t kind, uint32_t flags, const int32_t* tbl, int lane)
{
    if (kind == 0) return (flags & 0x100) ? 0 : 3;
    if (kind == 7 && (flags & 0xf0000) == 0x30000) return 3;
    return tbl[kind * 2 + lane];
}
void D2Common_7ECC0(void* pObj, int32_t base, const int32_t* tbl,
                    void (*set42A70)(void*, int32_t, int32_t, int32_t, int32_t, int32_t),
                    void (*call7EB40)())
{
    void* p = rd<void*>(pObj, 0x14);
    int32_t n = rd<int32_t>(p, 0x60);
    for (int32_t i = 0; i < n; i++) {
        const void* a = rd<void*>(p, 0x2c); const void* b = rd<void*>(p, 0x30);
        const void* c = rd<void*>(p, 4);    const void* d = rd<void*>(p, 8);
        int32_t ia = at32(d, i), ib = at32(b, i);
        set42A70(pObj, base, i, 0x1a, Map7ECC0(at32(c, ia + base), at32(a, ib + base), tbl, 0), 0);
        set42A70(pObj, base + 1, i, 0x1b, Map7ECC0(at32(c, ia + base + 1), at32(a, ib + base + 1), tbl, 1), 0);
    }
    if (rd<B>(p, 0) & 0x14) call7EB40();
}

// 1.13c D2Common+0x42920: find-or-create a per-id round-robin node on the list at +0x1cc of pCtx
// (node: id, count, cursor, next), seed LCG at +0x1c4/+0x1c8, then advance cursor = (cursor+1)%count.
// pRow = the 0x1b0-byte table row (id at +0, count at +0x40); alloc = Ordinal_10045.
void D2Common_42920(void* pCtx, int32_t id, const void* pRow, int32_t* (*alloc)())
{
    int32_t* n = rd<int32_t*>(pCtx, 0x1cc);
    for (; n; n = (int32_t*)rd<void*>(n, 12)) if (n[0] == id) goto found;
    {
        n = alloc();
        n[0] = rd<int32_t>(pRow, 0); n[1] = rd<int32_t>(pRow, 0x40); n[2] = 0; n[3] = 0;
        uint32_t c = (uint32_t)n[1], r;
        if ((int32_t)c < 1) r = 0;
        else {
            uint64_t s = (uint64_t)rd<uint32_t>(pCtx, 0x1c4) * 0x6ac690c5ull + rd<uint32_t>(pCtx, 0x1c8);
            wr<uint64_t>(pCtx, 0x1c4, s);
            r = (c & (c - 1)) == 0 ? (c - 1) & (uint32_t)s : (uint32_t)((s & 0xffffffffull) % c);
        }
        n[2] = (int32_t)r;
        wr<void*>(n, 12, rd<void*>(pCtx, 0x1cc));
        wr<void*>(pCtx, 0x1cc, n);
    }
found:
    n[2] = (n[2] + 1) % n[1];
}

// Shared tail of 0x68CD0 / 0x68F00: apply the 'flags' word (unaff_EBX / param_5) to rec[5].
static void ApplyFlags(int32_t* r, uint32_t f, uint32_t (*ord10051)(int32_t))
{
    if ((int8_t)f < 0) r[5] |= 1;
    if (f & 0x10000000) r[5] |= 0x102;
    if (f & 0x20000)    r[5] |= 0x40;
    if (f & 0x10000)    r[5] |= 0x80;
    if (f & 8)          r[5] |= 4;
    r[5] = ((int32_t)f < 0) ? (r[5] | 8) : (r[5] & ~8);
    if (f & 0x4000000)  r[5] |= 0x20c;
    if (f & 0x20000000) r[5] |= 0x800;
    if (f & 4)          r[5] |= 0x2000;
    uint32_t q = ord10051(r[6]);
    if (q & 1) r[5] |= 4;
    if (q & 4) r[5] |= 0x800;
}
static void InitRec(int32_t* r, void* pRoom, int32_t** pHead, int32_t x, int32_t y, int32_t v6, int32_t v7)
{
    if (!pHead) r[8] = 0; else { r[8] = (int32_t)(intptr_t)*pHead; *pHead = r; }
    r[2] = x - rd<int32_t>(pRoom, 0x34);
    r[3] = y - rd<int32_t>(pRoom, 0x38);
    r[0] = (x - (y + 1)) * 0x50;
    r[1] = (((y + 1 + x) * 0x50) >> 1) + 0x28;
    r[6] = v6; r[7] = v7; r[5] = 0; r[9] = 0;
    wr<B>(r, 0x29, 0xff); wr<B>(r, 0x2a, 0xff); wr<B>(r, 0x28, 0xff);
}

// 1.13c D2Common+0x68CD0: allocate record #(+0x10) of the 0x30-byte array at (+0x24) in the pool at
// pRoom+0x54, link it, fill it with type 0xd and the flags word. Returns the record.
int32_t* D2Common_68CD0(void* pRoom, int32_t x, int32_t** pHead, int32_t y, int32_t v6,
                        uint32_t flags, uint32_t (*ord10051)(int32_t))
{
    void* pool = rd<void*>(pRoom, 0x54);
    int32_t* r = (int32_t*)(rd<B*>(pool, 0x24) + rd<int32_t>(pool, 0x10) * 0x30);
    wr<int32_t>(pool, 0x10, rd<int32_t>(pool, 0x10) + 1);
    InitRec(r, pRoom, pHead, x, y, v6, 0xd);
    ApplyFlags(r, flags, ord10051);
    return r;
}

// 1.13c D2Common+0x68F00: same for the array at (+0x14)/count (+8); calls 0x68A90(pRoom,v7); when
// v7==3 recurses with type 4 (v6 from 0x68B90(pRoom,4)) and OR-s ((flags>>18&3)*0x4000+0x4000) plus
// the shared flag logic into the child. Returns the outer record.
int32_t* D2Common_68F00(void* pRoom, int32_t** pHead, int32_t x, int32_t y, uint32_t flags,
                        int32_t v6, int32_t v7, void (*f68A90)(void*, int32_t),
                        uint32_t (*f68B90)(void*, int32_t), uint32_t (*ord10051)(int32_t))
{
    void* pool = rd<void*>(pRoom, 0x54);
    int32_t* r = (int32_t*)(rd<B*>(pool, 0x14) + rd<int32_t>(pool, 8) * 0x30);
    wr<int32_t>(pool, 8, rd<int32_t>(pool, 8) + 1);
    InitRec(r, pRoom, pHead, x, y, v6, v7);
    f68A90(pRoom, v7);
    if (v7 == 3) {
        int32_t* c = D2Common_68F00(pRoom, pHead, x, y, flags, (int32_t)f68B90(pRoom, 4), 4, f68A90, f68B90, ord10051);
        c[5] |= ((flags >> 18) & 3) * 0x4000 + 0x4000;
        ApplyFlags(c, flags, ord10051);
    }
    return r;
}

// 1.13c D2Common+0x814C0: walk pRoom's record array (+0x54 -> +0x14, count +0x18, stride 0x30) and
// call cb(rec) for records with (f&0x1c000)==0x4000, type(+0x1c)!=0xf, bit 11 clear; then for each
// neighbour room (array +8, count +0x2c) other than pRoom, over its list head (**(+0x54)) entries
// with kind==0, the same test on each chained record (next +0x20) whose absolute tile (neighbour
// +0x34/+0x38 plus rec +8/+0xc) lies inside pRoom's box (+0x34,+0x38,+0x3c,+0x40).
void D2Common_814C0(void* pRoom, void (*cb)(void*))
{
    auto hit = [](uint32_t f, int32_t t) { return (f & 0x1c000) == 0x4000 && t != 0xf && (~(f >> 11) & 1); };
    void* pool = rd<void*>(pRoom, 0x54);
    for (uint32_t i = 0; i < rd<uint32_t>(pool, 0x18); i++) {
        B* r = rd<B*>(pool, 0x14) + i * 0x30;
        if (hit(rd<uint32_t>(r, 0x14), rd<int32_t>(r, 0x1c))) cb(r);
    }
    for (uint32_t k = 0; k < rd<uint32_t>(pRoom, 0x2c); k++) {
        B* o = (B*)rd<void*>((B*)rd<void*>(pRoom, 8) + k * sizeof(void*), 0);
        if (!o || o == pRoom || !rd<void*>(o, 0x54)) continue;
        for (B* l = (B*)rd<void*>(rd<void*>(o, 0x54), 0); l; l = (B*)rd<void*>(l, 8)) {
            if (rd<int32_t>(l, 0) != 0) continue;
            for (B* r = (B*)rd<void*>(l, 4); r; r = (B*)rd<void*>(r, 0x20)) {
                if (!hit(rd<uint32_t>(r, 0x14), rd<int32_t>(r, 0x1c))) continue;
                int32_t y = rd<int32_t>(o, 0x38) + rd<int32_t>(r, 0xc), x = rd<int32_t>(o, 0x34) + rd<int32_t>(r, 8);
                if (rd<int32_t>(pRoom, 0x34) <= x && rd<int32_t>(pRoom, 0x38) <= y &&
                    x <= rd<int32_t>(pRoom, 0x3c) + rd<int32_t>(pRoom, 0x34) &&
                    y <= rd<int32_t>(pRoom, 0x40) + rd<int32_t>(pRoom, 0x38)) cb(r);
            }
        }
    }
}

// 1.13c D2Common+0x81650: cb(rec) for each record of pRoom's array with flag 4 set and (f&0x1c000)==0.
void D2Common_81650(void* pRoom, void (*cb)(void*))
{
    void* pool = rd<void*>(pRoom, 0x54);
    for (uint32_t i = 0; i < rd<uint32_t>(pool, 0x18); i++) {
        B* r = rd<B*>(pool, 0x14) + i * 0x30;
        uint32_t f = rd<uint32_t>(r, 0x14);
        if ((f & 4) && !(f & 0x1c000)) cb(r);
    }
}

// 1.13c D2Common+0x81AD0: for each neighbour room (array +8, count +0x2c, != pRoom, +100 non-null)
// whose box (+0x34,+0x38,+0x3c,+0x40) touches or overlaps pRoom's on both axes (gap < 1), call
// f816C0(x, other, pRoom) twice per column x along pRoom's top/bottom edge and twice per row y
// (with x = left, then right edge); loops are inclusive. Argument order as the decompile.
void D2Common_81AD0(void* pRoom, void (*f816C0)(int32_t, void*, void*))
{
    int32_t n = rd<int32_t>(pRoom, 0x2c);
    for (int32_t i = 0; i < n; i++) {
        B* o = (B*)rd<void*>((B*)rd<void*>(pRoom, 8) + (size_t)i * sizeof(void*), 0);
        if (o == pRoom || !rd<int32_t>(o, 100)) continue;
        int32_t px = rd<int32_t>(pRoom, 0x34), ox = rd<int32_t>(o, 0x34);
        int32_t py = rd<int32_t>(pRoom, 0x38), oy = rd<int32_t>(o, 0x38);
        int32_t gx = px < ox ? ox - rd<int32_t>(pRoom, 0x3c) - px : px - rd<int32_t>(o, 0x3c) - ox;
        int32_t gy = py < oy ? oy - rd<int32_t>(pRoom, 0x40) - py : py - rd<int32_t>(o, 0x40) - oy;
        if (gx >= 1 || gy >= 1) continue;
        for (int32_t x = px; x <= rd<int32_t>(pRoom, 0x3c) + px; x++) { f816C0(x, o, pRoom); f816C0(x, o, pRoom); }
        for (int32_t y = py; y <= rd<int32_t>(pRoom, 0x40) + py; y++) {
            f816C0(px, o, pRoom); f816C0(rd<int32_t>(pRoom, 0x3c) + px, o, pRoom);
        }
    }
}

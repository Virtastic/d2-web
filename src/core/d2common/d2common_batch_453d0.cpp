// 1.13c D2Common batch: +0x453D0, +0x3DC90, +0x84AD0. Written from the 1.13c decompile with
// retail 32-bit offsets (documented in each block); callees are named by RVA, declared here.
// Registers the decompile shows as implicit (EAX/EDI) are explicit parameters.
#include <cstdint>

struct D2B453 { uint8_t* base; };
static inline int32_t& I32(void* p, uint32_t off) { return *(int32_t*)((uint8_t*)p + off); }
static inline void* P32(void* p, uint32_t off) { return (void*)(uintptr_t)(uint32_t)I32(p, off); }

// Callees (RVA names).
int32_t* D2Common_44CE0(void* ctx, int32_t id);
void*    D2Common_2D9B0(void* ctx, int32_t id);
void     D2Common_2E360(void* p);
int32_t* D2Common_19950(void* ctx, int32_t id);
int      D2Common_452C0(uint8_t slot, void* unit, int32_t a, uint32_t idx, int32_t v);
void     D2Common_36CD0(void* p, int32_t a, int32_t b, int32_t c);
void*    D2Common_6C4D0(uint32_t a, int32_t idx);
void     D2Common_83CA0(void* ctx, void* e);
void     D2Common_84500(void* self, void* e);
void     D2Common_84890(void* self, void* e);
void     D2Common_fatal_10024(const void* msg, uint32_t code);

// 1.13c D2Common+0x453D0: thiscall(this, param_1, unit, idx). unit+0x58 = data, data+0x1D0 = key,
// data+0x1B4 = list owner (+0x47C list head, node next at dword 0x6B, node key at dword 0x74).
// Tries the idx-th slot whose table entry equals key first, then falls back to the first that works.
void D2Common_453D0(void* self, int32_t p1, void* unit, uint32_t idx)
{
    if (!unit || (uint8_t)idx > 7) { D2Common_fatal_10024(nullptr, 0x350); return; }
    void* data = P32(unit, 0x58);
    int32_t key = I32(data, 0x1D0);
    int32_t* t = D2Common_44CE0(self, key);
    void* owner = P32(data, 0x1B4);
    uint32_t i = idx & 0xFF;
    int32_t k2 = t[i];
    int32_t* node = (int32_t*)P32(owner, 0x47C);
    for (; node; node = (int32_t*)P32(node, 0x6B * 4))
        if (node[0x74] == k2) break;
    if (!node) node = (int32_t*)D2Common_2D9B0(owner, k2);
    int32_t* t2 = D2Common_44CE0(owner, k2);
    int32_t* t3 = D2Common_19950(owner, I32(data, 0x1D0));
    int32_t v = t3[i];
    if (node[4] == 0) D2Common_2E360(node);
    if (v != -1) {
        int32_t cnt = 0, seen = 0;
        int32_t* q = t;
        for (uint32_t n = (uint8_t)idx ? i : 0; n; --n) { if (*q == k2) ++cnt; ++q; }
        for (uint32_t s = 0; s < 8; ++s)
            if (t2[s] == key) {
                if (cnt == seen) { if (D2Common_452C0((uint8_t)s, unit, p1, idx, v)) return; break; }
                ++seen;
            }
    }
    for (uint32_t s = 0; s < 8; ++s)
        if (t2[s] == key && D2Common_452C0((uint8_t)s, unit, p1, idx, v)) return;
}

// 1.13c D2Common+0x3DC90: fastcall(a), EAX=b. Walks a+0x14 -> table of lists at +0x68 (count at
// +0x260); for every node with a non-null next (+0x10) calls 36CD0(b+0x20 -> +0x3C, 1, 0, 2).
void D2Common_3DC90(void* a, void* b)
{
    uint8_t* tgt = (uint8_t*)P32(b, 0x20);
    void* t = P32(a, 0x14);
    for (int32_t i = 0; i < I32(t, 0x260); ++i)
        for (void* n = P32(t, 0x68 + 4 * i); n; n = P32(n, 0x10))
            if (I32(n, 0x10)) D2Common_36CD0(tgt + 0x3C, 1, 0, 2);
}

// 1.13c D2Common+0x84AD0: fastcall(a), EDI=self. self[0xC] = entry index (-1 none), self[0xE] =
// bitmask; entries are 0x15C apart, one per set bit. self[0] -> +0x58 -> +0x1B4 -> +0x464 is ctx.
void D2Common_84AD0(uint32_t a, int32_t* self)
{
    if (self[0xC] == -1) return;
    uint8_t* e = (uint8_t*)D2Common_6C4D0(a, self[0xC]);
    void* ctx = P32(P32(P32(self, 0), 0x58), 0x1B4);
    ctx = P32(ctx, 0x464);
    for (uint32_t m = (uint32_t)self[0xE]; m; m >>= 1, e += 0x15C)
        if (m & 1) {
            D2Common_83CA0(ctx, e);
            if (I32(e, 0x40) == 0) D2Common_84500(self, e); else D2Common_84890(self, e);
        }
}

// Batch b66 (D2Common). Only real-logic row: 0x10EB0. The other rows of the batch are MSVC CRT /
// SEH / FPU-fixup code (0x58C0 0x62AA 0x6DB8 0x6F53 0x70E7 0x8221 0x8A1D 0x8C14 0x86337 0x868C8)
// and are skipped by instruction.
#include <cstdint>
#include <initializer_list>

typedef uint8_t* P;
template <class T> static inline T& F(void* p, int off) { return *(T*)((P)p + off); }
#define I(p, o) F<int32_t>(p, o)
#define PT(p, o) F<void*>(p, o)

extern "C++" {
void* sub_7D9B0(void* p, int id); void* sub_F000(void*, int, int); void sub_E490(int);
void sub_44F20(); void sub_2D3C0(int, int*, void*);
void sub_106B0(uint32_t, const int32_t* tbl, void* ctx, uint32_t* out);
extern void* (*const PTR_FUN_6fde6178[])(); // per-variant head-node getter
extern const uint8_t DAT_6fde6138[], DAT_6fde60f8[]; // 0x10-byte entries
}

// 1.13c D2Common+0x10EB0
void sub_10EB0(void* g) {
    void* u = PT(g, 0x1b4);
    void* s = nullptr;
    for (void* p = PT(u, 0x47c); p; p = PT(p, 0x6b * 4)) if (I(p, 0x74 * 4) == 0x1b) { s = p; break; }
    if (!s) s = sub_7D9B0(u, 0x1b);
    uint32_t v = (uint32_t)I(PT(s, 5 * 4), 4), idx = v;
    void* n = PTR_FUN_6fde6178[v]();
    void* lv = PT(g, 0x14);
    int dx = I(s, 7 * 4), dy = I(s, 8 * 4);
    if (v <= 2) {
        static const int cnt[3] = {2, 3, 0};
        n = sub_F000(n, cnt[v], 0);
        if (n) { sub_E490(1); void* q = PT(n, 0x20); I(q, 0) = 0xa7; I(q, 4) = (int)v; I(q, 12) |= 2; }
        sub_44F20();
        if (v == 0) {
            dx -= I(n, 0xd * 4) + I(lv, 0x10);
            dy += I(s, 10 * 4) / 2 - I(n, 0xe * 4);
        } else if (v == 1) {
            dx += -6 + I(s, 9 * 4) / 2 - I(n, 0xd * 4);
            dy -= I(n, 0xe * 4) + I(lv, 0x14);
        } else {
            dx += I(s, 9 * 4) - I(n, 0xd * 4);
            dy += 1 + I(s, 10 * 4) / 2 - I(n, 0xe * 4);
        }
    } else {
        sub_44F20();
    }
    uint64_t r = (uint64_t)(uint32_t)I(g, 0x1c4) * 0x6ac690c5ull + (uint32_t)I(g, 0x1c8);
    F<uint64_t>(g, 0x1c4) = r;
    uint32_t sel = idx;
    if ((r & 1) == 0) {
        sub_106B0(0, (const int32_t*)(DAT_6fde6138 + idx * 0x10), g, &sel);
        sub_106B0(0, (const int32_t*)(DAT_6fde60f8 + sel * 0x10), g, &sel);
    } else {
        sub_106B0((uint32_t)I(g, 0x1c8), (const int32_t*)(DAT_6fde60f8 + idx * 0x10), g, &sel);
        sub_106B0(0, (const int32_t*)(DAT_6fde6138 + sel * 0x10), g, &sel);
    }
    for (void* rm = PT(g, 0x10); rm; rm = PT(rm, 0x24)) { I(rm, 0x34) += dx; I(rm, 0x38) += dy; }
    int o[4] = {0, 0, 0, 0}; // retail stack order: o[3]=param_1 (min x), o[2]=min y, o[1]=max x, o[0]=max y
    sub_2D3C0(0, o, g);
    I(g, 0x1c) = o[3]; I(g, 0x20) = o[2]; I(g, 0x24) = o[1] - o[3]; I(g, 0x28) = o[0] - o[2];
}

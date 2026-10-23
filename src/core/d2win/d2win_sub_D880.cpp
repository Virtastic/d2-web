// src/core/d2win/d2win_sub_D880.cpp -- 1.13c D2Win+0xD880 (sub_D880, 1879 B): one-shot init of the item
// component/type table (D2Comp). Callees and retail globals are seams in the env; hidden-register inputs of
// 0xB4F0/0xC810 are explicit (unverified, ).
#include <cstdint>
#include <cstring>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct D2WinD880Col { const void* name; uint32_t type, a, off; const void* extra; };

struct D2WinD880Env {
    int32_t* once;            // 6f9aa67c
    uint32_t* tbl;            // 6f9a8cb0, 0x2fd dwords, 12-byte entries [id, v1, v2]
    uint32_t* tbl2;           // 6f9a98a8, one dword per entry
    uint32_t* cnt;            // 6f9a98a4
    void** h8ca4; void** t8ca8; int32_t* c8cac; void** h8c9c; void** t8c98;
    int32_t srcCount;         // 6f9a8c68
    const uint8_t* src;       // 6f9a8c6c, 0x1a8-byte records
    uint32_t pairCount;       // 6f900c6c
    const uint32_t* pairs;    // 6f900c70, (key,index) pairs
    const uint32_t* vals;     // 6f900c30
    const void *s8fcd90, *s8fcce8, *s8fcc94, *s8fcd2c;
    void* (*alloc)(const char* file, int32_t line);                         // Ordinal_10211
    void* (*loadTable)(const D2WinD880Col* cols, int32_t* outCount);        // 0xC810
    void (*finish)(void);                                                   // 0xCBA0
    uint32_t (*test)(int32_t k, const uint8_t* rec, int32_t idx);           // 0xB4F0
};

extern "C" EMSCRIPTEN_KEEPALIVE
// 1.13c D2Win+0xD880
void d2win_sub_D880(const D2WinD880Env* e) {
    if (*e->once) return;
    memset(e->tbl, 0, 0x2fd * 4);
    *e->once = 1;
    void *p = e->h8ca4, *q = e->h8c9c;
    const D2WinD880Col t1[] = {
        {e->s8fcd90, 10, 0, 0, p}, {"equiv1", 0xf, 0, 4, p}, {"equiv2", 0xf, 0, 6, p},
        {"repair", 4, 0, 8, 0}, {e->s8fcce8, 4, 0, 9, 0}, {"shoots", 0xf, 0, 0xc, p},
        {"quiver", 0xf, 0, 0xe, p}, {"throwable", 4, 0, 0x10, 0}, {"reload", 4, 0, 0x11, 0},
        {"reequip", 4, 0, 0x12, 0}, {"autostack", 4, 0, 0x13, 0}, {"magic", 4, 0, 0x14, 0},
        {"normal", 4, 0, 0x16, 0}, {"charm", 4, 0, 0x17, 0}, {e->s8fcc94, 4, 0, 0x18, 0},
        {"beltable", 4, 0, 0x19, 0}, {"costformula", 4, 0, 0x20, 0}, {"varinvgfx", 4, 0, 0x23, 0},
        {"invgfx1", 1, 0x1f, 0x24, 0}, {"invgfx2", 1, 0x1f, 0x44, 0}, {"invgfx3", 1, 0x1f, 0x64, 0},
        {"invgfx4", 1, 0x1f, 0x84, 0}, {"invgfx5", 1, 0x1f, 0xa4, 0}, {e->s8fcd2c, 0, 0, 0, 0}};
    const D2WinD880Col t2[] = {{e->s8fcd90, 10, 0, 0, q}, {e->s8fcd2c, 0, 0, 0, 0}};
    *e->h8ca4 = e->alloc("..\\Source\\D2Win\\Src\\D2Comp.cpp", 0xac0);
    *e->t8ca8 = e->loadTable(t1, e->c8cac);
    *e->h8c9c = e->alloc("..\\Source\\D2Win\\Src\\D2Comp.cpp", 0xacc);
    *e->t8c98 = e->loadTable(t2, nullptr);
    e->finish();
    uint32_t* T = e->tbl;
    T[4] = e->vals[0]; T[7] = e->vals[0]; T[10] = e->vals[0];
    T[3] = 0x2074696c; T[5] = 3; T[6] = 0x2064656d; T[8] = 3; T[9] = 0x20797668; T[11] = 3;
    int32_t n = 4, m = 4, off = 0x30;   // iVar6, iVar10, iVar11
    auto lookup = [&](int32_t key) -> uint32_t {
        for (uint32_t i = 0; i < e->pairCount; i++)
            if ((uint32_t)key == e->pairs[i * 2]) return e->vals[e->pairs[i * 2 + 1]];
        return e->vals[0];
    };
    for (int32_t s = 0; s < e->srcCount; s++) {
        const uint8_t* r = e->src + (size_t)s * 0x1a8;
        int32_t id = *(const int32_t*)(r + 0x90);
        if (id == 0) id = *(const int32_t*)(r + 0x80);
        bool ok = true;
        if ((!e->test(0, r, 0) && !e->test(1, r, 0) && !e->test(2, r, 0) && !e->test(3, r, 0)) || e->test(4, r, 0))
            ok = false;
        bool found = false;
        for (int32_t j = 0; j < n; j++) if ((int32_t)T[j * 3] == id) { found = true; break; }
        if (found || !ok) continue;
        int32_t i = off / 12;   // entry index iVar4 starts at byte offset iVar11
        for (;; i++, n++)
            if (!(((e->test(5, r, i) && e->test(6, r, i)) || (e->test(7, r, i) && e->test(8, r, i))) || T[i * 3] != 0)) break;
        i = n;
        if (n > 0xfe) i = m;
        T[i * 3] = (uint32_t)id;
        T[i * 3 + 2] = (uint32_t)(int32_t)*(const int16_t*)(r + 0x11e);
        T[i * 3 + 1] = lookup(*(const int32_t*)(r + 0xc0));
        e->tbl2[i] = lookup(*(const int32_t*)(r + 0xc4));
        n = m;
        if (m == i) { n = m + 1; off += 0xc; m = n; }
    }
    *e->cnt = 0xff;
}

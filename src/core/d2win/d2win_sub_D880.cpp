// src/core/d2win/d2win_sub_D880.cpp -- 1.13c D2Win+0xD880 (sub_D880, 1879 B): one-shot init of the item-type component
// table (D2Comp.cpp). The retail globals are slots of the env (32-bit words, as retail stores them), the callees are
// injectable: the allocator (0x76C6, D2Win's thunk to a Fog import), the txt loader (0xC810), the follow-up pass
// (0xCBA0) and the is-a test (0xB4F0, EAX = type, EDI = target, over the 0xE4-byte equivalence records the first
// loader call returns).
#include <cstdint>
#include <cstring>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

// one column of the loader's descriptor table (five dwords in retail)
struct D2WinD880Col { const char* name; uint32_t type, len, off; const uint32_t* dest; };

struct D2WinD880Env {
    uint32_t* once;           // 0x6F9AA67C
    uint32_t* tbl;            // 0x6F9A8CB0, 0xFF entries of {code, value, type}
    uint32_t* tbl2;           // 0x6F9A98A8, one dword per entry
    uint32_t* end;            // 0x6F9A98A4 (the word after tbl)
    uint32_t *h8ca4, *t8ca8, *c8cac, *h8c9c, *t8c98;
    const uint32_t* srcCount; // 0x6F9A8C68
    const uint8_t* src;       // *0x6F9A8C6C, 0x1A8-byte records
    const uint32_t* cls;      // 0x6F8FFEE8, 12-byte static entries, the first dword of each a type
    const uint32_t* vals;     // 0x6F900C30
    const uint32_t* pairCount;// 0x6F900C6C
    const uint32_t* pairs;    // 0x6F900C70, {code, index}
    uint32_t (*alloc)(const char* file, uint32_t line);                                                       // 0x76C6
    uint32_t (*loadTable)(const char* name, uint32_t ecx, const D2WinD880Col* cols, uint32_t* outCount, uint32_t n); // 0xC810
    void (*finish)(uint32_t eax);                                                                             // 0xCBA0
    uint32_t (*isA)(int32_t type, int32_t target);                                                           // 0xB4F0
};

namespace {
uint32_t d880_lookup(const D2WinD880Env* e, uint32_t key) {
    const uint32_t n = *e->pairCount;
    for (uint32_t i = 0; i < n; i++)
        if (key == e->pairs[i * 2]) return e->vals[e->pairs[i * 2 + 1]];
    return e->vals[0];
}
}  // namespace

// 1.13c D2Win+0xD880. Returns what retail leaves in EAX: the flag when it was already set, vals[0] for no
// records, else the record count.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2win_sub_D880(const D2WinD880Env* e) {
    if (*e->once) return *e->once;
    memset(e->tbl, 0, 0x2fd * 4);
    *e->once = 1;
    static const char kFile[] = "..\\Source\\D2Win\\Src\\D2Comp.cpp";
    const D2WinD880Col t1[] = {
        {"code", 10, 0, 0, e->h8ca4}, {"equiv1", 0xf, 0, 4, e->h8ca4}, {"equiv2", 0xf, 0, 6, e->h8ca4},
        {"repair", 4, 0, 8, nullptr}, {"body", 4, 0, 9, nullptr}, {"shoots", 0xf, 0, 0xc, e->h8ca4},
        {"quiver", 0xf, 0, 0xe, e->h8ca4}, {"throwable", 4, 0, 0x10, nullptr}, {"reload", 4, 0, 0x11, nullptr},
        {"reequip", 4, 0, 0x12, nullptr}, {"autostack", 4, 0, 0x13, nullptr}, {"magic", 4, 0, 0x14, nullptr},
        {"normal", 4, 0, 0x16, nullptr}, {"charm", 4, 0, 0x17, nullptr}, {"gem", 4, 0, 0x18, nullptr},
        {"beltable", 4, 0, 0x19, nullptr}, {"costformula", 4, 0, 0x20, nullptr}, {"varinvgfx", 4, 0, 0x23, nullptr},
        {"invgfx1", 1, 0x1f, 0x24, nullptr}, {"invgfx2", 1, 0x1f, 0x44, nullptr}, {"invgfx3", 1, 0x1f, 0x64, nullptr},
        {"invgfx4", 1, 0x1f, 0x84, nullptr}, {"invgfx5", 1, 0x1f, 0xa4, nullptr}, {"end", 0, 0, 0, nullptr}};
    const D2WinD880Col t2[] = {{"code", 10, 0, 0, e->h8c9c}, {"end", 0, 0, 0, nullptr}};
    *e->h8ca4 = e->alloc(kFile, 0xac0);
    *e->t8ca8 = e->loadTable("itemtypes", 0, t1, e->c8cac, 0xe4);
    *e->h8c9c = e->alloc(kFile, 0xacc);
    *e->t8c98 = e->loadTable("hitclass", 0, t2, nullptr, 4);
    e->finish(0);
    uint32_t* T = e->tbl;
    T[4] = T[7] = T[10] = e->vals[0];
    T[3] = 0x2074696c; T[5] = 3; T[6] = 0x2064656d; T[8] = 3; T[9] = 0x20797668; T[11] = 3;   // "lit " "med " "hvy "
    uint32_t m = 4, off = 0x30, eax = e->vals[0];
    for (int32_t s = 0; s < (int32_t)*e->srcCount; s++) {
        const uint8_t* r = e->src + (uint32_t)s * 0x1a8;
        uint32_t id = *(const uint32_t*)(r + 0x90);
        if (id == 0) id = *(const uint32_t*)(r + 0x80);
        const int32_t t = *(const int16_t*)(r + 0x11e);
        const bool ok = (e->isA(t, 0x2d) || e->isA(t, 3) || e->isA(t, 0x33) || e->isA(t, 0x25)) && !e->isA(t, 0x4b);
        bool found = false;
        if ((int32_t)off > 0)
            for (uint32_t j = 0; j < m; j++) if (T[j * 3] == id) { found = true; break; }
        if (!found && ok) {
            uint32_t i = m, b = off;
            for (;;) {
                const int32_t c = (int32_t)e->cls[b / 4];
                if (e->isA(c, 0x2d) && e->isA(t, 0x2d)) { i++; b += 12; continue; }
                if (e->isA(c, 0x32) && e->isA(t, 0x32)) { i++; b += 12; continue; }
                if (T[b / 4] != 0) { i++; b += 12; continue; }
                break;
            }
            if (i >= 0xff) i = m;
            T[i * 3] = id;
            T[i * 3 + 2] = (uint32_t)t;
            T[i * 3 + 1] = d880_lookup(e, *(const uint32_t*)(r + 0xc0));
            e->tbl2[i] = d880_lookup(e, *(const uint32_t*)(r + 0xc4));
            if (m == i) { m++; off += 12; }
        }
        eax = (uint32_t)s + 1;
    }
    *e->end = 0xff;
    return eax;
}

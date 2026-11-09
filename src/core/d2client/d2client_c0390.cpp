// 1.13c D2Client+0xC0390: ECX = unit (may be null), EDI = hostile-flag out, stack dst
// (Unicode name buffer), pct (int out); ret 8. Finds the unit's id (+0x0C, -1 without a unit) in the list at 0x11C4D4
// (+0x08 id, +0x0C roster id, +0x30 next); with a roster id and a unit whose +0x10 mode is neither 0 nor 0x0C it copies
// the roster record's name (+0x66, 0x80 chars) into dst, stores the life percent (0x9D670 << 7) / 100 and sets *EDI to
// 0x9DC60(player 0x11BBFC id, roster id) != 0. Returns 1 when the roster entry exists, 0 otherwise.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cC0390Env {
    uint8_t* (*roster)(uint32_t id);                             // 0x9D630 (EDX = id)
    uint32_t (*life)(uint32_t id);                               // 0x9D670 (EDX = id)
    uint32_t (*hostile)(uint32_t a, uint32_t b);                 // 0x9DC60 (EDX = a, stack b; ret 4)
    void (*toUnicode)(uint16_t* dst, const char* src, int n);    // D2Lang Unicode::toUnicode
    uintptr_t* list;                                             // 0x11C4D4
    uintptr_t* player;                                           // 0x11BBFC
};
static uint32_t rd(uintptr_t p, uint32_t o) { uint32_t v; memcpy(&v, (const void*)(p + o), 4); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_c0390(const D2cC0390Env* e, const uint8_t* unit, uint32_t* hostileOut, uint16_t* dst, int32_t* pct) {
    uint32_t id = unit ? rd((uintptr_t)unit, 0xc) : 0xFFFFFFFFu;
    uintptr_t n = *e->list;
    if (!n) return 0;
    while (rd(n, 8) != id) { n = rd(n, 0x30); if (!n) return 0; }
    uint32_t rid = rd(n, 0xc);
    if (rid == 0xFFFFFFFFu) return 0;
    if (!unit) return 1;
    uint32_t mode = rd((uintptr_t)unit, 0x10);
    if (mode == 0 || mode == 0xc) return 1;
    uint8_t* r = e->roster(rid);
    if (!r) return 0;
    e->toUnicode(dst, (const char*)(r + 0x66), 0x80);
    uint32_t l = e->life(rid);
    *pct = (int32_t)(l << 7) / 100;
    *hostileOut = 0;
    uintptr_t p = *e->player;
    uint32_t pid = p ? rd(p, 0xc) : 0xFFFFFFFFu;
    if (e->hostile(pid, rid)) *hostileOut = 1;
    return 1;
}

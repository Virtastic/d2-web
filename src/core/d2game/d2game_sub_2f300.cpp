// D2Game OBJECTS_InitHandler, over retail-layout (32-bit) blobs.
// Convention (disassembly): ESI = object unit, EBX = game, stack: unitId, room, x, y; ret 0x10.
// The first call pushes ESI for the stdcall objects.txt lookup (D2Common 10296), so the word stored at unit+0xC
// is the first stack argument (the unit id), not a return address. Callees are Env seams.
// Fatal paths: Ordinal 10265(code) -> 10024(text) -> _exit(-1).
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint32_t r32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void w32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2GameSub2F300Env {
    void*    (*ptr)(uint32_t v);                          // a 32-bit struct field -> host pointer
    uint8_t* (*objTxt)(void* unit);                       // 0xB156: D2Common 10296 (stdcall unit)
    uint32_t (*ord10265)(uint32_t code);
    void     (*ord10024)(uint32_t code);
    void     (*deleteTimers)(void* game, void* unit);     // 0x8E880: EAX = game, stack unit
    const uint32_t* initTable;                            // DAT_6fd2e050: 0x50 init-function words (0 = none)
    int      (*isBadCodePtr)(uint32_t fn);
    void     (*callInit)(uint32_t fn, uint32_t* block);   // ECX = the 7-word init block
    uint32_t (*roll)(void* seed, int32_t n);              // 0x1180: EAX = n, ECX = seed
    void     (*changeMode)(void* unit, uint32_t mode);    // 0xA4A2: D2Common 11090 (stdcall unit, mode)
    uint32_t (*allocTimerArg)(void* game);                // 0x2E7D0: EDI = game
    void     (*setTimerArg)(void* unit, uint32_t arg);    // 0xB2D0: D2Common 10169 (ECX unit, EDX arg)
};

[[noreturn]] static void fatal(const D2GameSub2F300Env* e, uint32_t code) {
    e->ord10024(e->ord10265(code));
    _exit(-1);
}

// 1.13c D2Game+0x2F300
void D2Game_sub_2F300(const D2GameSub2F300Env* e, void* unit, void* game, uint32_t unitId, uint32_t room, uint32_t x, uint32_t y) {
    uint8_t* t = e->objTxt(unit);
    if (!unit) fatal(e, 0xa48);
    uint8_t* u = (uint8_t*)unit;
    const uint32_t mode = r32(u, 0x10);
    const uint32_t f0 = r32(u, 0xc4);
    uint32_t f = f0 & ~8u;
    w32(u, 0xc, unitId); u[0x78] = 0; w32(u, 0xc4, f); w32(u, 0xa4, 0);
    if ((int32_t)mode >= 8) fatal(e, 0x240);
    f = t[0x128] ? (f | 4) : (f0 & 0xfffffff3);
    w32(u, 0xc4, f);
    ((uint8_t*)e->ptr(r32(u, 0x14)))[4] = 0;                  // pObjectData->InteractType, no null check
    const uint16_t cls = (uint16_t)r32(u, 4);
    if (r32(u, 0xdc)) e->deleteTimers(game, u);
    w32(u, 0xdc, 0);
    if (cls >= 0x23d) fatal(e, 0x260);
    const uint8_t k = t[0x1b1];
    if (k >= 0x50) fatal(e, 0x262);
    const uint32_t ctrl = r32(game, 0x10f0);
    if (const uint32_t fn = e->initTable[k]) {
        if (e->isBadCodePtr(fn)) fatal(e, 0x26a);
        uint32_t blk[7] = { (uint32_t)(uintptr_t)game, (uint32_t)(uintptr_t)unit, room, ctrl, (uint32_t)(uintptr_t)t, x, y };
        e->callInit(fn, blk);
    }
    f = r32(u, 0xc4);
    w32(u, 0xc4, t[mode + 0xc4] ? (f | 2) : (f & ~2u));
    if (t[0x13d] && !((r32(u, 0xc4) >> 7) & 1)) {
        if (e->roll(e->ptr(ctrl), 14) == 0) e->changeMode(u, 2);
    }
    e->setTimerArg(u, e->allocTimerArg(game));
}

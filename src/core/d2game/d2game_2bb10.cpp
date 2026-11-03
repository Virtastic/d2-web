// D2Game+0x2BB10 (stdcall ret 0x20, EBX = register input, eight stack words a0..a7; a6/a7 are not read): the game-list "client joined" step.
// Order (disassembled): list count (0x6FD31C24) == 0 returns; the game-list lock is entered (0xA328, edx 0x125) and left (0xF8D00) around the slot read
// slots[a0 & 0xFFFF] (0 and -1 are "no game"); no game returns; g = 0x4AAE0(id) (null returns); Ordinal 10021(ebx, id) (0xA38E);
// cl = 0x6A9B0(eax = ebx, g, a1, a2, a3, a4, a5). No client: the callback block (0x6FD31C3C) function at +0x20, when set, is checked with IsBadCodePtr
// (bad = fatal 0x769) and called (ecx = g + 0x20, edx = a2, stack a3); then 0x49BA0(g), Ordinal 10002(0, ebx, &local, 1) (0xA35E), Ordinal 10000(ebx) (0xA37C).
// A client: 0x4AC60(g), cl[0x1AC] = the byte at 0x6FD31C30, 0x49BA0(g); the +0x08 callback, when set (bad = fatal 0x795), is called (ecx = cl + 0x68, edx = a2,
// stack ebx, a3) and 0x49D40(cl) follows; with no callback 0x8A3E0(eax = cl, &local, 1) runs.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game2BB10Env {
    void     (*enterWrap)(uint32_t line);
    uint32_t (*leaveCs)(uint32_t cs);
    uint32_t (*lockGame)(uint32_t id);
    void     (*notify)(uint32_t ebx, uint32_t id);
    uint32_t (*findClient)(uint32_t ebx, uint32_t game, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
    uint32_t (*isBad)(uint32_t fn);
    void     (*callCb)(uint32_t fn, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t nst);
    void     (*unlock)(uint32_t game);
    void     (*sendPacket)(uint32_t zero, uint32_t ebx, void* local, uint32_t one);
    void     (*fn10000)(uint32_t ebx);
    void     (*bindClient)(uint32_t game);
    void     (*after)(uint32_t client);
    void     (*append)(uint32_t client, void* local, uint32_t n);
    void     (*fatal)(uint32_t line);
    uint32_t listCount;
    const uint32_t* slots;   // 0x6FD30C04
    uint32_t cb;             // 0x6FD31C3C: the callback block address (0 = none)
    uint8_t  cbByte;         // 0x6FD31C30
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x2BB10
extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_2BB10(uint32_t ebx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, const D2Game2BB10Env* e)
{
    if (!e->listCount) return;
    e->enterWrap(0x125);
    uint32_t id = e->slots[a0 & 0xffff];
    if (id == 0 || id == 0xffffffffu) id = 0;
    e->leaveCs(0x6fd30be8u);
    if (!id) return;
    const uint32_t g = e->lockGame(id);
    if (!g) return;
    e->notify(ebx, id);
    const uint32_t cl = e->findClient(ebx, g, a1, a2, a3, a4, a5);
    uint8_t local[4] = { 0, 0, 0, 0 };
    if (!cl) {
        const uint32_t fn = e->cb ? rd32(e->cb + 0x20) : 0;
        if (fn) {
            if (e->isBad(fn)) e->fatal(0x769);
            e->callCb(fn, g + 0x20, a2, a3, 0, 1);
        }
        e->unlock(g);
        local[0] = 0xb0;
        e->sendPacket(0, ebx, local, 1);
        e->fn10000(ebx);
        return;
    }
    e->bindClient(g);
    *(uint8_t*)(uintptr_t)(cl + 0x1ac) = e->cbByte;
    e->unlock(g);
    const uint32_t fn = e->cb ? rd32(e->cb + 0x08) : 0;
    if (fn) {
        if (e->isBad(fn)) e->fatal(0x795);
        e->callCb(fn, cl + 0x68, a2, ebx, a3, 2);
        e->after(cl);
        return;
    }
    local[0] = 2;
    e->append(cl, local, 1);
}

// 1.13c Fog+0x1E550: D2QSSocket handle release. Retail: ESI = socket object; the object keeps retail's 32-bit field layout.
// If sock+0x110 holds a record whose +0 flag and +8 handle are both set, take the record's critical section (+0xC), free the
// handle, clear +8 and +0, and leave the section. The section calls and the handle free are the injected seams.
#include <cstdint>
#include <cstring>

struct Fog1E550Env {
    void (*csPrep)(void* cs, uint32_t line);   // Fog+0x11690 (validator, EDX = 0x15F)
    void (*csEnter)(void* cs);                 // EnterCriticalSection
    void (*freeHandle)(uint32_t handle);       // Fog+0xCD82 (import thunk, stdcall one word)
    void (*csLeave)(void* cs);                 // LeaveCriticalSection
};

static inline uint32_t rd(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void wr(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }

extern "C" void fog_1e550(uint8_t* sock, const Fog1E550Env* e) {  // 1.13c Fog+0x1E550
    if (!sock) return;
    uint8_t* c = (uint8_t*)(uintptr_t)rd(sock + 0x110);
    if (!c || !rd(c) || !rd(c + 8)) return;
    void* cs = c + 0xc;
    e->csPrep(cs, 0x15f);
    e->csEnter(cs);
    c = (uint8_t*)(uintptr_t)rd(sock + 0x110);
    e->freeHandle(rd(c + 8));
    wr(c + 8, 0);
    wr(c, 0);
    e->csLeave(c + 0xc);
}

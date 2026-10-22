// 1.13c D2Game+0xE8FE0 -- quest-NPC event handler (ECX=self quest record, EDX=event). Transcribed from the cached
// decompile and resolved against the 1.13c disassembly (Ghidra lost the register args). Image-handle pointers
// as in d2game_batch_e6850.cpp. Callee register conventions are in the env comments.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(uintptr_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(uintptr_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint8_t  rd8(uintptr_t p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void     wr8(uintptr_t p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }

struct D2GameE8FE0Env {
    uintptr_t (*p)(uint32_t handle);                               // image handle -> host pointer
    uint32_t  (*addr)(uint32_t rva);                               // image handle of D2Game+rva
    uintptr_t (*getPlayerData)(uintptr_t player);                  // 0xA4C0, EAX=player
    uint32_t  (*questBit)(uint32_t quest, uint32_t q, uint32_t b); // 0xA568, stack (quest,q,bit)
    void      (*setState)(uintptr_t self, uint32_t st);            // 0x55EB0, ESI=self EAX=state
    void      (*fire)(uint32_t cbRva, uintptr_t unit, uint32_t arg); // 0xDFCC0, EAX=0 EBX=cb EDI=unit, stack arg
    void      (*sub55910)(uint32_t eax, uint32_t ecx, uint32_t unit); // 0x55910, EAX=ev[2] ECX=ev[3], stack ev[0]
    uint32_t  (*findItem)(uint32_t eax, uint32_t ebx, uint32_t tag);   // 0xE660, EAX=ev[3] EBX=ev[0], stack tag
    void      (*sub566A0)(uint32_t eax, uint32_t ecx, uint32_t esi);   // 0x566A0, EAX=tag ECX=ev[0] ESI=ev[3]
    void      (*sub53880)(uintptr_t eaxSelfPlus1c, uint32_t esi);      // 0x53880
    void      (*sub53920)(uint32_t eax, uint32_t a, uint32_t b);       // 0x53920, EAX=ev[0], stack (a,b)
    void      (*callSelf)(uintptr_t self, uint32_t cb);                // call cb with ECX=self
    void      (*fatal)(uint32_t line);                                 // 0x2A3A_/_exit(-1) path (line 0x1ca/0x5b8)
};

// 1.13c D2Game+0xE8FE0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E8FE0(uintptr_t self, uintptr_t ev, const D2GameE8FE0Env* env)
{
    uintptr_t d = rd32(self, 0x18) ? env->p(rd32(self, 0x18)) : 0;
    uint32_t  unitH = rd32(ev, 0), ev2 = rd32(ev, 8), ev3 = rd32(ev, 0xc);
    uint16_t  w14 = *(uint16_t*)(ev + 0x14), msg = *(uint16_t*)(ev + 0x18);
    uintptr_t unit = unitH ? env->p(unitH) : 0;
    uintptr_t other = ev3 ? env->p(ev3) : 0;
    uintptr_t pd = env->getPlayerData(other);
    uint32_t qp = rd32(pd, 0x10 + rd8(unit, 0x6d) * 4);
    if (env->questBit(qp, 0x11, 0) == 1 || w14 != 0xfe) return;

    if (msg == 0x225) {
        env->setState(self, 2);
        wr8(self, 0x14, 0); wr8(self, 0xb, 1);
        env->fire(0xE6550, env->p(rd32(self, 4)), 0);   // EDI=self[4]; retail null-checks the literal address (fatal 0x5b8), never null
        env->fire(0xE5370, env->p(rd32(self, 4)), 0);
        env->sub55910(ev2, ev3, unitH);
        return;
    }
    if (msg != 0x234) return;
    env->sub55910(ev2, ev3, unitH);
    if (env->questBit(qp, 0x11, 0) != 0) return;
    if (env->findItem(ev3, unitH, 0x20626262)) {
        env->sub566A0(0x20626262, unitH, ev3);
        wr8(d, 0xc, 1);
        wr32(d, 0x10, other ? rd32(other, 0xc) : 0xffffffffu);
        wr32(self, 0xa8, env->addr(0xE51C0));
    }
    if (rd8(d, 0) == 1) {
        env->fire(0xE64C0, unit, ev2);
        wr8(d, 0, 0);
        env->fire(0xE6470, unit, ev2);
        env->fire(0xE5230, unit, ev2);
    }
    wr8(self, 0x14, 0); wr8(self, 0xb, 0xd);
    env->sub53880(self + 0x1c, other ? rd32(other, 0xc) : 0xffffffffu);
    if (env->questBit(qp, 0x11, 0xd) && rd8(self, 9)) {
        env->sub53920(unitH, 0x11, 0xd);
        if (rd8(self, 0xc) != 5) {
            env->setState(self, 5);
            uint32_t cb = rd32(self, 0xf0);
            if (!cb) env->fatal(0x1ca);
            env->callSelf(self, cb);
        }
    }
}

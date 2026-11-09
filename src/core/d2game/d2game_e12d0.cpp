// 1.13c D2Game+0xE12D0: create a preset unit by type.
// Callees are seams taking retail's own registers and stack words (the decompile hides the extra stack words and the
// ESI/EBX register inputs; read from the disassembly). Replaces n67_CreatePresetUnit in d2game_n67.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameE12D0Env {
    // D2Game callee by RVA: registers eax ecx edx ebx esi and the stack words pushed (right to left, so stk[0] is the top), result in EAX
    uint32_t (*call)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, const uint32_t* stk, uint32_t nstk);
    uint32_t (*ord10826)(uint32_t arg);                // D2Common #10826, stdcall, one word
};

// ECX = p1 (carried through, pushed again as a hidden stack word), EDX = type, EAX = preset id; stack p3..p6 (ret 0x10).
// 1.13c D2Game+0xe12d0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2game_e12d0_port(uint32_t id, uint32_t p1, uint32_t type, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, const D2GameE12D0Env* env)
{
    uint32_t u;
    if (type == 1) {
        const uint32_t s[4] = { p3, p5, p6, p1 };
        u = env->call(0x47CF0, 0, p4, 0, 0, 0, s, 4);
    } else {
        if (type == 2) {
            if ((int32_t)id > 0x23d) {
                const uint32_t s[5] = { p3, p4, p5, p6, p1 };
                u = env->call(0x2E830, 0, 0, 0, 0, id, s, 5);
                goto done;
            }
            if (id == 0x23d) return 0;
        }
        {
            const uint32_t s[7] = { p5, p6, p3, p4, 1, p1, 0 };
            u = env->call(0xE1D90, 0, type, id, 0, 0, s, 7);
        }
        if (type == 4) {
            if (!u) return 0;
            uint32_t blk[33] = {0};                    // rep stos 0x21 dwords
            blk[12] = 2;
            blk[3] = env->ord10826(p4);
            const uint32_t s[3] = { p3, (uint32_t)(uintptr_t)blk, 1 };
            if (env->call(0x11070, 0, 0, 0, (uint32_t)(uintptr_t)&u, 0, s, 3) == 0) {   // ECX = 0 left by rep stos, EBX = &u
                env->call(0xE20A0, 0, p3, u, 0, 0, nullptr, 0);
                return 0;
            }
        }
    }
done:
    if (u) { uint32_t f; std::memcpy(&f, (uint8_t*)(uintptr_t)u + 0xC4, 4); f |= 0x3000000; std::memcpy((uint8_t*)(uintptr_t)u + 0xC4, &f, 4); }
    return u;
}

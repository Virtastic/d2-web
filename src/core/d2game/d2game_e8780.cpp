// D2Game+0xE8780 -- ACT3Q3 fetish-boss object spawn: create object 0x197 in a room and link it to its owner record.
// esi = quest record, ebx = room, stack game (ret 4). Writes rec+4 = 1 for the duration of the call (a reentrancy flag).
//   u = 0x24950(eax 1, ecx 0x197, edx 0; game, room, 0, 0, -1)
//   when made: 0x23CA0(game, u, 1); 0x248A0(eax 1, ecx 0, edx game, edi u; room, 3, 6); rec+3 = 0; rec+2 = 1; rec+0x24 = u+0xC.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameE8780Env {
    uint32_t (*sub24950)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    void     (*sub23CA0)(uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*sub248A0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t edi, uint32_t s1, uint32_t s2, uint32_t s3);
};

// 1.13c D2Game+0xE8780
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_e8780_probe(uint32_t rec, uint32_t room, uint32_t game, const D2GameE8780Env* env)
{
    uint8_t* r = (uint8_t*)(uintptr_t)rec;
    r[4] = 1;
    const uint32_t u = env->sub24950(1, 0x197, 0, game, room, 0, 0, 0xffffffffu);
    if (u != 0) {
        env->sub23CA0(game, u, 1);
        env->sub248A0(1, 0, game, u, room, 3, 6);
        r[3] = 0;
        r[2] = 1;
        uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(u + 0xc), 4);
        std::memcpy(r + 0x24, &v, 4);
    }
    r[4] = 0;
}

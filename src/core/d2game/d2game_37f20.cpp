// D2Game last-attacker hand-off in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>
#include <unistd.h>

struct D2Game37F20Env {
    uint32_t (*lastAttacker)(uint32_t defender);   // D2Common thunk 0xA97C, stdcall 1 arg; the whole EAX is forwarded
    // 0xD60D0: ECX = attacker, EAX = defender id; stack (arg, flag, 1, &arg), ret 0x10
    uint32_t (*hit)(uint32_t attacker, uint32_t defenderId, uint32_t arg, uint32_t flag, uint32_t one, uint32_t* pArg);
};

// 1.13c D2Game+0x37F20
// EAX = defender, EDI = attacker; one stack arg (ret 4). A null defender exits through Fog with 0xC6, a null
// attacker with 0xC7. The defender id (+0xC) is read, the thunk 0xA97C runs on the defender, and 0xD60D0
// gets the attacker, the id, the stack arg, the thunk's EAX, 1 and the address of the stack arg slot
// (pushed before the thunk call). Result: 1 when 0xD60D0 returned non-zero.
uint32_t UNIT_HitFromAttacker_37F20(uint32_t defender, uint32_t attacker, uint32_t arg, const D2Game37F20Env& env)
{
    if (defender == 0 || attacker == 0) _exit(-1);   // Fog fatal 0xC6 / 0xC7
    uint32_t id; std::memcpy(&id, (const void*)(uintptr_t)(defender + 0xC), 4);
    const uint32_t flag = env.lastAttacker(defender);
    return env.hit(attacker, id, arg, flag, 1, &arg) != 0 ? 1u : 0u;
}

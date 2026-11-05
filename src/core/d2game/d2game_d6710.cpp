// 1.13c D2Game+0xD6710 -- set flag 0x10 on the first unit and, when it has no owner word and the check passes, finish the pickup; injectable callees.
// ECX = param1 (copied to EDI), EAX = unit (copied to ESI), stack word 0 = arg (ret 4). Callees (all stack words, balanced by the callee):
//   0xA65E (3 words: param1, 0x10, 1), 0xA640 (2 words: param1, unit; returns the gate), 0xD47C0 (ESI = unit, EDI = param1, 1 word 0),
//   0xD51A0 (3 words: arg, unit, 0), 0xA9A6 (2 words: unit, 1).
#include <cstdint>
struct GD6710Env {
    void (*a65e)(uint32_t w0, uint32_t w1, uint32_t w2);
    uint32_t (*a640)(uint32_t w0, uint32_t w1);
    void (*d47c0)(uint32_t esi, uint32_t edi, uint32_t w0);
    void (*d51a0)(uint32_t w0, uint32_t w1, uint32_t w2);
    void (*a9a6)(uint32_t w0, uint32_t w1);
};
// 1.13c D2Game+0xD6710
void g_d6710(uint32_t param1, uint32_t unit, uint32_t arg, const GD6710Env& env)
{
    env.a65e(param1, 0x10, 1);
    if (param1 != 0 && *(const uint32_t*)(uintptr_t)(param1 + 0x10) != 0) return;
    if (env.a640(param1, unit) == 0) return;
    env.d47c0(unit, param1, 0);
    env.d51a0(arg, unit, 0);
    env.a9a6(unit, 1);
}

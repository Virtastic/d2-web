// Decompile-derived, 1.13c-only. Seams: the AI-command lookup (D2Game+0x20E00) and the AI-command creator (D2Game+0x21290).
#include <cstdint>

struct D2Game217d0Env {
    uint32_t (*find)(uint32_t unit, uint32_t key, uint32_t flag);                 // 0x20E00: EAX = unit, ESI = key, stack flag (ret 4)
    void (*create)(uint32_t game, uint32_t unit, const uint32_t block[7]);        // 0x21290: EAX = game, ECX = unit, stack &block (ret 4)
};

// 1.13c D2Game+0x217D0
// ESI = command key (EAX on entry), EDI = unit, EBX = flag, stack arg = pGame (ret 4). Looks the command up; when it is missing creates it from
// a block whose word 2 is the key and words 3..6 are 0 (words 0 and 1 are never written), then looks it up again and returns that result.
extern "C" uint32_t d2game_get_or_create_ai_command_217d0(uint32_t key, uint32_t unit, uint32_t flag, uint32_t game, const D2Game217d0Env* env)
{
    if (!env->find(unit, key, flag)) {
        uint32_t block[7] = { 0, 0, key, 0, 0, 0, 0 };
        env->create(game, unit, block);
    }
    return env->find(unit, key, flag);
}

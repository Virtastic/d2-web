// Affix-group membership (1.13c D2Game+0x131E0, split out of d2game_batch_131e0.cpp). EDI is the item, EBX
// the affix group to reject. Walk the item's three prefix slots; each prefix id (16-bit) resolves to a
// magic-affix record whose +0x5C (dwGroup) answers 1 on a match, a null record ends the prefix pass. The
// three suffix slots then run the same probe, with a null record answering 0. Everything else answers 0.
#include "d2game_131e0_env.h"

// 1.13c D2Game+0x131E0
extern "C" int32_t d2g_131e0(void* item, uint32_t group, const D2GameBatch131E0Env* env)
{
    for (int32_t i = 0; i < 3; ++i) {
        const void* rec = env->ordinal10248(env->ordinal10257(item, i));
        if (rec == nullptr) break;
        if (rd32(rec, 0x5C) == group) return 1;
    }
    for (int32_t i = 0; i < 3; ++i) {
        const void* rec = env->ordinal10248(env->ordinal11028(item, i));
        if (rec == nullptr) return 0;
        if (rd32(rec, 0x5C) == group) return 1;
    }
    return 0;
}

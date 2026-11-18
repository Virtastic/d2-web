// Env-seam adapter for the stand-alone B6840/B6A60 ports (d2game_batch_b4520.cpp) and the b6720 FN probe: runs the game body
// SKILLS_EndAnimTail_B6720 (D2Game/SKILLS/SkillEndAnimTailOutlined.cpp) with its callees (D2Game+0x1320, +0x8E6F0, +0x8EA80)
// routed to the env's `sub` seam in retail argument order.
#include <cstdint>
#include "../include/D2Game/SKILLS/SkillMonst.h"
#include "../include/D2Game/GAME/Event.h"
#include "../include/D2Game/MONSTER/MonsterRegion.h"
#include "../compat/test_seam.h"

struct D2GameBatchB4520Env {
    int32_t (*ord)(uint32_t n, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    int32_t (*sub)(uint32_t rva, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    const uint8_t* dt;
    const uint8_t* tbl1B510;
    const uint8_t* tbl1B518;
};

static const D2GameBatchB4520Env* g_b6720Env;
static inline uint32_t w6720(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" uint32_t d2_fn_d2game_B6720(void* game, void* unit, const D2GameBatchB4520Env* env)
{
    g_b6720Env = env;
    D2Seam<&MONSTERREGION_GetMonStats2TxtRecord>::hook = [](int32_t nClass) -> D2MonStats2Txt* {
        return (D2MonStats2Txt*)(uintptr_t)(uint32_t)g_b6720Env->sub(0x1320, (uint32_t)nClass, 0, 0, 0, 0, 0, 0);
    };
    D2Seam<&sub_6FC34890>::hook = [](D2GameStrc* pGame, D2EventTimerStrc* pTimer) {
        g_b6720Env->sub(0x8E6F0, w6720(pTimer), w6720(pGame), 0, 0, 0, 0, 0);
    };
    D2Seam<&D2GAME_InitTimer_6FC351D0>::hook = [](D2GameStrc* pGame, D2UnitStrc* pUnit, D2C_EventTypes nType, int32_t nFrame, EventTimerCallback pCb, int32_t nId, int32_t nParam) {
        g_b6720Env->sub(0x8EA80, w6720(pGame), w6720(pUnit), (uint32_t)nType, (uint32_t)nFrame, w6720((const void*)pCb), (uint32_t)nId, (uint32_t)nParam);
    };
    const uint32_t res = (uint32_t)SKILLS_EndAnimTail_B6720((D2GameStrc*)game, (D2UnitStrc*)unit);
    D2Seam<&MONSTERREGION_GetMonStats2TxtRecord>::hook = nullptr;
    D2Seam<&sub_6FC34890>::hook = nullptr;
    D2Seam<&D2GAME_InitTimer_6FC351D0>::hook = nullptr;
    return res;
}

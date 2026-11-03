// 1.13c D2Game+0x58DA0 OBJECTS_OperateFunction69_InvisibleAncient.
// pOp = { game, object, player } (ECX in retail). The player's quest bit buffer for the game's difficulty is read twice through
// D2Common #10920 (player data, buffer pointer at +0x10 + 4 * difficulty) and #10174 (quest state): when quest 0x27 has state 0
// (reward granted) set and state 4 (area entered) clear, the scroll message 0x4EC9 (20169) goes to the player (0x34B80: EAX = player,
// ECX = object, one stack word). Always returns 0. The callees ride in Env so a probe records them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2G58DA0Env {
    uint32_t (*playerData)(uint32_t player);                                  // D2Common #10920 (0xA4C0)
    uint32_t (*questState)(uint32_t flags, uint32_t quest, uint32_t state);   // D2Common #10174 (0xA568)
    void     (*scroll)(uint32_t player, uint32_t object, uint32_t msg);       // 0x34B80
};

static inline uint32_t rd32_58da0(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_58DA0(uint32_t pOp, const D2G58DA0Env* e)
{
    const uint32_t game = rd32_58da0(pOp), object = rd32_58da0(pOp + 4), player = rd32_58da0(pOp + 8);
    uint32_t flags = rd32_58da0(e->playerData(player) + 0x10 + (*(const uint8_t*)(uintptr_t)(game + 0x6D)) * 4);
    if (e->questState(flags, 0x27, 0) != 0) {
        const uint32_t game2 = rd32_58da0(pOp), player2 = rd32_58da0(pOp + 8);
        flags = rd32_58da0(e->playerData(player2) + 0x10 + (*(const uint8_t*)(uintptr_t)(game2 + 0x6D)) * 4);
        if (e->questState(flags, 0x27, 4) == 0) e->scroll(rd32_58da0(pOp + 8), object, 0x4EC9);
    }
    return 0;
}

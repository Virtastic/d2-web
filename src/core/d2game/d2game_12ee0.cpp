// Item durability / quantity tick (1.13c D2Game+0x12EE0, called from 0xDEBA0). ESI is the item, EDI its owner,
// the one stack word the game. An item carrying either replenish stat (0x32 or 0x2D) that passes 0xA970 rolls
// the owner's 64-bit seed at +0x20 (lo * 0x6AC690C5 + hi); the new low dword modulo 100 under the rate fires.
// The rate is 10 when the 0x32 stat is set, or when 0xA9AC accepts the item and the game's +0x70 is set (a clear
// +0x70 there ends the tick); 4 otherwise. Durability (0x48) above 1 drops by one, clamped to 0xAA00, after the
// 0xEA70 refresh. At 1, an item without the 0x2D stat that 0xA8A4 refuses goes to 0xD5740; one 0xB37E rejects
// has its durability zeroed; otherwise quantity (0x46) above 1 is spent to restock durability, and the last one
// clears both and runs 0x79100; 0xAA06 then decides a second 0xEA70 refresh. Every stat write is reported to
// the owner's client through 0x8B010. Retail is 32-bit: the callees are seams in retail argument order, with
// the register-passed ones named.
#include <cstdint>
#include <cstring>

struct D2G12EE0Env {
    int32_t (*hasStat)(void* item, int32_t stat);                                        // 0xA430
    int32_t (*itemCheck)(void* item);                                                    // 0xA970
    int32_t (*itemCheck2)(void* item);                                                   // 0xA9AC
    int32_t (*getStat)(void* item, int32_t stat, int32_t layer);                         // 0xA42A
    int32_t (*maxDurability)(void* item);                                                // 0xAA00
    void (*refresh)(void* itemEbx, void* game);                                          // 0xEA70
    void (*setStat)(void* item, int32_t stat, int32_t value, int32_t layer);             // 0xA472
    void* (*ownerData)(void* owner);                                                     // 0xA4C0
    void (*report)(void* itemEax, uint32_t ebx, uint32_t client, int32_t one, int32_t stat, int32_t value); // 0x8B010
    int32_t (*itemFlag)(void* item, int32_t flag, int32_t line, uint32_t file);          // 0xA8A4
    void (*broken)(void* itemEax, void* ownerEcx, void* gameEdx);                        // 0xD5740
    uint16_t (*stackable)(void* item);                                                   // 0xB37E
    void (*spent)(void* ownerEax, void* game);                                           // 0x79100
    int32_t (*requantity)(void* item);                                                   // 0xAA06
    uint32_t file;                                                                       // D2Game+0xFD078, the source-file string 0xA8A4 takes
};

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// The stat write and its report: the client word is the owner data's +0x9C for a player owner (type 0), else 0.
void put(void* item, void* owner, int32_t stat, int32_t value, int32_t reported, const D2G12EE0Env* env) {
    env->setStat(item, stat, value, 0);
    const uint32_t client = (owner && rd32(owner, 0) == 0) ? rd32(env->ownerData(owner), 0x9C) : 0;
    env->report(item, 0, client, 1, stat, reported);
}
}

// 1.13c D2Game+0x12EE0
extern "C" void d2g_12ee0(void* item, void* owner, void* game, const D2G12EE0Env* env) {
    const int32_t s50 = env->hasStat(item, 0x32);
    const int32_t s45 = env->hasStat(item, 0x2D);
    if (!s50 && !s45) return;
    if (!env->itemCheck(item)) return;
    int32_t rate = 4;
    if (s50) rate = 10;
    else if (env->itemCheck2(item)) {
        if (!rd32(game, 0x70)) return;
        rate = 10;
    }
    const uint64_t x = (uint64_t)rd32(owner, 0x20) * 0x6AC690C5u + rd32(owner, 0x24);
    wr32(owner, 0x24, (uint32_t)(x >> 32));
    wr32(owner, 0x20, (uint32_t)x);
    if ((int32_t)((uint32_t)x % 100) >= rate) return;
    int32_t dur = env->getStat(item, 0x48, 0) - 1;
    if (dur > 0) {
        const int32_t m = env->maxDurability(item);
        if (dur >= m) dur = m;
        env->refresh(item, game);
        put(item, owner, 0x48, dur, dur, env);
        return;
    }
    if (!s45 && !env->itemFlag(item, 0x100, 0xC5F, env->file)) { env->broken(item, owner, game); return; }
    if (env->stackable(item) < 1) { put(item, owner, 0x48, 0, 0, env); return; }
    const int32_t q = env->getStat(item, 0x46, 0);
    if (q > 1) {
        put(item, owner, 0x46, q - 1, q - 1, env);
        const int32_t m = env->maxDurability(item);
        put(item, owner, 0x48, m, m, env);
    } else {
        put(item, owner, 0x48, 0, 0, env);
        put(item, owner, 0x46, 0, q, env);
        env->spent(owner, game);
    }
    if (env->requantity(item) > 0) env->refresh(item, game);
}

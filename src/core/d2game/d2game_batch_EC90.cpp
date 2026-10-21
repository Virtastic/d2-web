// src/core/d2game/d2game_batch_EC90.cpp -- 1.13c D2Game.dll sub_EC90 (0xEC90): the socket roll at item
// creation (1.10f sub_6FC4D6B0, D2MOO ITEMS). Item arrives in a register, the game in EDI, the drop
// record in EBX (retail disassembly authoritative). Called from 0x10600 and 0x10A20 for quality 1..3.
// Difficulty (game+0x6D) caps the socket count at 3/4/6; a roll of rnd%100 < 33 (or drop flag 0x800,
// or ALWAYSSOCKETS 0x10) sockets the item unless NOSOCKETS (0x8). Callees are host seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

struct D2GameBatchEC90Env {
    int32_t  (*getItemQuality)(void*);                   // #10106
    int32_t  (*checkSocketable)(void*);                  // #10757
    int32_t  (*checkStackable)(void*);                   // #10048
    uint8_t  (*getMaxSockets)(void*);                    // #11151
    uint16_t (*getItemFormat)(void*);                    // #10718
    int32_t  (*checkItemTypeId)(void*, int32_t);         // #10744
    void*    (*getItemSeed)(void*);                      // #10411
    uint32_t (*rollRandom)(void*);                       // D2Game 0x1180
    void     (*setItemFlag)(void*, uint32_t, int32_t);   // #10977
    uint32_t (*getStartSeed)(void*);                     // #10193
    void     (*addSockets)(void*, uint32_t);             // #10992
};
static const D2GameBatchEC90Env g_batchEC90Env{};

// 1.13c D2Game+0xEC90
static void RollSockets(void* pItem, void* pGame, void* pDrop, const D2GameBatchEC90Env* env)
{
    if (env->getItemQuality(pItem) < 2 || !env->checkSocketable(pItem) || env->checkStackable(pItem)) return;

    uint32_t nMax = env->getMaxSockets(pItem);
    if (nMax == 0) return;
    const uint8_t diff = rd8(pGame, 0x6D);
    const uint32_t cap = diff == 0 ? 3 : diff == 1 ? 4 : diff == 2 ? 6 : nMax;
    if (nMax > cap) nMax = cap;

    if (env->getItemFormat(pItem) >= 100 || !env->checkItemTypeId(pItem, 3 /* ITEMTYPE_ARMOR */)) {
        uint32_t nChance = env->rollRandom(env->getItemSeed(pItem)) % 100;
        const uint32_t f2 = rd32(pDrop, 0x80);
        if (f2 & 8) return;                                  // NOSOCKETS
        if (f2 & 0x10) nChance = 0;                          // ALWAYSSOCKETS
        if ((rd32(pDrop, 0x44) & 0x800) == 0 && nChance >= 33) return;
        env->setItemFlag(pItem, 0x800, 1);                   // IFLAG_SOCKETED
        if (env->getItemFormat(pItem) != 0) {
            env->addSockets(pItem, env->getStartSeed(pItem) % nMax + 1);
        } else {
            if (nMax > 3) nMax = 3;
            if (env->checkItemTypeId(pItem, 0x25 /* ITEMTYPE_HELM */) && nMax > 2) nMax = 2;
            env->addSockets(pItem, nMax);
        }
    }
}

// r[0]=item r[1]=game(EDI) r[2]=drop(EBX)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ec90(uint32_t* r)
{
    RollSockets((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], (void*)(uintptr_t)r[2], &g_batchEC90Env);
    r[4] = 0;
}

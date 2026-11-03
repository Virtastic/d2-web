// d2game+0x5e7f0 QuestNodeFlagNotify: probe. thiscall this = game blob, [esp+4] = unit. The port (d2game_5E7F0) reads the game
// through a seam table; only rd8/rd32/ptr are needed on the no-notify paths (the table's first thirteen members, same order as
// D2GameBatch531F0Env in d2game_batch_531f0.cpp), the rest stays null.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct E531F0Prefix {
    void* m0to8[9];
    uint8_t (*rd8)(void*, uint32_t);
    uint16_t (*rd16)(void*, uint32_t);
    uint32_t (*rd32)(void*, uint32_t);
    void* (*ptr)(void*, uint32_t);
    void* rest[96];
};
extern "C" void d2game_5E7F0_env(const void* env, void* game, void* p);
static uint8_t rb(void* o, uint32_t off) { return *((uint8_t*)o + off); }
static uint16_t rw(void* o, uint32_t off) { uint16_t v; memcpy(&v, (uint8_t*)o + off, 2); return v; }
static uint32_t rd(void* o, uint32_t off) { uint32_t v; memcpy(&v, (uint8_t*)o + off, 4); return v; }
static void* rp(void* o, uint32_t off) { return (void*)(uintptr_t)rd(o, off); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5e7f0(uint32_t* r) {
    static E531F0Prefix E;
    E.rd8 = rb; E.rd16 = rw; E.rd32 = rd; E.ptr = rp;
    d2game_5E7F0_env(&E, (void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1]);
    r[4] = 0;
}

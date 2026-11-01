// 1.13c D2Game+0x24030 -- amplify-damage curse application (ECX = game, EDX = target, stack: source, level; ret 8).
// Gated by E0A60(eax=source, ecx=target, game); skill 0x42's record (dataTables[0xB98] + 0x9378) names a stat (+0x54) and a
// state (+0x82), both range-checked; duration (+0x60) and value (+0x68) are AC9A formula results; CC00B0 builds the curse
// and returns its stat list, which then receives up to five extra stats (+0x56 ids, +0x6C formulas) through A6E2.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game24030Env {
    uint32_t (*subE0A60)(uint32_t source, uint32_t target, uint32_t game);
    int32_t (*evalAC9A)(uint32_t target, uint32_t formula, uint32_t skill, uint32_t level);
    uint32_t (*subCC00B0)(const uint32_t* block);
    void (*setStatA6E2)(uint32_t list, int32_t stat, int32_t value, uint32_t layer);
    uint32_t dataTables;
};
template<class T> static T rd24030(uint32_t a, uint32_t o) { T v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, sizeof v); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_24030(uint32_t game, uint32_t target, uint32_t source, int32_t level, const D2Game24030Env* e)
{
    if (!e->subE0A60(source, target, game)) return;
    const uint32_t dt = e->dataTables;
    if (rd24030<int32_t>(dt, 0xBA0) <= 0x42) return;
    const uint32_t rec = rd24030<uint32_t>(dt, 0xB98) + 0x9378u;
    if (!rec) return;
    const int32_t nStat = rd24030<int32_t>(dt, 0xBD4);
    const int32_t stat = rd24030<int16_t>(rec, 0x54);
    if (stat < -1 || stat >= nStat) return;
    const int32_t state = rd24030<int16_t>(rec, 0x82);
    if (state < 0 || state >= rd24030<int32_t>(dt, 0xC4)) return;
    const int32_t duration = e->evalAC9A(target, rd24030<uint32_t>(rec, 0x60), 0x42, level);
    const int32_t value = e->evalAC9A(target, rd24030<uint32_t>(rec, 0x68), 0x42, level);
    const uint32_t blk[9] = { target, source, 0x42, (uint32_t)level, (uint32_t)duration, (uint32_t)stat, (uint32_t)value, (uint32_t)state, 0 };
    const uint32_t list = e->subCC00B0(blk);
    if (!list) return;
    for (uint32_t i = 0; i < 5; ++i) {
        const int32_t s = rd24030<int16_t>(rec, 0x56 + i * 2);
        if (s >= 0 && s < nStat) {
            const int32_t x = e->evalAC9A(target, rd24030<uint32_t>(rec, 0x6C + i * 4), 0x42, level);
            if (x) e->setStatA6E2(list, s, x, 0);
        }
    }
}

// D2Game+0x3FED0 -- monster mode "heal target" do-function, with retail's real convention: ECX = game, EDX = unit, plain ret.
// Order (disassembled): monstats row (unit+4, dt+0xB64/+0xB6C, stride 0x1A4) and skill row (0xB26A(unit) id, dt+0xB98/+0xBA0, stride 0x23C), either
// missing returns 2. Flag (+0xC8 bit 10) set: target = 0xE03A0(EAX = unit+0x94, EDX = unit+0x98, ECX = game); a target that is not dead (0xDFA90, EAX = target):
// v = 0xB0FC(unit); when 0xB28E(unit) == row+0x38 and the target's stat 6 >> 8 is below 0xA3A0(target) >> 8: pct = 0xAC9A(target, skill+0x138, id, v),
// hp = min(cur + 0x14D0(EAX pct, ECX 100, EDX max), max) stored << 8 by 0xA472, then 0xA83E(target, row short +0x36, 0) when it is in (0, dt+0xBC0).
// Every path past the row checks ends in the tail 0x3F320 (stack game, EBX = unit), whose result is returned.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game3FED0Env {
    const uint8_t* dataTables;                                       // *sgptDataTables
    uint32_t (*getTarget)(uint32_t a94, uint32_t a98, uint32_t game);  // 0xE03A0
    uint32_t (*targetDead)(uint32_t target);                         // 0xDFA90 (EAX)
    uint32_t (*skillId)(uint32_t unit);                              // 0xB26A
    uint32_t (*level)(uint32_t unit);                                // 0xB0FC
    uint32_t (*period)(uint32_t unit);                               // 0xB28E
    uint32_t (*stat)(uint32_t unit, uint32_t id, uint32_t layer);    // 0xA42A
    uint32_t (*maxHp)(uint32_t unit);                                // 0xA3A0
    uint32_t (*apply)(uint32_t target, uint32_t a, uint32_t id, uint32_t v);  // 0xAC9A
    uint32_t (*scale)(uint32_t a, uint32_t b, uint32_t c);           // 0x14D0 (EAX, ECX, EDX)
    void     (*setStat)(uint32_t unit, uint32_t id, uint32_t val, uint32_t layer);  // 0xA472
    void     (*addState)(uint32_t unit, uint32_t state, uint32_t z);  // 0xA83E
    uint32_t (*tail)(uint32_t game, uint32_t unit);                  // 0x3F320
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t dt32(const D2Game3FED0Env* e, uint32_t o) { return rd32((uint32_t)(uintptr_t)e->dataTables + o); }

// 1.13c D2Game+0x3FED0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_3FED0(uint32_t game, uint32_t unit, const D2Game3FED0Env* e)
{
    if (!unit) return 2;
    const int32_t cls = (int32_t)rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)dt32(e, 0xB6C)) return 2;
    const uint32_t rec = dt32(e, 0xB64) + (uint32_t)cls * 0x1A4;
    if (!rec) return 2;
    const int32_t sid = (int32_t)e->skillId(unit);
    if (sid < 0 || sid >= (int32_t)dt32(e, 0xBA0)) return 2;
    const uint32_t sk = dt32(e, 0xB98) + (uint32_t)sid * 0x23C;
    if (!sk) return 2;
    if ((rd32(unit + 0xC8) >> 10) & 1) {
        const uint32_t tgt = e->getTarget(rd32(unit + 0x94), rd32(unit + 0x98), game);
        if (tgt && !e->targetDead(tgt)) {
            const uint32_t v = e->level(unit);
            if (e->period(unit) == rd32(rec + 0x38)) {
                const int32_t cur = (int32_t)e->stat(tgt, 6, 0) >> 8;
                const int32_t max = (int32_t)e->maxHp(tgt) >> 8;
                if (cur < max) {
                    const uint32_t pct = e->apply(tgt, rd32(sk + 0x138), (uint32_t)sid, v);
                    int32_t n = cur + (int32_t)e->scale(pct, 100, (uint32_t)max);
                    if (n >= max) n = max;
                    e->setStat(tgt, 6, (uint32_t)n << 8, 0);
                    int16_t s; std::memcpy(&s, (const void*)(uintptr_t)(rec + 0x36), 2);
                    if (s > 0 && s < (int32_t)dt32(e, 0xBC0)) e->addState(tgt, (uint32_t)(int32_t)s, 0);
                }
            }
        }
    }
    return e->tail(game, unit);
}

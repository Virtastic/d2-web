// 1.13c D2Game+0xCDF30: the skill-use dispatch of the skill packets. Disassembled convention: EAX = skill, ECX = unit, three stack
// words (s0, s1, s2), ret 0xC. A null unit, or unit+0x10 (mode) 0 or 0x11, returns 2. Ordinal_10747(unit) non-zero swaps the skill
// for 0xCB1F0's (EBX = unit, stack = skill). The use mode is Ordinal_10787(unit, skill); a null Ordinal_10858 record (ECX = skill)
// returns 2. Modes 1, 2 and 4 with the record's +7 byte holding gdwBitMasks[4]'s low byte (0x10, D2Common data) re-pick the skill
// through Ordinal_10708 (ECX = unit, EDX = 0, stack -1) and re-read the mode. Ordinal_10952(unit, skill) zero returns 2; mode 0
// calls 0x78550 (ECX = s0, EDX = unit, stack = skill, that result, s1, s2, 0) and returns 0; mode 1 calls 0xCBED0 (EAX = unit).
#include <cstdint>
#include <cstring>
#include "d2game_cdf30.h"

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }

static constexpr uint8_t kBitMask4 = 0x10;   // the low byte of D2Common gdwBitMasks[4] (1 << 4)

uint32_t d2game_cdf30_port(uint32_t skill, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, const D2GameCDF30Env* e) {
    if (!unit) return 2;
    const uint32_t mode0 = rd32(unit, 0x10);
    if (mode0 == 0 || mode0 == 0x11) return 2;
    if (e->o10747(unit)) skill = e->f_cb1f0(unit, skill);
    int32_t mode = (int32_t)e->o10787(unit, skill);
    const uint32_t txt = e->o10858(skill);
    if (!txt) return 2;
    if (mode > 0 && (mode <= 2 || mode == 4) && (kBitMask4 & rd8(txt, 7))) {
        skill = e->o10708(unit, 0, 0xFFFFFFFFu);
        mode = (int32_t)e->o10787(unit, skill);
    }
    const uint32_t r = e->o10952(unit, skill);
    if (!r) return 2;
    if (mode == 0) { e->f_78550(s0, unit, skill, r, s1, s2, 0); return 0; }
    if (mode == 1) e->f_cbed0(unit);
    return 2;
}

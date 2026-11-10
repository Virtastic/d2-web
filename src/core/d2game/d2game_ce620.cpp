// 1.13c D2Game+0xCE620: the targeted skill-use dispatch (the unit-target twin of 0xCDF30). Disassembled convention: EAX = skill,
// ECX = unit, stack = game, target type, target id, flag; ret 0x10. Ordinal_10747(unit) non-zero swaps the skill for 0xCB1F0's
// (EBX = unit, stack = skill); the use mode is Ordinal_10787(unit, skill); a null Ordinal_10858 record (ECX = skill) returns 2.
// Modes 1, 2 and 4 with the record's +7 byte holding gdwBitMasks[4] (0x10) re-pick the skill (Ordinal_10708 ECX = unit, EDX = 0,
// stack -1) and re-read the mode; any non-zero mode returns 2, as does a zero Ordinal_10952(unit, skill). The record's +7 byte
// holding gdwBitMasks[3] (0x08), or a target type 0, 1 or 3, goes on; any other type returns 2. 0xE03A0 (EAX = type, ECX = game,
// EDX = id) finds the target; one that answers 0xA68E(target, 0x8F) is swapped for 0xE0A20's (EAX = target, stack game) when that
// is non-null, taking its type (+0) and id (+0xC). A non-zero flag with a target asks 0xAB7A(unit, skill): 1 needs
// 0xA754(unit, target, 0xAAC0(target)) non-zero and 4 always fails; a failure calls 0xCE120 (EAX = game, EBX = type, ECX = unit,
// ESI = id, stack = 0xA49C(skill, PlrMsg.cpp, 0x7A6)). Otherwise 0x78430 (EAX = game; stack skill, the 10952 result, type, id, 0).
// Both paths return 0.
#include <cstdint>
#include "d2game_ce620.h"

static inline uint8_t rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return *(const uint32_t*)(uintptr_t)(a + o); }

static constexpr uint8_t kBitMask3 = 0x08, kBitMask4 = 0x10;   // the low bytes of D2Common gdwBitMasks[3] / [4]
static constexpr uint32_t kPlrMsgRva = 0xFB188;                // ..\Source\D2Game\PLAYER\PlrMsg.cpp (D2Game data; the record zeroes it)

uint32_t d2game_ce620_port(uint32_t skill, uint32_t unit, uint32_t game, uint32_t type, uint32_t id, uint32_t flag, const D2GameCE620Env* e) {
    if (e->o10747(unit)) skill = e->f_cb1f0(unit, skill);
    int32_t mode = (int32_t)e->o10787(unit, skill);
    const uint32_t txt = e->o10858(skill);
    if (!txt) return 2;
    if (mode > 0 && (mode <= 2 || mode == 4) && (kBitMask4 & rd8(txt, 7))) {
        skill = e->o10708(unit, 0, 0xFFFFFFFFu);
        mode = (int32_t)e->o10787(unit, skill);
    }
    if (mode != 0) return 2;
    const uint32_t r = e->o10952(unit, skill);
    if (!r) return 2;
    const int32_t t = (int32_t)type;
    if (!(kBitMask3 & rd8(txt, 7)) && !(t == 0 || t == 1 || t == 3)) return 2;
    uint32_t tgt = e->f_e03a0(type, game, id);
    if (tgt && e->f_a68e(tgt, 0x8f)) {
        const uint32_t t2 = e->f_e0a20(tgt, game);
        if (t2) { tgt = t2; type = rd32(t2, 0); id = rd32(t2, 0xc); }
    }
    if (flag && tgt) {
        const uint32_t k = e->f_ab7a(unit, skill);
        bool fail = false;
        if (k == 1) fail = !e->f_a754(unit, tgt, e->f_aac0(tgt));
        else if (k == 4) fail = true;
        if (fail) {
            const uint32_t a = e->f_a49c(skill, kPlrMsgRva, 0x7a6);
            e->f_ce120(game, type, unit, id, a);
            return 0;
        }
    }
    e->f_78430(game, skill, r, type, id, 0);
    return 0;
}

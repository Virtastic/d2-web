// 1.13c D2Game+0x241B0 -- monster skill grant and AI flag (ECX = unit, stack: enable flag; ret 4).
// Type-1 unit with a nonzero flag: when the skills table has more than 0xB8 rows, AddSkill (#A9FA: unit, 0xB8, 1, 1, source
// file, line 0x112A) then the update call (#AEB6); the free (0x96FA0(unit[0x80], unit)) fires only for a type-0 unit, which
// the entry test already excluded. Then A3F4(ecx=unit, edx=0xB8, 4) feeds AD9C, and the AI record (unit[0x14] -> +0x28) byte
// +8 |= 0x20. Retail faults on a missing AI record (a write at address 8); the port stops there.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game241B0Env {
    void (*addSkillA9FA)(uint32_t unit, uint32_t skill, uint32_t a, uint32_t b, uint32_t file, uint32_t line);
    void (*updateAEB6)(uint32_t unit);
    void (*free96FA0)(uint32_t gameId, uint32_t unit);
    uint32_t (*getA3F4)(uint32_t unit, uint32_t skill, uint32_t four);
    void (*rollAD9C)(uint32_t value);
    uint32_t dataTables;
    uint32_t fileToken;
};
template<class T> static T rd241b0(uint32_t a, uint32_t o) { T v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, sizeof v); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_241B0(uint32_t unit, uint32_t flag, const D2Game241B0Env* e)
{
    if (!unit || rd241b0<uint32_t>(unit, 0) != 1 || !flag) return;
    if (rd241b0<int32_t>(e->dataTables, 0xBA0) > 0xB8) {
        e->addSkillA9FA(unit, 0xB8, 1, 1, e->fileToken, 0x112A);
        e->updateAEB6(unit);
        if (rd241b0<uint32_t>(unit, 0) == 0) e->free96FA0(rd241b0<uint32_t>(unit, 0x80), unit);
    }
    e->rollAD9C(e->getA3F4(unit, 0xB8, 4));
    const uint32_t pd = rd241b0<uint32_t>(unit, 0x14);
    if (rd241b0<uint32_t>(unit, 0) == 1 && pd) {
        const uint32_t ai = rd241b0<uint32_t>(pd, 0x28);
        if (ai) *((uint8_t*)(uintptr_t)ai + 8) |= 0x20;
    }
}

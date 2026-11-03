// 1.13c D2Game+0x778A0 -- the player's two selected skills as {right id, left id, right owner, left owner}.
// ESI = 16-byte output record, EBX = unit (no stack args). Left skill (0xA3AC): present, owner (0xA4BA) goes to +0xC and the
// PlrModes.cpp id (0xA49C, line 0x56D) to +4; absent, -1 and 0. Right skill (0xA3B2) the same into +8 / +0 (line 0x579).
// No 1.10f counterpart; the body is the 1.13c decompile, call order as the disassembly.
#include <cstdint>
#include <cstring>

struct D2Game778A0Env {
    uint32_t (*leftSkill)(uint32_t unit);                                  // 0xA3AC (stdcall 1)
    uint32_t (*rightSkill)(uint32_t unit);                                 // 0xA3B2 (stdcall 1)
    uint32_t (*owner)(uint32_t skill);                                     // 0xA4BA (stdcall 1)
    uint32_t (*skillId)(uint32_t skill, const char* file, uint32_t line);  // 0xA49C (stdcall 3)
};

static inline void w32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// 1.13c D2Game+0x778A0
void D2Game_778A0(uint8_t* out, uint32_t unit, const D2Game778A0Env& e)
{
    static const char kFile[] = "..\\Source\\D2Game\\PLAYER\\PlrModes.cpp";
    uint32_t s = e.leftSkill(unit);
    if (s != 0) {
        w32(out, 0xC, e.owner(s));
        w32(out, 0x4, e.skillId(s, kFile, 0x56D));
    } else {
        w32(out, 0xC, 0xFFFFFFFFu);
        w32(out, 0x4, 0);
    }
    s = e.rightSkill(unit);
    if (s != 0) {
        w32(out, 0x8, e.owner(s));
        w32(out, 0x0, e.skillId(s, kFile, 0x579));
    } else {
        w32(out, 0x8, 0xFFFFFFFFu);
        w32(out, 0x0, 0);
    }
}

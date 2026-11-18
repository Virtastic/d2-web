// 1.13c D2Client+0x49130: the NPC Introduction line. ESI = the hero's class (+4 of the player pointer at 0x11BBFC, -1 without one);
// the NPC record comes from 0x46150 (+4 = NPC class). Malah (0x201) with a Barbarian picks line 15, Nihlathak (0x202) with a
// Necromancer 11, Qual-Kehk (0x203) with a Paladin 10; otherwise line 1 when the hero class equals the word at +0x1A of the
// record that the pointer at +5 of 0x119675's target points to, else line 0. The line goes to 0x48590 in EAX with one zero
// stack word (Ghidra drops the EAX argument); returns 1 on every path.
#include <cstdint>
#include "cl_env.h"

// 1.13c D2Client+0x49130
extern "C" uint32_t d2c_49130(const cl::Env& e) {
    const int32_t cls = e.self ? (int32_t)e.R(e.self + 4) : -1;
    uint32_t idx = 0xffffffffu;
    if (const uint32_t npc = e.H(0x46150)) {
        const uint32_t n = e.R(npc + 4);
        if (n == 0x201) { if (cls == 4) idx = 0xF; }
        else if (n == 0x202) { if (cls == 2) idx = 0xB; }
        else if (n == 0x203) { if (cls == 3) idx = 0xA; }
    }
    if (idx == 0xffffffffu) idx = (cls == (int32_t)e.R(e.R(e.gl + 5) + 0x1a)) ? 1u : 0u;
    e.H(0x48590, 0, 0, 0, 0, 0, idx);
    return 1;
}

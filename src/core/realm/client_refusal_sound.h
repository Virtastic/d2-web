#pragma once
// 1.13c D2Client+0x18F90/0x18F40: a refused cast speaks the class's "not enough mana" line (Sounds event 0x15) for skill use
// state 1 and the "can't do that" line (0x13) for states 2-4. Only the client's own use-state refusal (refusal 4) speaks;
// every other refusal is silent. 0 = nothing to say.
namespace d2::realm {
inline int refusalSoundEvent(int refusal, int useState) {
    if (refusal != 4) return 0;
    return useState == 1 ? 0x15 : (useState >= 2 && useState <= 4) ? 0x13 : 0;
}
}

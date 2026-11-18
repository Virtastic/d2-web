// 1.13c D2Launch+0x121B0 (sub_121B0). ESI = key, stack a0 (enable), a1, a2 (string ids), ret 0xC. While the flag
// DAT_6fa64d94 is -1: a0 != 0 sets both text controls (DAT_6fa65ac0/ac4) from the D2Lang string a1/a2 and saves ESI in
// DAT_6fa64d90; a0 == 0 clears them when ESI is the saved key. Returns 1.
#include <cstdint>

extern "C" uint32_t d2l_121b0(uint32_t esi, uint32_t a0, uint32_t a1, uint32_t a2, const int32_t* flag, uint32_t* saved,
                              uint32_t ac0, uint32_t ac4, void (*imp996a)(uint32_t), uint32_t (*imp9b32)(uint32_t),
                              void (*imp99be)(uint32_t, uint32_t)) {
    if (*flag != -1) return 1;
    if (a0) {
        imp996a(ac0);
        imp99be(ac0, imp9b32(a1));
        imp996a(ac4);
        imp99be(ac4, imp9b32(a2));
        *saved = esi;
        return 1;
    }
    if (*saved == esi) {
        imp996a(ac0);
        imp996a(ac4);
    }
    return 1;
}

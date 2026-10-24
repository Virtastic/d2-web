// 1.13c Storm SEVT command senders 0x34410 and 0x344D0/0x344F0/0x34510/0x34530 (ordinals 512-515).
// Real extern "C" bodies; ESI (the group key) is an explicit parameter.
#include <cstdint>

extern "C" uint32_t* Storm_Sub15E0(uint32_t* list, uint32_t size, uint32_t flags, uint32_t mode);
extern "C" uint32_t Storm_Sub35CD0_Real(uint32_t, uint32_t, uint32_t, uint32_t);

// 0x6FC458E0 group-list object {base, head, tail}; the walk starts at the tail (0x458E8).
uint32_t g_stormEvtList458e0[3];

// 1.13c Storm+0x34410: find (or create, via 0x15E0) the list node whose +8 is `key`, then run
// 0x35CD0(command, key, a, b). ponytail: EDI at the 0x15E0 call is not visible in the decompile;
// insert mode 1 (tail) matches the tail-first walk.
extern "C" void Storm_Sub34410_Real(uint32_t command, uint32_t a, uint32_t b, uint32_t key) {
    uint32_t tail = g_stormEvtList458e0[2];
    uint32_t n = (int32_t)tail < 1 ? 0 : tail;
    bool found = false;
    for (; (int32_t)n > 0; n = *(uint32_t*)(uintptr_t)(n + 4)) {
        if (*(uint32_t*)(uintptr_t)(n + 8) == key) { found = true; break; }
    }
    if (!found) Storm_Sub15E0(g_stormEvtList458e0, 0, 0, 1)[2] = key;
    Storm_Sub35CD0_Real(command, key, a, b);
}
// 1.13c Storm+0x344D0 (#515)
extern "C" void Storm_Sub344D0_Real(uint32_t, uint32_t a, uint32_t b, uint32_t key) { Storm_Sub34410_Real(0x534d5347, a, b, key); }
// 1.13c Storm+0x344F0 (#514)
extern "C" void Storm_Sub344F0_Real(uint32_t, uint32_t a, uint32_t b, uint32_t key) { Storm_Sub34410_Real(0x534d534a, a, b, key); }
// 1.13c Storm+0x34510 (#513)
extern "C" void Storm_Sub34510_Real(uint32_t, uint32_t a, uint32_t b, uint32_t key) { Storm_Sub34410_Real(0x534d5349, a, b, key); }
// 1.13c Storm+0x34530 (#512)
extern "C" void Storm_Sub34530_Real(uint32_t, uint32_t a, uint32_t b, uint32_t key) { Storm_Sub34410_Real(0x534d5348, a, b, key); }

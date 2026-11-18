// 1.13c Storm SEVT command senders 0x34410 and 0x344D0/0x344F0/0x34510/0x34530 (ordinals 512-515).
// Real extern "C" bodies; ESI (the group key) is an explicit parameter.
#include <cstdint>

extern "C" uint32_t* Storm_Sub15E0(uint32_t* list, uint32_t size, uint32_t flags, uint32_t mode);
extern "C" uint32_t Storm_Sub35CD0_Real(uint32_t, uint32_t, uint32_t, uint32_t);

// 0x6FC458E0 group-list object {base, head, tail}; the walk starts at the tail (0x458E8).
uint32_t g_stormEvtList458e0[3];

// 1.13c Storm+0x34410: find (or create, via 0x15E0, EDI = 2) the list node whose +8 is `key`, then run
// 0x35CD0(command, key, a, b). `head` is the word at 0x558E8; the callees are seams so FUNCTION mode can record them.
extern "C" void Storm_Sub34410_Env(uint32_t command, uint32_t a, uint32_t b, uint32_t key, uint32_t head, void* list,
                                   uint32_t* (*f_15e0)(void*, int32_t, uint32_t), uint32_t (*f_35cd0)(uint32_t, uint32_t, uint32_t, uint32_t)) {
    uint32_t n = (int32_t)head < 1 ? 0 : head;
    bool found = false;
    for (; (int32_t)n > 0; n = *(uint32_t*)(uintptr_t)(n + 4)) {
        if (*(uint32_t*)(uintptr_t)(n + 8) == key) { found = true; break; }
    }
    if (!found) f_15e0(list, 0, 0)[2] = key;
    f_35cd0(command, key, a, b);
}
static uint32_t* Seam15E0(void* list, int32_t, uint32_t) { return Storm_Sub15E0((uint32_t*)list, 0, 0, 2); }
extern "C" void Storm_Sub34410_Real(uint32_t command, uint32_t a, uint32_t b, uint32_t key) {
    Storm_Sub34410_Env(command, a, b, key, g_stormEvtList458e0[2], g_stormEvtList458e0, Seam15E0, Storm_Sub35CD0_Real);
}
// 0x344D0 (#515), 0x344F0 (#514), 0x34510 (#513), 0x34530 (#512): stack (key, a, b); each saves ESI, loads the key from its first stack
// argument into ESI (push esi; mov esi,[esp+8]) and sends its own command through 0x34410. The fourth parameter (the caller's ESI) is unused.
extern "C" void Storm_Sub344xx_Env(int which, uint32_t a, uint32_t b, uint32_t key, uint32_t head, void* list,
                                   uint32_t* (*f_15e0)(void*, int32_t, uint32_t), uint32_t (*f_35cd0)(uint32_t, uint32_t, uint32_t, uint32_t)) {
    static const uint32_t cmd[4] = { 0x534d5347, 0x534d534a, 0x534d5349, 0x534d5348 };
    Storm_Sub34410_Env(cmd[which], a, b, key, head, list, f_15e0, f_35cd0);
}
static void Send344xx(int which, uint32_t a, uint32_t b, uint32_t key) {
    Storm_Sub344xx_Env(which, a, b, key, g_stormEvtList458e0[2], g_stormEvtList458e0, Seam15E0, Storm_Sub35CD0_Real);
}
// 1.13c Storm+0x344D0 (#515)
extern "C" void Storm_Sub344D0_Real(uint32_t key, uint32_t a, uint32_t b, uint32_t) { Send344xx(0, a, b, key); }
// 1.13c Storm+0x344F0 (#514)
extern "C" void Storm_Sub344F0_Real(uint32_t key, uint32_t a, uint32_t b, uint32_t) { Send344xx(1, a, b, key); }
// 1.13c Storm+0x34510 (#513)
extern "C" void Storm_Sub34510_Real(uint32_t key, uint32_t a, uint32_t b, uint32_t) { Send344xx(2, a, b, key); }
// 1.13c Storm+0x34530 (#512)
extern "C" void Storm_Sub34530_Real(uint32_t key, uint32_t a, uint32_t b, uint32_t) { Send344xx(3, a, b, key); }

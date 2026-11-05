// 1.13c Storm+0x38130 (stdcall obj, ret 4): release a blit object's streams. The two 3-word slots at obj+0x18: a set slot's stream goes through
// 0x1090 (SCODE release) and SMemFree(".?AUSTREAM@@", -2) under the AUSTREAM lock 0x6FC45858; every slot is then zeroed. When obj+0x30 is set, the
// 0x100 entries (stride 12, stream at +8) are released the same way, the table is freed (SBLT.CPP, 0x72) and obj+0x30 cleared. Words are 32-bit.
#include <cstdint>
#include <Windows.h>

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" void storm_port_1090(uint32_t* r);   // r[0] = node: Storm+0x1090
CRITICAL_SECTION* Storm_StreamLockCS();

// FUNCTION-mode probes route 0x1090 here (null in the game: the port).
extern "C" void (*g_storm38130_release)(void*) = nullptr;

static void release38130(uint32_t s) {
    EnterCriticalSection(Storm_StreamLockCS());
    if (g_storm38130_release) g_storm38130_release((void*)(uintptr_t)s);
    else { uint32_t r[5] = { s, 0, 0, 0, 0 }; storm_port_1090(r); }
    Storm_Sub1B960((void*)(uintptr_t)s, ".?AUSTREAM@@", -2);
    LeaveCriticalSection(Storm_StreamLockCS());
}

extern "C" void Storm13c_38130(uint32_t* obj) {
    uint32_t* slot = obj + 6;
    for (int i = 0; i < 2; i++, slot += 3) {
        if (slot[0]) release38130(slot[0]);
        slot[0] = slot[1] = slot[2] = 0;
    }
    if (obj[12]) {
        for (uint32_t o = 0; o < 0xc00; o += 0xc) {
            const uint32_t s = *(const uint32_t*)(uintptr_t)(obj[12] + o + 8);
            if (s) release38130(s);
        }
        Storm_Sub1B960((void*)(uintptr_t)obj[12], "..\\3rdParty\\STORM\\SOURCE\\SBLT.CPP", 0x72);
        obj[12] = 0;
    }
}

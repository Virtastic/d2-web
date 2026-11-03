#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Client+0x119C0: the keybind-assign tail. The caller's EAX is the key
// (pushed whole as the third argument of 0x3A270). out = 0; ok = 0x3A270(ECX = entry, EAX = &out, stack mod, key).
// ok != 0: 0x114F0(1) (cdecl). ok == 0: when the low word of out is nonzero, lastWord (0x6FBCC54C, 16-bit store) = it
// and lastTick (0x6FBCC550) = GetTickCount(). Always returns nothing.
// Globals: 0x6FBA8B94 table base (entry stride 40, the entry value is its first dword), 0x6FBCC538 idx,
// 0x6FBA8BE8 mod, 0x6FBCC54C lastWord, 0x6FBCC550 lastTick; import slot 0x6FB7EF5C = GetTickCount.
struct D2c119c0Env {
    uint32_t (*c3a270)(uint32_t entry, uint32_t* out, uint32_t mod, uint32_t key);
    void (*c114f0)(uint32_t arg);
    uint32_t (*tick)();
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_119c0(uint32_t key, const uint8_t* table, uint32_t idx, uint32_t mod,
                                                     uint16_t* lastWord, uint32_t* lastTick, const D2c119c0Env* env) {
    uint32_t out = 0, entry;
    memcpy(&entry, table + idx * 40u, 4);
    if (env->c3a270(entry, &out, mod, key)) { env->c114f0(1); return; }
    if ((uint16_t)out != 0) { *lastWord = (uint16_t)out; *lastTick = env->tick(); }
}

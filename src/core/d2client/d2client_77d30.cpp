// 1.13c D2Client+0x77D30 (69 B, no arguments): the cel-file slot at table D2Client+0xE2500 + idx * 0x22 (idx = the byte at
// D2Client+0x11991C; the dword is unaligned). A set slot is closed through import 0xC18A (stdcall, 1 word), released through
// the Fog free thunk 0xBF86 (ECX = slot, EDX = the 0xD6AB0 source string, line 0x7A, 0) and zeroed (the index byte is read again).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c77D30Env {
    void (*closeFile)(uint32_t handle);                                                     // 0xC18A
    void (*fogFree)(uint32_t p, uint32_t sourceRva, uint32_t line, uint32_t zero);          // 0xBF86
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_77d30(uint8_t* table_e2500, const uint8_t* idx_11991c, const D2c77D30Env* env) {
    uint32_t v;
    std::memcpy(&v, table_e2500 + (uint32_t)*idx_11991c * 0x22u, 4);
    if (!v) return;
    env->closeFile(v);
    env->fogFree(v, 0xD6AB0, 0x7a, 0);
    const uint32_t zero = 0;
    std::memcpy(table_e2500 + (uint32_t)*idx_11991c * 0x22u, &zero, 4);
}

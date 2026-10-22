// src/core/d2cmp/d2cmp_9040.cpp -- 1.13c D2CMP.dll 0x9040, transcribed from the cached Ghidra decompile.
// Alpha lane: implementation + marker only. Host seams stand for Fog #10103 (file close), #10265/#10024 (error text) + _exit(-1).
#include <cstdint>

struct D2CMP9040Env {
    void (*closeFile)(void* f);       // Fog #10103
    void (*fatal)(uint32_t msgId);    // Fog #10265(id) + #10024(str,..) + _exit(-1); does not return
};

// 1.13c D2CMP+0x09040 (37 bytes): close a file handle; a null handle is fatal (string 0x49).
// 1.13c D2CMP+0x09040
extern "C"
void d2cmp_9040(const D2CMP9040Env* env, void* f) {
    if (!f) env->fatal(0x49);
    env->closeFile(f);
}

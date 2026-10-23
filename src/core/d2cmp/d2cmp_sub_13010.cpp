// src/core/d2cmp/d2cmp_sub_13010.cpp -- 1.13c D2CMP.dll sub_13010 from the cached Ghidra decompile
// (build-wasm/ghidra-d2cmp/decomp/0x13010.c). Alpha lane: no vector. Prior art: none.
// Fog #10102/10104/10105/10103/10265/10024 are seams in Env.
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct D2CMP13010Env {
    void* (*open)();                                   // Fog #10102: handle, null on failure
    int (*read)(uint32_t n, void* buf);                // Fog #10104: 1 ok; *got via readCount
    uint32_t (*readCount)();                           // bytes the last read delivered
    uint32_t (*tell)();                                // Fog #10105
    void (*close)();                                   // Fog #10103
    void (*fatal)(uint32_t id);                        // Fog #10265 + #10024, then _exit(-1)
};

// 1.13c D2CMP+0x13010 (243 bytes): reads a 15-byte tile-library header ('t' tag at +0, count at +2,
// dwords at +3/+7), fills out[4..0x1b], reads count*4 offsets, stores the tell result at
// out[0x1c + count*4], sets flag 2. count > 0x20 is fatal (id 0x50a). Returns 1 ok, 0 failed.
// 1.13c D2CMP+0x13010
extern "C"
uint32_t d2cmp_sub_13010(uint8_t* out, const D2CMP13010Env* env) {
    if (!env->open()) return 0;
    uint8_t h[15] = {0};
    if (env->read(0xf, h) && env->readCount() == 0xf && h[0] == 't') {
        uint32_t n = h[2], v;
        std::memset(out + 0xc, 0, 4);
        out[4] = h[1]; out[5] = out[6] = out[7] = 0;
        std::memcpy(&v, h + 7, 4); std::memcpy(out + 8, &v, 4);
        out[0x14] = h[2]; out[0x15] = out[0x16] = out[0x17] = 0;
        std::memcpy(out + 0x18, h + 3, 4);
        if (n > 0x20) { env->fatal(0x50a); _Exit(-1); }
        if (env->read(n * 4, nullptr) && env->readCount() == n * 4) {
            uint32_t t = env->tell();
            std::memcpy(out + 0x1c + n * 4, &t, 4);
            env->close();
            out[0] |= 2;
            return 1;
        }
    }
    env->close();
    return 0;
}

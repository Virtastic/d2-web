// 1.13c D2CMP+0x9B00 (348 bytes): load the 256-colour palette of a PCX file into 0x6FE87F18.
#include <cstring>
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09B00
// EAX = file name. Open failure: "Unable to open ", name, "\n" to the debugger, return 0. The
// 0x80-byte header must read in full with manufacturer 0x0A, version 5, 8 bits per pixel, else
// "File is not a valid 256-color PCX file.\n". The last 0x301 bytes must read in full and start
// with 0x0C, else "Unable to extract palette from\n". On success entry i bytes 0..2 = the file's
// R,G,B (byte 3 untouched), close, return 1. Every failure after the open closes and returns 0.
extern "C" uint32_t d2cmp_9b00(const D2CmpPalEnv* env, const char* name) {
    intptr_t h = env->open(name);
    if (h == -1) { env->debug("Unable to open "); env->debug(name); env->debug("\n"); return 0; }
    uint8_t hdr[0x80] = {};
    uint32_t got = env->read(h, hdr, 0x80);
    const char* msg;
    if (got == 0x80 && hdr[0] == 0x0a && hdr[1] == 5 && hdr[3] == 8) {
        uint8_t tail[0x301] = {};
        env->seekEnd(h, -0x301);
        got = env->read(h, tail, 0x301);
        if (got == 0x301 && tail[0] == 0x0c) {
            for (uint32_t i = 0; i < 0x100; ++i) std::memcpy(&g_d2cmp_pal_6fe87f18[i * 4], &tail[1 + i * 3], 3);
            env->close(h);
            return 1;
        }
        msg = "Unable to extract palette from\n";
    } else {
        msg = "File is not a valid 256-color PCX file.\n";
    }
    env->debug(msg);
    env->close(h);
    return 0;
}

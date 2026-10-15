#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x119C0: out=0; ok = sub_3A270(eax=&out, [[0x6FBA8B94]+[0x6FBCC538]*40], [0x6FBA8BE8], out(=0));
// if ok then sub_114F0(1). Returns 1 when 114F0(1) is called. Input is the 0x3A270 result (not yet mapped).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_119c0(uint32_t ok3a270) { return ok3a270 != 0; }

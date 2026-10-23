// D2Client sub_AF510: env-seam port (callees injected, no retail data).
#include <cstdint>
#include <cstring>
typedef int32_t i32; typedef uint32_t u32;

struct D2cAF510Env {
    void (*f2A1B0)(u32 a, u32 b, u32 c);                  // D2Client+0x2A1B0 (fastcall ecx,edx + 1 stack)
    void (*f2A750)(u32 type, i32* a, u32* pt);            // D2Client+0x2A750 (ecx=type, edx=a; 3-word buf)
};

// 1.13c D2Client+0xAF510  (ecx = param_1, edx = packet ptr)
void d2c_af510(const D2cAF510Env* e, i32* param_1, const uint8_t* pkt) {
    uint16_t w7, w9, we; memcpy(&w7, pkt + 7, 2); memcpy(&w9, pkt + 9, 2); memcpy(&we, pkt + 0xe, 2);
    u32 buf[3] = { w7, w9, pkt[0xb] };   // local_1c, local_18, local_14
    e->f2A1B0(we, 0, 0);
    e->f2A750(pkt[6], param_1, buf);
}

// 1.13c D2Client+0x267B0: bottom-centre belt-toggle button, hover hint. Callees reach through the injected dispatcher.
#include "d2client_callee.h"
// 1.13c D2Client+0x267B0
extern "C" void d2c_267b0(const D2cSvc& v, int32_t w, int32_t h, int32_t mouseA /*0x11B828*/, int32_t mouseB /*0x11B824*/, uint32_t expanded /*0xFADD4*/) {
    const int32_t w2 = (w - (w >> 31)) >> 1;
    if (mouseA < w2 - 8 || mouseA > w2 + 5) return;
    if (mouseB < h - 0x27 || mouseB > h - 0xd) return;
    const uint32_t id = expanded ? 0x1048 : 0x1047, x = (uint32_t)(w2 - 1);
    const uint32_t t = d2c_call(v, 0xd1ec, w2, id, 0, 0, x);   // D2Lang #10003 string lookup (ECX = id; the three words pushed before it stay for the next call)
    d2c_call(v, 0xd384, 0, t, x, 0, x, (uint32_t)(h - 0x27), 0, 1);                      // D2Win #10085 hint draw (ECX text, EDX x, stack y 0 1)
}

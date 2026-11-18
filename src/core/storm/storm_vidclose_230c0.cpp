// 1.13c Storm+0x230C0 -- tear down a video stream object.
#include "storm_vidclose_230c0.h"
extern "C" uint32_t Storm_VidClose_230c0(const StormVidCloseEnv& e, uint32_t p) {
    if (!p) { *e.lastError = 0x57; e.setLastError(0x57); return 0; }
    if (!e.check(p)) return 0;
    uint32_t* w = e.node(p);
    const uint32_t q = w[0x58 / 4];
    if (!q) return 0;
    const uint32_t* qw = e.node(q);
    if (qw[0]) e.fnFree0(qw[0]);
    if (e.node(q)[1]) e.fnFree3(e.node(q)[1]);
    e.heapFree(q, 0x47474, 0x406);
    w[0x58 / 4] = 0;
    if (w[2] != 0xFFFFFFFFu) { e.closeHandle(w[2]); w[2] = 0xFFFFFFFFu; }
    if (w[0x4c / 4]) { e.heapFree(w[0x4c / 4], 0x47474, 0x40f); w[0x4c / 4] = 0; w[0x2c / 4] = 0; }
    if (w[0xa0 / 4]) e.heapFree(w[0xa0 / 4], 0x47474, 0x415);
    e.reset(p, 0x559e8);
    return 1;
}

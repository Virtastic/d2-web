// D2Client+0xABAC0: window-message filter (EAX=msg, ESI=unaff, EDX=param_2); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cABAC0Env {
    int32_t gate;                                                  // dword at D2Client+0xBCBBE0
    void (*fABA10)(uint32_t eax, int32_t esi, int32_t edx);        // D2Client+0xABA10
};
extern "C" {
// 1.13c D2Client+0xABAC0
EMSCRIPTEN_KEEPALIVE void d2c_abac0(const D2cABAC0Env* e, uint32_t msg, int32_t esi, int32_t edx) {
    if (!e->gate) return;
    switch (msg) {  // WM_MOUSEMOVE/LBUTTONDOWN/UP/RBUTTONDOWN/UP, WM_KEYDOWN/UP/CHAR, WM_COMMAND, 0x10 WM_CLOSE
    case 0x10: case 0x100: case 0x101: case 0x102: case 0x111:
    case 0x200: case 0x201: case 0x202: case 0x204: case 0x205:
        e->fABA10(msg, esi, edx);
    }
}
}

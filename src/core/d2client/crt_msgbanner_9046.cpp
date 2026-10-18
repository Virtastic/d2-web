// D2Client+0x09046 = MSVC 2003 Release CRT __FF_MSGBANNER(void), 57 bytes, pulled out of libcmt.
// Body (base 0x6FAB0000): if (__error_mode == 1 || (__error_mode == 0 && __app_type == 1)) {
// __NMSG_WRITE(0xFC); if (_pfnmsg != NULL) _pfnmsg(); __NMSG_WRITE(0xFF); } -- void, no arguments.
// Globals, all zero in the shipped .data: __error_mode +0x0FA8F8 (0x6FBAA8F8), __app_type +0x0FA8FC
// (0x6FBAA8FC), the host-installed hook _pfnmsg +0x0FAB84 (0x6FBAAB84). Callee __NMSG_WRITE is
// D2Client+0x08ECE -- the CRT diagnostic writer (stderr WriteFile / CRT message box), host state, so it
// crosses the D2cHost seam; the hook is a host function pointer and is called directly when non-NULL.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"

namespace {
constexpr uint32_t kNmsgWrite = 0x08ECE;   // __NMSG_WRITE
uint32_t g_error_mode;                     // +0x0FA8F8  __error_mode
uint32_t g_app_type;                       // +0x0FA8FC  __app_type
void (*g_pfnmsg)();                        // +0x0FAB84  _pfnmsg

void nmsg(D2cHost h, uint32_t msg) {
    D2cCall c = {};
    c.rva = kNmsgWrite;
    c.s[0] = msg;
    h(&c);
}
}  // namespace

// 1.13c D2Client+0x9046
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_9046(D2cHost h) {
    if (!(g_error_mode == 1 || (g_error_mode == 0 && g_app_type == 1))) return;
    nmsg(h, 0xFC);                         // _RT_MSG
    if (g_pfnmsg != nullptr) g_pfnmsg();
    nmsg(h, 0xFF);                         // _RT_FLOAT
}

// Scenario setup: pin the two mode words and the optional hook before invoking the banner.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_9046_pin(uint32_t error_mode, uint32_t app_type,
                                             void (*pfnmsg)()) {
    g_error_mode = error_mode;
    g_app_type = app_type;
    g_pfnmsg = pfnmsg;
}

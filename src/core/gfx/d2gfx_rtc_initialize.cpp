#include <emscripten.h>

// 1.13c D2Gfx+0x01AA2 __RTC_Initialize
// MSVC 2003 CRT: walks the RTC init-function table [DAT_6fa8e7b8, DAT_6fa8e7b8).
// Begin and end are the same address (no /RTC objects linked), so the loop body never runs.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Gfx_RTC_Initialize(void)
{
}

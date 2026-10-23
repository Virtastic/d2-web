// 1.13c Fog+0x086E0 -- CRT ANSI code page of a locale: GetLocaleInfoA(lcid, LOCALE_IDEFAULTANSICODEPAGE, buf, 6) then atol; -1 on failure.
#include <cstdint>
#include <cstdlib>

namespace {
// Host seam for GetLocaleInfoA(lcid, 0x1004, buf, 6); returns chars written (0 = failure). Null when absent.
using GetLocaleInfoFn = int (*)(uint32_t lcid, char* buf, int cch);
GetLocaleInfoFn g_getLocaleInfo = nullptr;
}

long fog_crtLocaleCodePage(uint32_t lcid) {
    char buf[7];
    buf[6] = 0;
    if (!g_getLocaleInfo || g_getLocaleInfo(lcid, buf, 6) == 0) return -1;
    return atol(buf);
}

// 1.13c D2Net+0x3376 setSBCS: reset the multibyte globals to the single-byte state.
// Retail: rep stosd 0x40 dwords + stosb over _mbctype (0xB460, 257 bytes), then 0 into the codepage (0xB564), the
// multibyte flag (0xB45C) and the locale id (0xB454), then 3 dwords at 0xB570. 0xB458 and the gaps are left alone.
#include <cstring>
#include "d2net_mbwin.h"
namespace d2net_3376 {
// 1.13c D2Net+0x3376
void setSBCS(d2net_mb::Window& w) {
    memset(w.mbctype, 0, sizeof w.mbctype);
    w.mbcodepage = 0;
    w.ismbcodepage = 0;
    w.lcid = 0;
    memset(w.mbulinfo, 0, 12);
}
}

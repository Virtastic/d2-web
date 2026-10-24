#include "d2common_crt.h"

// 1.13c D2Common+0x05E04
// __FF_MSGBANNER(void): when __error_mode (0x6fdef5f4) == 1, or it is 0 and
// __app_type (0x6fdef5f8) == 1: _NMSG_WRITE(0xFC); call the _pfnmsg hook (0x6fdef88c) if set;
// _NMSG_WRITE(0xFF).
extern "C" void d2common_ff_msgbanner_5e04(void)
{
    if (g_d2common_crt.errorMode == 1 || (g_d2common_crt.errorMode == 0 && g_d2common_crt.appType == 1)) {
        d2common_nmsg_write_5c8c(0xfc);
        if (g_d2common_crt.pfnmsg) g_d2common_crt.pfnmsg();
        d2common_nmsg_write_5c8c(0xff);
    }
}

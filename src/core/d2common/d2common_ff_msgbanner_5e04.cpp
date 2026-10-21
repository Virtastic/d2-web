#include <cstdint>

// 1.13c D2Common+0x05E04 __FF_MSGBANNER(void): when __error_mode (0x6fdef5f4) == 1, or it is 0 and
// __app_type (0x6fdef5f8) == 1: _NMSG_WRITE(0xFC); call the _pfnmsg hook (0x6fdef88c) if set;
// _NMSG_WRITE(0xFF). Env seam: _NMSG_WRITE (+0x5C8C) is a host hook.
struct D2CommonFFMsgBannerEnv {
    int32_t errorMode;
    int32_t appType;
    void (*pfnmsg)();
    void (*nmsgWrite)(int32_t);
};

// 1.13c D2Common+0x05E04
void d2common_ff_msgbanner_5e04(const D2CommonFFMsgBannerEnv& e)
{
    if (e.errorMode == 1 || (e.errorMode == 0 && e.appType == 1)) {
        e.nmsgWrite(0xfc);
        if (e.pfnmsg) e.pfnmsg();
        e.nmsgWrite(0xff);
    }
}

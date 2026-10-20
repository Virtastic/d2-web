#include <cstdint>
#include <cstdlib>

// 1.13c D2Common+0x02D73 __amsg_exit(msg): when __error_mode == 1, or it is 0 and __app_type == 1,
// write the banner (__FF_MSGBANNER, +0x1B1E); write message msg (+0x5C8C); then exit via the
// __exit pointer with 0xFF. Env seam: banner/message writers are host hooks, exit is _Exit.
struct D2CommonAmsgEnv {
    int32_t errorMode;  // DAT 0x6fdef5f4
    int32_t appType;    // DAT 0x6fdef5f8
    void (*msgBanner)();
    void (*writeMsg)(int32_t);
    void (*exitFn)(int32_t);
};

void d2common_amsg_exit_2d73(const D2CommonAmsgEnv& e, int32_t msg)
{
    if (e.errorMode == 1 || (e.errorMode == 0 && e.appType == 1))
        e.msgBanner();
    e.writeMsg(msg);
    if (e.exitFn) e.exitFn(0xff); else std::_Exit(0xff);
}

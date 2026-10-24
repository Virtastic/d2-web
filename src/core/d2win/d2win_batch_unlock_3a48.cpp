// SEH cleanup funclets of the CRT low-level file I/O; each unlocks the descriptor the parent frame holds in EBX.
#include <cstdint>
#include <emscripten.h>
struct D2WinSmallEnv;
extern "C" void d2win_sub_69DC(int32_t fd, const D2WinSmallEnv *e);

// 1.13c D2Win+0x03A48 -- funclet of the frame at 0x39C1; fd = parent's EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_3A48(int32_t fd, const D2WinSmallEnv *e) { d2win_sub_69DC(fd, e); }
// 1.13c D2Win+0x03CAF -- funclet of the frame at 0x3C28; fd = parent's EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_3CAF(int32_t fd, const D2WinSmallEnv *e) { d2win_sub_69DC(fd, e); }

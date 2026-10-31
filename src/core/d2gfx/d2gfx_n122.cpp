// D2Gfx 1.13c: two 8-byte CRT thunks. Each is `push ebx; call __unlock_fhandle(0x5D74); pop ecx; ret`,
// i.e. the handle arrives in EBX (caller's register) and is forwarded as the cdecl argument.
#include <cstdint>

// host seam: __unlock_fhandle 0x5D74. Null until a host installs it; the FN probes install a recorder.
extern "C" void (*g_d2gfx_unlock_fhandle)(int32_t handle) = nullptr;

// 1.13c D2GFX+0x03CF2
extern "C" void d2gfx_unlock_fhandle_thunk_3cf2(int32_t handle) { g_d2gfx_unlock_fhandle(handle); }

// 1.13c D2GFX+0x03F59
extern "C" void d2gfx_unlock_fhandle_thunk_3f59(int32_t handle) { g_d2gfx_unlock_fhandle(handle); }

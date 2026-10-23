// 1.13c D2sound+0x0C200 -- MSVC CRT x87 math-error shim (callers 0x9D4D, 0xC349).
// Spills the register args and ST0 into a stack frame and forwards (&frame, &param_3) to CRT
// helper 0xCEFB (float exception dispatch, _matherr path). Pure CRT plumbing: no game state,
// ST0 is returned unchanged. wasm has no x87 and no float in our code, so ours is a no-op.
#include <cstdint>
void d2s_crt_c200() {}

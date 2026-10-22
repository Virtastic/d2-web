// BNClient.dll alpha-lane row 0x58FD (decomp/0x58FD.c): __NLG_Notify1, same body as 0x5906.
#include <cstdint>
#include <emscripten.h>

// +0x3D634/38/3C: the NLG notifier's saved EAX, ECX argument and EBP (separate from 0x5906's statics).
static uint32_t g_nlg1_eax, g_nlg1_param, g_nlg1_ebp;

// 1.13c BNClient+0x058FD (9 B, caller 0x182B0): __NLG_Notify1 (__fastcall) -- store ECX at +0x3D638,
// EAX at +0x3D634, EBP at +0x3D63C. EAX/EBP ride as extra arguments.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_58fd(uint32_t param, uint32_t eax, uint32_t ebp) {
    g_nlg1_param = param;
    g_nlg1_eax = eax;
    g_nlg1_ebp = ebp;
}

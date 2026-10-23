// D2Launch batch 15 -- transcribed from build-wasm/ghidra-d2launch/decomp (base 0x6FA40000).
#include <cstdint>

static int32_t g_launch_9b70_value;  // DAT_6fa65efc

// 1.13c D2Launch+0x9B70 -- stores its argument in a global, returns 1 (caller 0x1A87).
extern "C" int32_t d2launch_sub_9b70(int32_t v) { g_launch_9b70_value = v; return 1; }

// 1.13c D2Launch+0x93E6 -- GetLocaleInfoA(lcid, 0x1004 = LOCALE_IDEFAULTANSICODEPAGE) then atol; -1 on failure.
// ponytail: no Win32 locale in wasm; the failure result (-1) is the only reachable one.
extern "C" int32_t d2launch_sub_93e6(uint32_t lcid) { (void)lcid; return -1; }

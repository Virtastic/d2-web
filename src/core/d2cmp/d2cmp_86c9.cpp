#include <cstdint>
int32_t d2cmp_8641(const char* s);

// 1.13c D2CMP+0x086C9
// CRT: ANSI code page of a locale = atol(GetLocaleInfoA(lcid, LOCALE_IDEFAULTANSICODEPAGE 0x1004, buf, 6)); -1 if the call fails.
// ponytail: no Win32 locale in wasm; getInfo returns the string (NUL-terminated, <=5 chars) or nullptr for failure.
int32_t d2cmp_86c9(const char* (*getInfo)(uint32_t lcid), uint32_t lcid) {
    const char* s = getInfo ? getInfo(lcid) : nullptr;
    return s ? d2cmp_8641(s) : -1;
}

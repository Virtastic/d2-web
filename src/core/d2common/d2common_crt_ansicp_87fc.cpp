#include <cstdint>
#include <cstdlib>

// 1.13c D2Common+0x087FC (71 B, callers 0x4EF4/0x64D3): ANSI codepage of a locale.
struct D2CommonAnsiCpEnv {
    int32_t (*getLocaleInfoA)(uint32_t locale, uint32_t type, char* buf, int32_t cch);  // GetLocaleInfoA
};

extern "C" int32_t d2common_locale_ansi_codepage_87fc(uint32_t locale, const D2CommonAnsiCpEnv* env)
{
    char buf[6] = {0, 0, 0, 0, 0, 0};
    if (env->getLocaleInfoA(locale, 0x1004, buf, 6) == 0) return -1;  // LOCALE_IDEFAULTANSICODEPAGE
    return (int32_t)std::strtol(buf, nullptr, 10);                    // _atol 0x1E33
}

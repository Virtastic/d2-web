#include <cstdint>
#include <cstring>

// 1.13c D2Common+0x064D3 (421 B, callers 0x31F9/0x6DB8) ___crtGetStringTypeA(plocinfo, infoType, src, cch,
// out, code_page, bError). Same CRT routine as BNClient+0x6D18. First call probes GetStringTypeW(1, L"", 1):
// success -> wide state 1, ERROR_CALL_NOT_IMPLEMENTED (0x78) -> 2, otherwise stays 0. State 2/0: locale is
// code_page or the default (DAT 0x6fdef920), target is `out` or the default ctype table (DAT 0x6fdef930);
// the locale's ANSI codepage (0x87FC) of -1 falls out with 0; when it differs from the target codepage the
// string is converted first (0x8843, null -> 0, temporary freed), then GetStringTypeA answers. State 1:
// MultiByteToWideChar(cp, bError?9:1) sizes a zeroed buffer (stack, or heap 0x52CA when large), the fill call
// follows, GetStringTypeW answers; a failed conversion returns 0. State word is DAT 0x6fdef8e4.
// Win32 and CRT calls are host seams.
struct D2CommonCrtGstEnv {
    uint32_t defaultLocale;                                   // DAT 0x6fdef920
    uint16_t* defaultCtype;                                   // DAT 0x6fdef930
    uint32_t (*getLastError)();
    int32_t (*getStringTypeW)(uint32_t type, const uint16_t* src, int32_t cch, uint16_t* out);
    int32_t (*getStringTypeA)(uint32_t locale, uint32_t type, const char* src, int32_t cch, uint16_t* out);
    uint32_t (*multiByteToWideChar)(uint32_t cp, uint32_t flags, const char* src, int32_t cb, uint16_t* out, int32_t cch);
    int32_t (*localeAnsiCodepage)(uint32_t locale);           // 0x87FC
    char* (*convert)(uint32_t from, uint32_t to, const char* src, uint32_t* pcb);  // 0x8843
    void* (*alloc)(uint32_t bytes);                           // 0x52CA / _malloca heap
    void (*free)(void* p);
};
static uint32_t g_d2c_gst_wide_state;  // DAT 0x6fdef8e4

extern "C" int32_t d2common_crt_get_string_type_a_64d3(uint32_t plocinfo, uint32_t infoType, const char* src,
        int32_t cch, uint16_t* out, uint32_t codePage, int32_t bError, const D2CommonCrtGstEnv* env)
{
    (void)plocinfo;
    if (g_d2c_gst_wide_state == 0) {
        uint16_t probe[2] = {0, 0};
        if (env->getStringTypeW(1, (const uint16_t*)u"", 1, probe) != 0) g_d2c_gst_wide_state = 1;
        else if (env->getLastError() == 0x78) g_d2c_gst_wide_state = 2;
    }
    uint16_t* dest = out ? out : env->defaultCtype;
    if (g_d2c_gst_wide_state != 1) {
        uint32_t locale = codePage ? codePage : env->defaultLocale;
        int32_t acp = env->localeAnsiCodepage(locale);
        if (acp != -1) {
            char* conv = nullptr;
            if ((uint32_t)acp != codePage) {
                uint32_t n = (uint32_t)cch;
                conv = env->convert(codePage, (uint32_t)acp, src, &n);
                if (!conv) return 0;
                cch = (int32_t)n;
            }
            int32_t rc = env->getStringTypeA(locale, infoType, conv ? conv : src, cch, dest);
            if (conv) env->free(conv);
            return rc;
        }
        return 0;
    }
    int32_t n = (int32_t)env->multiByteToWideChar(codePage, (bError != 0) * 8 + 1, src, cch, nullptr, 0);
    if (n == 0) return 0;
    uint16_t* w = (uint16_t*)env->alloc(((uint32_t)n * 2 + 3) & ~3u);
    if (!w) return 0;
    std::memset(w, 0, (size_t)n * 2);
    int32_t rc = 0;
    if (env->multiByteToWideChar(codePage, 1, src, cch, w, n) != 0)
        rc = env->getStringTypeW(infoType, w, n, dest);
    env->free(w);
    return rc;
}

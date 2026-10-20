#include <cstdint>
#include <emscripten.h>
#include "d2gfx_atol_7231.h"

struct D2GfxLocaleInfoHost {
    // GetLocaleInfoA(locale, type, buf, cch): chars written incl. NUL, 0 on failure.
    int32_t (*get_locale_info_a)(uint32_t locale, uint32_t type, char* buf, int32_t cch);
    const D2GfxAtolHost* atol_host;
};

// 1.13c D2Gfx+0x072B9 (71 B): LOCALE_IDEFAULTANSICODEPAGE (0x1004) of a locale as a number;
// -1 when GetLocaleInfoA fails. The buffer is 6 bytes with byte 5 pre-cleared.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_locale_ansi_cp_72b9(
        uint32_t locale, const D2GfxLocaleInfoHost* host) {
    char buf[6] = {0, 0, 0, 0, 0, 0};
    if (host->get_locale_info_a(locale, 0x1004, buf, 6) == 0) return -1;
    return d2gfx_atol_7231(buf, host->atol_host);
}

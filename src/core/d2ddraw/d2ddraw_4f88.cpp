// src/core/d2ddraw/d2ddraw_4f88.cpp -- 1.13c D2DDraw.dll ___crtGetStringTypeA, from the retail disassembly (the decompile
// shifts the arguments). Seams in d2ddraw_cp.h.
#include <cstring>
#include "d2ddraw_cp.h"

// 1.13c D2DDraw+0x4F88 (cdecl: type, src, cch, out, codePage, lcid, bError)
// The first call probes the wide API with GetStringTypeW(1, L"", 1, &w): success selects it (1); ERROR_CALL_NOT_IMPLEMENTED
// (0x78) selects the ANSI API (2); any other failure leaves the probe at 0, which also takes the ANSI path, so the probe repeats.
// ANSI: lcid or the default locale, codePage or the default code page; when the locale's ANSI code page (0x5919, -1 fails)
// differs, the text goes through 0x5960 first and the converted copy is freed after GetStringTypeA. Wide: a counting
// MultiByteToWideChar (flags 9 with bError, else 1; 0 fails) sizes a zeroed stack buffer, the filling call (flags 1) gives the
// count GetStringTypeW is handed; a failed fill answers 0.
extern "C" int32_t d2ddraw_crt_get_string_type_a_4f88(const D2DDrawCpHost* h, uint32_t type, const char* src, int32_t cch,
                                                      uint16_t* out, uint32_t codePage, uint32_t lcid, int32_t bError) {
    if (*h->useWide == 0) {
        uint16_t w[2];
        if (h->getStringTypeW(1, h->emptyW, 1, w)) *h->useWide = 1;
        else if (h->getLastError() == 0x78) *h->useWide = 2;
    }
    const uint32_t mode = *h->useWide;
    if (mode == 2 || mode == 0) {
        const uint32_t loc = lcid ? lcid : *h->defLcid;
        const uint32_t cp = codePage ? codePage : *h->defCodePage;
        const uint32_t ansi = h->ansiCodePage(loc);
        if (ansi == 0xFFFFFFFFu) return 0;
        char* conv = nullptr;
        if (ansi != cp) {
            conv = h->convert(h, cp, ansi, src, (uint32_t*)&cch, nullptr, 0);
            if (!conv) return 0;
            src = conv;
        }
        const int32_t rc = h->getStringTypeA(loc, type, src, cch, out);
        if (conv) h->free(conv);
        return rc;
    }
    if (mode != 1) return 0;
    if (!codePage) codePage = *h->defCodePage;
    const int32_t n = h->multiByteToWideChar(codePage, bError ? 9u : 1u, src, cch, nullptr, 0);
    if (!n) return 0;
    uint16_t* w = (uint16_t*)__builtin_alloca(((uint32_t)n * 2 + 3) & ~3u);   // retail: _alloca (0x2710) + memset
    memset(w, 0, (uint32_t)n * 2);
    int32_t rc = 0;
    const int32_t got = h->multiByteToWideChar(codePage, 1, src, cch, w, n);
    if (got) rc = h->getStringTypeW(type, w, got, out);
    return rc;
}

// src/core/d2ddraw/d2ddraw_5960.cpp -- 1.13c D2DDraw.dll CRT code-page converter (the __convertcp shape), from the retail
// disassembly. Seams in d2ddraw_cp.h.
#include <cstring>
#include "d2ddraw_cp.h"

// 1.13c D2DDraw+0x5960 (cdecl: from, to, src, pcb, out, outN)
// Equal pages answer null. Both pages single-byte (GetCPInfo MaxCharSize 1; the second page is asked only when the first is)
// take the wide length from *pcb, or strlen+1 when it is -1; otherwise a counting MultiByteToWideChar sizes it (0 fails).
// The text is widened into a zeroed stack buffer. With an out buffer one WideCharToMultiByte fills it and answers it; without,
// a fresh _calloc_crt(1, n) block (n counted by WideCharToMultiByte unless both pages are single-byte) is filled and answered,
// its byte count written back through pcb unless *pcb was -1; a failed fill frees it and answers null.
extern "C" char* d2ddraw_convert_cp_5960(const D2DDrawCpHost* h, uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                                         char* out, int32_t outN) {
    const uint32_t cb = *pcb;
    if (from == to) return nullptr;
    bool single = false;
    if (h->getCPInfoMaxChar(from) == 1 && h->getCPInfoMaxChar(to) == 1) single = true;
    int32_t n;
    if (single) n = cb != 0xFFFFFFFFu ? (int32_t)cb : (int32_t)strlen(src) + 1;
    else {
        n = h->multiByteToWideChar(from, 1, src, (int32_t)cb, nullptr, 0);
        if (!n) return nullptr;
    }
    uint16_t* w = (uint16_t*)__builtin_alloca(((uint32_t)n * 2 + 3) & ~3u);   // retail: _alloca (0x2710) + memset
    memset(w, 0, (uint32_t)n * 2);
    char* res = nullptr;
    if (!h->multiByteToWideChar(from, 1, src, (int32_t)cb, w, n)) return nullptr;
    if (out) {
        if (h->wideCharToMultiByte(to, 0, w, n, out, outN, nullptr, nullptr)) res = out;
        return res;
    }
    if (!single) {
        n = h->wideCharToMultiByte(to, 0, w, n, nullptr, 0, nullptr, nullptr);
        if (!n) return nullptr;
    }
    res = (char*)h->calloc(1, (uint32_t)n);
    if (!res) return nullptr;
    const int32_t got = h->wideCharToMultiByte(to, 0, w, n, res, n, nullptr, nullptr);
    if (!got) { h->free(res); res = nullptr; }
    else if (cb != 0xFFFFFFFFu) *pcb = (uint32_t)got;
    return res;
}

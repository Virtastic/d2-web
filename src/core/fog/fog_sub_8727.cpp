// 1.13c Fog+0x08727 -- CRT __convertcp: convert a string between two code pages via UTF-16.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {
// Host seams for the Win32 NLS calls (null on wasm: conversion then fails, returns null).
struct FogNls {
    int (*maxCharSize)(uint32_t cp);  // GetCPInfo MaxCharSize, 0 on failure
    int (*mbToWide)(uint32_t cp, const char* s, int n, uint16_t* out, int outN);
    int (*wideToMb)(uint32_t cp, const uint16_t* s, int n, char* out, int outN);
};
FogNls g_nls = {};
}

// Returns outBuf (when given), else a malloc'd buffer (caller frees), else null. *pLen is updated
// to the converted byte count only for the allocating path with explicit length.
char* fog_convertcp(uint32_t from, uint32_t to, const char* src, uint32_t* pLen, char* outBuf, int outN) {
    if (from == to || !g_nls.mbToWide) return nullptr;
    uint32_t cb = *pLen;
    bool sbcs = g_nls.maxCharSize(from) == 1 && g_nls.maxCharSize(to) == 1;
    uint32_t wn = cb;
    if (sbcs) { if (cb == 0xffffffffu) wn = (uint32_t)strlen(src) + 1; }
    else { wn = (uint32_t)g_nls.mbToWide(from, src, (int)cb, nullptr, 0); if (!wn) return nullptr; }
    std::vector<uint16_t> w(wn);
    if (!g_nls.mbToWide(from, src, (int)cb, w.data(), (int)wn)) return nullptr;
    if (outBuf)
        return g_nls.wideToMb(to, w.data(), (int)wn, outBuf, outN) ? outBuf : nullptr;
    uint32_t bn = wn;
    if (!sbcs) { bn = (uint32_t)g_nls.wideToMb(to, w.data(), (int)wn, nullptr, 0); if (!bn) return nullptr; }
    char* r = (char*)malloc(bn);
    if (!r) return nullptr;
    int got = g_nls.wideToMb(to, w.data(), (int)wn, r, (int)bn);
    if (!got) { free(r); return nullptr; }
    if (cb != 0xffffffffu) *pLen = (uint32_t)got;
    return r;
}

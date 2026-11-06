// src/core/d2ddraw/d2ddraw_cp.h -- the host seams of the 1.13c D2DDraw.dll statically linked CRT code-page rows
// (0x4F88 ___crtGetStringTypeA, 0x5960 the code-page converter). Read from the retail disassembly: the decompile of 0x4F88
// shifts the stack arguments past its SEH prolog. Imports are KERNEL32 (IAT slots: 0xB0D4 GetStringTypeW, 0xB100 GetLastError,
// 0xB134 MultiByteToWideChar, 0xB0CC GetStringTypeA, 0xB0BC GetCPInfo, 0xB114 WideCharToMultiByte).
#pragma once
#include <cstdint>

struct D2DDrawCpHost {
    int32_t (*getStringTypeW)(uint32_t type, const uint16_t* src, int32_t n, uint16_t* out);
    uint32_t (*getLastError)();
    int32_t (*getStringTypeA)(uint32_t lcid, uint32_t type, const char* src, int32_t n, uint16_t* out);
    int32_t (*multiByteToWideChar)(uint32_t cp, uint32_t flags, const char* src, int32_t n, uint16_t* out, int32_t outN);
    int32_t (*wideCharToMultiByte)(uint32_t cp, uint32_t flags, const uint16_t* src, int32_t n, char* out, int32_t outN,
                                   const char* defChar, int32_t* usedDef);
    int32_t (*getCPInfoMaxChar)(uint32_t cp);              // GetCPInfo: -1 when it fails, else CPINFO.MaxCharSize
    uint32_t (*ansiCodePage)(uint32_t lcid);               // 0x5919 (cdecl), 0xFFFFFFFF on failure
    char* (*convert)(const D2DDrawCpHost* h, uint32_t from, uint32_t to, const char* src, uint32_t* pcb, char* out, int32_t outN);  // 0x5960
    void* (*calloc)(uint32_t count, uint32_t size);        // 0x1F05 _calloc_crt
    void (*free)(void* p);                                 // 0x1E94 _free
    uint32_t* useWide;                                     // DAT 0xF27C: 0 unprobed, 1 wide API, 2 ANSI API
    const uint32_t* defLcid;                               // DAT 0xF264
    const uint32_t* defCodePage;                           // DAT 0xF274
    const uint16_t* emptyW;                                // retail's L"" (.rdata 0xBFDC)
};

extern "C" {
int32_t d2ddraw_crt_get_string_type_a_4f88(const D2DDrawCpHost* h, uint32_t type, const char* src, int32_t cch, uint16_t* out,
                                           uint32_t codePage, uint32_t lcid, int32_t bError);
char* d2ddraw_convert_cp_5960(const D2DDrawCpHost* h, uint32_t from, uint32_t to, const char* src, uint32_t* pcb, char* out,
                              int32_t outN);
}

// src/core/d2launch/d2launch_n22_host.h -- the host seams of the 1.13c D2Launch CRT locale rows in d2launch_batch_n22.cpp
// (0x8C78 ___crtGetStringTypeA, 0x8EC0 ___crtLCMapStringA, 0x942D the code-page converter); shared with their function probes.
#pragma once
#include <cstdint>
// Host seams
struct N22Host {
    int (*GetStringTypeW)(uint32_t, const uint16_t*, int, uint16_t*);
    int (*GetStringTypeA)(uint32_t lcid, uint32_t type, const char*, int, uint16_t*);
    int (*MultiByteToWideChar)(uint32_t cp, uint32_t fl, const char*, int, uint16_t*, int);
    int (*WideCharToMultiByte)(uint32_t cp, uint32_t fl, const uint16_t*, int, char*, int, const char*, int*);
    int (*LCMapStringW)(uint32_t, uint32_t, const uint16_t*, int, uint16_t*, int);
    int (*LCMapStringA)(uint32_t, uint32_t, const char*, int, char*, int);
    int (*GetCPInfoMaxChar)(uint32_t cp);                 // -1 on failure, else MaxCharSize
    uint32_t (*GetLastError)();
    uint32_t (*GetTickCount)();
    uint32_t (*cp_for_lcid)(uint32_t lcid);               // 0x93E6, 0xFFFFFFFF on failure
    int32_t  gst_mode;                                    // DAT_6fa65194
    int32_t  lcmap_mode;                                  // DAT_6fa651bc
    uint32_t def_lcid;                                    // DAT_6fa6517c
    uint32_t def_cp;                                      // DAT_6fa6518c
    const uint16_t* emptyW;                               // retail's L"" (0x6fa5d568); null = a local one
    char* (*convertCP)(N22Host*, uint32_t, uint32_t, const char*, uint32_t*, char*, int);   // 0x942D; null = n22_crtConvertCP
    void (*free_)(void*);                                 // _free 0x43B8; null = free
    void* (*calloc_)(uint32_t n, uint32_t sz);            // _calloc 0x49EA; null = calloc
};
char* n22_crtConvertCP(N22Host* h, uint32_t cpFrom, uint32_t cpTo, const char* src, uint32_t* pcb, char* out, int outCb);
int n22_crtGetStringTypeA(N22Host* h, uint32_t type, const char* src, int cch, uint16_t* outType, uint32_t cp, uint32_t lcid, int bError);

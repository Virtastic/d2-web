// src/core/d2cmp/d2cmp_palstate.h -- D2CMP palette state (0x6FE872E8..0x6FE88324) and the palette-
// shift table builders 0x9500..0xB930. The globals are defined once: the 0x9A10 set in
// d2cmp_batch_116d0.cpp, the rest in the file of the routine that owns them. Win32 / Storm / Fog
// callees are D2CmpPalEnv seams; the shift block (VirtualAlloc'd, 0x6C327 bytes) is a host pointer.
#pragma once
#include <cstdint>

extern int32_t  g_d2cmp_dist_6fe879f0[511];   // squares, index 255 = 0x6FE879F0 (difference 0)
extern int32_t  g_d2cmp_dist_6fe875f0;        // difference -256, the word 0x9C90 writes below the table
extern uint8_t  g_d2cmp_excl_6fe87df0[256];
extern uint8_t  g_d2cmp_pal_6fe87f18[256 * 4];
extern uint32_t g_d2cmp_palStart_6fe87ef0;
extern uint32_t g_d2cmp_palEnd_6fe875ec;
extern uint32_t g_d2cmp_skipSame_6fe875e8;
extern uint32_t g_d2cmp_lastColor_6fe8831c;
extern uint32_t g_d2cmp_useExcl_6fe88324;
extern uint8_t  g_d2cmp_raw_6fe872e8[0x300];  // a 768-byte .pal file read verbatim by 0xA2E0
extern uint32_t g_d2cmp_loaded_6fe88318;      // nonzero = 0x6FE87F18 holds a usable palette
extern uint32_t g_d2cmp_dark_6fe88320;        // nonzero = 0xA250 maps near-black triples to (4,0,0)

struct D2CmpPalEnv {
    void (*fatal)(uint32_t code);                       // Fog #10265 + #10024 + _exit(-1); does not return
    uint32_t (*strLen)(const char* s);                  // Storm #506
    intptr_t (*open)(const char* name);                 // CreateFileA(GENERIC_READ, share read, OPEN_EXISTING); -1 on failure
    uint32_t (*size)(intptr_t h);                       // GetFileSize
    uint32_t (*read)(intptr_t h, void* dst, uint32_t n);// ReadFile, returns bytes read
    void (*seekEnd)(intptr_t h, int32_t off);           // SetFilePointer(h, off, NULL, FILE_END)
    void (*close)(intptr_t h);                          // CloseHandle
    void (*debug)(const char* s);                       // OutputDebugStringA
    uint8_t* (*valloc)(uint32_t size);                  // VirtualAlloc(NULL, size, MEM_COMMIT|MEM_RESERVE, RWX)
    void (*vfree)(uint8_t* p);                          // VirtualFree(p, 0, MEM_RELEASE)
};

uint32_t d2cmp_sub_9d30(const uint8_t* pal, uint32_t count, int32_t p3, int32_t p4, int32_t p5);
extern "C" {
uint32_t d2cmp_9a10(uint8_t r, uint8_t g, uint8_t b);
void     d2cmp_9500(uint8_t* dst, const uint8_t* src, int32_t rows, uint32_t width, int32_t dstPitch, int32_t srcPitch);
void     d2cmp_99d0(const uint8_t* table);
uint32_t d2cmp_9b00(const D2CmpPalEnv* env, const char* name);
void     d2cmp_9c60(uint32_t color);
void     d2cmp_9c90(void);
uint32_t d2cmp_9cc0(const uint8_t* pal, uint32_t count, int32_t p3, int32_t p4, int32_t p5);
void     d2cmp_9da0(const D2CmpPalEnv* env, uint8_t* block);
void     d2cmp_9dc0(uint8_t* block);
void     d2cmp_9e60(uint8_t* block);
void     d2cmp_9ed0(uint8_t* block);
void     d2cmp_9fa0(uint8_t* block);
void     d2cmp_a070(uint8_t* block);
double   d2cmp_a1b0(double m1, double m2, double hue);
uint32_t d2cmp_a250(const uint8_t* src, uint8_t* dst, int32_t n, uint32_t (*cb)(uint8_t, uint8_t, uint8_t));
uint32_t d2cmp_a2e0(const D2CmpPalEnv* env, const char* name, int32_t lo, int32_t hi);
void     d2cmp_a490(uint8_t* block);
void     d2cmp_a640(uint8_t* block);
void     d2cmp_a710(const double* hsl, uint8_t* rgb);
void     d2cmp_a830(const D2CmpPalEnv* env, double* hsl, const uint8_t* pal);
void     d2cmp_aad0(double* hsl, uint8_t* block);
void     d2cmp_ab60(uint8_t* block, const double* hsl);
void     d2cmp_b920(uint8_t* block);
uint8_t* d2cmp_b930(const D2CmpPalEnv* env, const uint8_t* pal);
}

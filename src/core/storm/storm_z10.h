// Seam table of the Storm SBMP/SFile helpers (storm_batch_z10.cpp); FN probes fill it with recorders.
#pragma once
#include <cstdint>
struct StormZ10Env {
    uint32_t* lastError;                                            // DAT_6fc45388
    void (*setLastError)(uint32_t);                                 // SetLastError
    void (*enter)(void*); void (*leave)(void*);                     // Enter/LeaveCriticalSection
    void* csList;                                                   // DAT_6fc45940
    void* csFile;                                                   // DAT_6fc45988
    uint32_t* (*list2690)(void* ecx, uint32_t table);               // Storm+0x2690
    uint32_t table458f0;                                            // 0x6fc458f0
    void (*sub317F0)(uint32_t* e);                                  // Storm+0x317F0
    void (*sub308A0)(uint32_t* e, const float* rect);               // Storm+0x308A0
    void (*sub316D0)(int a, const float* r, uint32_t b, int c);     // Storm+0x316D0
    void (*sub319C0)(int a, const float* r, uint32_t* n, uint32_t* o); // Storm+0x319C0
    void (*sub318F0)(int a, uint32_t* n, uint32_t* o);              // Storm+0x318F0
    int32_t (*ftol)(void* st0);                                     // Storm+0x3C908 (x87 st0 -> int)
    // file side
    int (*read195B0)(uint8_t* f, void* buf, uint32_t n, int* got, int, int, int); // Storm+0x195B0
    int (*read19BE0)(uint8_t* f, void* buf, uint32_t n, int* got, int);           // Storm+0x19BE0
    uint32_t (*size162D0)(uint8_t* f, uint32_t* hi);                              // Storm+0x162D0
    uint32_t (*getFileSize)(uint32_t h);                            // GetFileSize
    uint32_t (*setFilePointer)(uint32_t h, int32_t d, int method);  // SetFilePointer
    void (*close16E20)(uint8_t* f);                                 // Storm+0x16E20
    int (*open18960)(void*, const void* name, uint8_t flags, uint8_t** out); // Storm+0x18960
    void* (*alloc)(uint32_t n, const char* file, int line, int fl); // Storm+0xC580
    void (*free)(void* p, const char* file, int line);              // Storm+0xB960
    uint32_t* openFlags43130; int32_t* cnt45984;                    // DAT_6fc43130 / DAT_6fc45984
    int (*bmp220B0)(uint32_t*, uint32_t*, int, uint32_t*, uint32_t);                     // Storm+0x220B0
    int (*bmp21E80)(const void*, int*, uint32_t, int, uint8_t*, uint32_t, int*);         // Storm+0x21E80
    int (*bmp228C0)(uint32_t, const void*, uint8_t*, uint32_t, uint32_t*, uint32_t*, uint32_t*); // Storm+0x228C0
    int (*bmp21FF0)(uint32_t, void*, void*, uint32_t, uint32_t, uint32_t);              // Storm+0x21FF0
    void (*bmp21C40)(int, uint32_t, uint8_t*, const uint8_t*, uint32_t, uint32_t, uint32_t); // Storm+0x21C40
    int (*tga22690)(void); int (*tga225C0)(const void*, uint8_t*, uint32_t, void*);     // Storm+0x22690 / 0x225C0
    int (*tga222F0)(uint8_t*, const void*, uint32_t, uint8_t*);                         // Storm+0x222F0
    void* wndProc34300;                                                                  // Storm+0x34300, the address 0x34370 answers
};
int Storm_Sub33730(const StormZ10Env& E, uint8_t* f, uint32_t* outW, int pal, uint8_t* pix, int, int* outH, int* outBpp);
int Storm_Sub33A10(const StormZ10Env& E, uint8_t* f, uint32_t* outW, uint32_t* outBpp, int pal, uint32_t* pix, uint32_t arg, uint32_t* outH);
int Storm_Sub33C30(const StormZ10Env& E, uint8_t* f, void* palOut, uint8_t* pixOut, uint32_t* outW, uint32_t* outH, uint32_t* outBpp);
int Storm_Sub333F0(const StormZ10Env& E, const char* name, const int* pal, uint8_t** outBuf, uint32_t* w, uint32_t* h, uint32_t* bpp,
                   uint32_t wantBpp, uint8_t* (*userAlloc)(uint32_t));
int Storm_Sub33EA0(const StormZ10Env& E, const char* name, int pal, uint8_t* pix, int pixSz, uint32_t* w, uint32_t* h, uint32_t* bpp,
                   int (*pcx)(uint8_t*, int, uint8_t*, int, int*, int*), int (*bmp)(uint32_t*, int, uint8_t*, int, uint32_t*), int (*tga)(int, uint8_t*, int, uint32_t*, uint32_t*, uint32_t*));

// src/core/d2gfx/d2gfx_dosmaperr.cpp -- 1.13c D2Gfx CRT __dosmaperr, reimplemented from the cached
// decompile (ghidra-d2gfx/decomp/0x386A.c); the 45-entry table is read from D2Gfx.dll 1.13c RVA 0x102C0.
// Alpha/Stage-A lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. The two ptd
// cells it writes are 0x3858 __errno and 0x3861 _doserrno, already ported in d2gfx_crt_errno.cpp.
#include <cstdint>

extern "C" int* d2gfx_errno(void);          // 0x3858
extern "C" uint32_t* d2gfx_doserrno(void);  // 0x3861

// DAT_6fa902c0 (RVA 0x102C0): 45 {dos error, errno} pairs, first match wins. Entry 30 repeats dos error
// 6 with errno 22 (EINVAL) after the 6 -> 9 (EBADF) entry 5; entry 44 is the image's own tail value.
static const uint32_t g_err_table[45][2] = {
    {1, 22},    {2, 2},     {3, 2},     {4, 24},    {5, 13},    {6, 9},     {7, 12},    {8, 12},
    {9, 12},    {10, 7},    {11, 8},    {12, 22},   {13, 22},   {15, 2},    {16, 13},   {17, 18},
    {18, 2},    {33, 13},   {53, 2},    {65, 13},   {67, 2},    {80, 17},   {82, 13},   {83, 13},
    {87, 22},   {89, 11},   {108, 13},  {109, 32},  {112, 28},  {114, 9},   {6, 22},    {128, 10},
    {129, 10},  {130, 9},   {131, 22},  {132, 13},  {145, 41},  {158, 13},  {161, 2},   {164, 11},
    {167, 13},  {183, 17},  {206, 2},   {215, 11},  {1816, 12},
};

// 1.13c D2Gfx+0x0386A __dosmaperr (115 bytes): void __cdecl __dosmaperr(unsigned long doserrno).
// _tdoserrno is written first, unconditionally. The 45-entry scan then sets _terrno from the first pair
// whose dos error matches and returns. With no match: 0x13..0x24 -> EACCES (13), 0xBC..0xCA -> ENOEXEC
// (8), anything else EINVAL (22). No return value and no other side effect.
// 1.13c D2Gfx+0x0386A
extern "C"
void d2gfx_dosmaperr(uint32_t doserrno) {
    *d2gfx_doserrno() = doserrno;
    for (uint32_t i = 0; i < 0x2d; ++i) {
        if (doserrno == g_err_table[i][0]) {
            *d2gfx_errno() = (int32_t)g_err_table[i][1];
            return;
        }
    }
    if (doserrno > 0x12 && doserrno < 0x25) {
        *d2gfx_errno() = 0xd;                   // EACCES
    } else if (doserrno > 0xbb && doserrno < 0xcb) {
        *d2gfx_errno() = 8;                     // ENOEXEC
    } else {
        *d2gfx_errno() = 0x16;                  // EINVAL
    }
}

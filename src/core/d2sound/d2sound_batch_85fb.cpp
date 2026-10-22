// src/core/d2sound/d2sound_batch_85fb.cpp -- 1.13c D2Sound.dll small helpers (0x85FB, 0x9F12, 0xC275, 0xC2A5, 0xC2FE).
// Not ported (no faithful C form): 0x15C5 (__lock, DAT_ lock table), 0x1770/0x17AB (SEH prolog/epilog frame hacks),
// 0xC200/0xC260 (x87 ST0 implicit, float).
#include <cstdint>
#ifdef _WIN32
#include <windows.h>

// 1.13c D2sound+0x85FB
// Error cleanup of the file-map loader: unmap view, close mapping and file, preserve LastError.
// Retail reads view/mapping from the caller frame (EBP-0x20/-0x24) and the file in EDI; explicit here.
void D2sound_85FB(const void* view, HANDLE mapping, HANDLE file) {
    DWORD err = GetLastError();
    if (view) UnmapViewOfFile(view);
    if (mapping) CloseHandle(mapping);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    SetLastError(err);
}
#endif

// 1.13c D2sound+0x9F12
// x87 round-compare with both branches returning: a no-op.
void D2sound_9F12(void) {}

// 1.13c D2sound+0xC275
void D2sound_C275(void) {}

// 1.13c D2sound+0xC2FE
void D2sound_C2FE(void) {}

// 1.13c D2sound+0xC2A5
// CRT __fload_withFB: `dbl` points at a double; returns the exponent field of its high word,
// or the whole high word when the exponent is all ones (Inf/NaN).
uint32_t D2sound_C2A5(const uint8_t* dbl) {
    uint32_t hi = (uint32_t)dbl[4] | (uint32_t)dbl[5] << 8 | (uint32_t)dbl[6] << 16 | (uint32_t)dbl[7] << 24;
    uint32_t e = hi & 0x7ff00000;
    return e != 0x7ff00000 ? e : hi;
}

#pragma once
#include <cstdarg>
#include <cstdint>

// Logical string-stream view: native pointers are deliberately not a retail FILE overlay.
// Only the fields consumed by the wrapper and its formatting callees are exposed.
struct D2CommonStringStream2814 {
    char* cursor;
    int32_t remaining;
    char* base;
    int32_t flags;
};
struct D2CommonVsprintf2814Calls {
    int (*output)(D2CommonStringStream2814*, const char*, va_list); // +0x3985
    int (*flushByte)(int, D2CommonStringStream2814*); // +0x37DE
};
extern "C" int D2Common_2814_Vsprintf(char*, const char*, va_list,
                                     const D2CommonVsprintf2814Calls*);

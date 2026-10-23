// src/core/d2win/d2win_sub_4ed0.cpp -- 1.13c D2Win+0x4ED0, MSVC CRT strcpy (word-at-a-time), from the cached decompile.
#include <cstdint>

// 1.13c D2Win+0x04ED0
extern "C" char* d2win_sub_4ed0(char* dst, const char* src) {
    char* d = dst;
    while ((*d++ = *src++) != 0) {}   // byte copy incl. NUL; retail's aligned dword path writes identical bytes
    return dst;
}

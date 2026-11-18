#include <cstddef>

// The MSVC 2003 Release CRT `_strncpy`, reached from D2Gfx's own code (callers 0x360C `sub_360C` and
// 0x608C `__report_gsfailure`). Copies at most `count` bytes, stopping after a NUL; when the source
// terminates first the rest of the count is NUL-padded, and when it does not no terminator is stored.
// `count == 0` leaves the destination untouched. The retail body is the aligned dword copy (byte copy
// to a 4-byte source boundary, then a word store selected by the zero-byte test, then word-wise zero
// fill); a byte loop produces the same bytes and the same return value.
extern "C"
// 1.13c D2Gfx+0x05AF0
char* d2gfx_strncpy(char* dest, const char* src, size_t count) {
    char* const ret = dest;
    while (count != 0 && *src != '\0') {
        *dest++ = *src++;
        --count;
    }
    while (count-- != 0) *dest++ = '\0';
    return ret;
}

// 1.13c D2Win+0x18B69 (CRT _cropzeros/_forcdecpt): in a formatted float string, find the first non-digit
// after the leading char (or an 'e'/'E' at the start) and insert the locale decimal point there, shifting the tail right.
#include <cstdint>
#include <cctype>
#include <emscripten.h>

extern "C" char d2win_g_decpt;   // 0x6F8FF984

extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_18b69(char* p) {
    if (tolower((unsigned char)*p) != 'e')
        do ++p; while (isdigit((unsigned char)*p));
    char c = *p;
    *p = d2win_g_decpt;
    do { ++p; char n = *p; *p = c; c = n; } while (*p);
}

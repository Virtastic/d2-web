// 1.13c D2CMP+0x18A1B (60 bytes): MSVC CRT float-format helper (_shift/_fassign family). Starting at p,
// advance past digits (at least one step unless *p is 'e'/'E'), then insert the locale decimal point
// there, shifting the rest of the string (incl. NUL) one byte right. dp = retail DAT_6FE32AB4.
#include <cctype>
extern "C"
void d2cmp_crt_18a1b(char* p, char dp) {   // 1.13c D2CMP+0x18A1B
    if (tolower((unsigned char)*p) != 'e') {
        do { ++p; } while (isdigit((unsigned char)*p));
    }
    char c = *p;
    *p = dp;
    do { ++p; char n = *p; *p = c; c = n; } while (*p != '\0');
}

// 1.13c BNClient+0x130B0 -- name hash over the DAT_6ff3f62c table (table index = param*0x100 + signed char of the upper-cased byte). Table passed in.
#include <cstdint>
extern "C" uint32_t bnclient_130b0_hash(const char* name, uint32_t param, const uint32_t* table) {
    uint32_t u2 = 0x7fed7fed;
    int32_t i3 = -0x11111112;
    for (const char* p = name; p && *p; ++p) {
        int32_t ch = (int32_t)(signed char)*p;
        int32_t c = (ch >= (int32_t)(signed char)0x61 && ch <= 0x7a) ? ch - 0x20 : ch;   // _toupper on ASCII
        int32_t index = (int32_t)(param * 0x100) + (int32_t)(signed char)c;
        u2 = (u2 + (uint32_t)i3) ^ table[index];
        i3 = i3 * 0x21 + (int32_t)(signed char)c + 3 + (int32_t)u2;
    }
    return u2;
}

// D2Client+0x8E180 -- the item flag line of the inventory tooltip: "Ethereal", ", " and "Socketed (n)".
#include <cstdint>
#include "d2client_8e180.h"
static void wcat(uint16_t* d, const uint16_t* s) { while (*d) ++d; while ((*d++ = *s++)) {} }
static const uint32_t kFile = 0xD476C;   // "..\Source\D2Client\UI\inv.cpp" in the module image

// 1.13c D2Client+0x8E180
void d2c_8e180(const D2Client8e180Env& e, uint16_t* out, uint32_t item) {
    const uint16_t* close = e.str(0xf9e);
    out[0] = 0;                                   // strcpy(out, L"")
    bool eth = e.itemFlag(item, 0x400000, 0x6c1, kFile) != 0;
    if (eth) wcat(out, e.str(0x58d9));
    if (e.itemFlag(item, 0x800, 0x6c9, kFile)) {
        if (eth) { static const uint16_t sep[3] = { ',', ' ', 0 }; wcat(out, sep); }
        wcat(out, e.str(0xd7d));
        const uint16_t* open = e.str(0xf9b);
        uint32_t n = e.socketCount(item) & 0xFF;
        uint16_t w[10]; char t[4]; int k = 0;     // "(%i)" of a byte, 9 chars at most
        if (n >= 100) t[k++] = (char)('0' + n / 100);
        if (n >= 10) t[k++] = (char)('0' + n / 10 % 10);
        t[k++] = (char)('0' + n % 10);
        int i = 0; w[i++] = '('; for (int j = 0; j < k; ++j) w[i++] = (uint16_t)t[j]; w[i++] = ')'; w[i] = 0;
        wcat(out, open); wcat(out, w); wcat(out, close);
    } else if (eth) wcat(out, close);
}

// D2Client FUN_6fb55fb0: real function; the Env form in sub_a5fb0.cpp stays for old callers.
#include <cstdint>
#include <unistd.h>
#define W __attribute__((weak))
extern "C" {
W uint32_t Ordinal_10265(uint32_t id);
W void Ordinal_10024(const void* fmt, uint32_t s);
W uint8_t DAT_6fb8359f[1];
W uint32_t* Ordinal_10392(uint32_t unitId);
W void Ordinal_10739(uint32_t unitId, uint32_t x, uint32_t y, uint32_t extra);
W void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);   // D2Client+0x2A0B0
W uint32_t FUN_6fab1630(const uint32_t* unit);                    // D2Client+0x1630 unit X
W uint32_t FUN_6fab1660(const uint32_t* unit);                    // D2Client+0x1660 unit Y

// 1.13c D2Client+0xA5FB0
// in_EAX = object (u32 at +0x2c = unit id); unaff_EDI = hidden arg of 2A0B0; unaff_retaddr = stack word passed on to Ordinal_10739.
void FUN_6fb55fb0(uint32_t* in_EAX, uint32_t unaff_EDI, uint32_t unaff_retaddr) {
    FUN_6fada0b0((const uint8_t*)in_EAX, unaff_EDI);
    if (in_EAX[0xb] == 0) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0x946)); _exit(-1); }
    uint32_t* p = Ordinal_10392(in_EAX[0xb]);
    if (p && p != in_EAX) {
        uint32_t id = in_EAX[0xb];
        if (id == 0) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0x7e2)); _exit(-1); }
        uint32_t y = FUN_6fab1660(p);
        uint32_t x = FUN_6fab1630(p);
        Ordinal_10739(id, x, y, unaff_retaddr);
    }
}
}

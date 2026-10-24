// D2Client FUN_6fb55f70: real function; the D2cA5F70Env form in sub_a5f70.cpp stays for old callers.
#include <cstdint>
#include <unistd.h>
#define W __attribute__((weak))
extern "C" {
W uint32_t Ordinal_10265(uint32_t id);
W void Ordinal_10024(const void* fmt, uint32_t s);
W uint8_t DAT_6fb8359f[1];
W void* Ordinal_10392(uint32_t unitId);
W void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);   // D2Client+0x2A0B0

// 1.13c D2Client+0xA5F70
// unaff_ESI = object (u32 at +0x2c = unit id); unaff_EDI = hidden register argument of 2A0B0 (its EAX is ESI here, as in sub_A1340).
void FUN_6fb55f70(const uint8_t* unaff_ESI, uint32_t unaff_EDI) {
    FUN_6fada0b0(unaff_ESI, unaff_EDI);
    uint32_t id = *(const uint32_t*)(unaff_ESI + 0x2c);
    if (!id) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0x946)); _exit(-1); }
    Ordinal_10392(id);
}
}

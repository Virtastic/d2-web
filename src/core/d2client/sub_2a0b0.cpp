// D2Client sub_2A0B0 (FUN_6fada0b0): real function; callees are the imported ordinals / real D2Client functions.
#include <cstdint>
#include <unistd.h>
#define W __attribute__((weak))
extern "C" {
W uint32_t Ordinal_10265(uint32_t id);
W void Ordinal_10024(const void* fmt, uint32_t s);
W uint8_t DAT_6fb8359f[1];
W void* Ordinal_10392(uint32_t unitId);
W void Ordinal_11163(uint32_t unitId, uint32_t* out);
W void Ordinal_10336(uint32_t unitId, uint32_t v);
W void* FUN_6fb54e20(uint32_t edi, void* tbl, uint32_t type);   // D2Client+0xA4E20
W uint8_t DAT_6fbba608[1];

// 1.13c D2Client+0x2A0B0
// in_EAX = unit (u32 at +0x2c = unit id), unaff_EDI = hidden register argument. Table stride = type * 0x80 (byte table).
// GUESS: Ordinal_11163 writes the type into out[0] (the decompile shows it as the stack local passed on).
void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI) {
    if (!in_EAX) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0)); _exit(-1); }
    uint32_t id = *(const uint32_t*)(in_EAX + 0x2c);
    if (!id) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0x2a)); _exit(-1); }
    if (Ordinal_10392(id)) {
        uint32_t out[2] = {0, 0};
        Ordinal_11163(id, out);
        void* p = FUN_6fb54e20(unaff_EDI, DAT_6fbba608 + out[0] * 0x80, out[0]);
        if (p != Ordinal_10392(id)) Ordinal_10336(id, 0);
    }
}
}

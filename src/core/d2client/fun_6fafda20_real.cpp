// D2Client FUN_6fafda20: real function (__fastcall, param_1 = ECX); the Svc form stays for old callers.
#include <cstdint>
#include <unistd.h>
#define W __attribute__((weak))
extern "C" {
W uint32_t Ordinal_10265(uint32_t id);
W void Ordinal_10024(const void* fmt, uint32_t s);
W uint8_t DAT_6fb8359f[1];
W int32_t* FUN_6fb560d0(uint32_t param_1);              // D2Client+0xA60D0
W void FUN_6fb61420(int32_t* self, uint32_t zero);      // D2Client+0xB1420

// 1.13c D2Client+0x4DA20
void FUN_6fafda20(uint32_t param_1) {
    int32_t* self = FUN_6fb560d0(param_1);
    if (!self) return;
    if (*self != 3) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0x161)); _exit(-1); }
    FUN_6fb61420(self, 0);
}
}

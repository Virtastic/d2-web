// 1.13c D2Client+0x38E80 -- open the key-bindings file. ESI and EBX are the two Ordinal_10107 inputs (passed through untouched).
// Gets the base directory (10115), then when the profile string at 0x119860 is non-empty: builds "%s%s\\" from it, asks 10114, builds "%s%s.key"
// (0x119928 = file stem) and opens it with 10107; a result other than -1 ends. Otherwise (empty profile or -1) it builds the plain "%s%s.key" and opens that.
#include <cstdint>
struct D2c38E80Env {
    void (*getBase)();                               // Ordinal_10115 (ECX = 0x104-byte buffer, EDX = 0x104)
    void (*sprintf)(uint32_t fmtRva);                // Ordinal_578 (0xBEA2): fmt 0xD5374 "%s%s\\" or 0xD6858 "%s%s.key"
    void (*getDir)();                                // Ordinal_10114 (0xC022, ECX = buffer)
    int32_t (*open)(uint32_t ebx, uint32_t esi);     // Ordinal_10107 (0xC02E): ECX buffer, EDX = ebx, stack (0, 0, esi, 0x80, 0)
};
extern "C" void d2c_38e80(uint32_t esi, uint32_t ebx, uint8_t profile0, const D2c38E80Env* env) {
    env->getBase();
    if (profile0) {
        env->sprintf(0xD5374);
        env->getDir();
        env->sprintf(0xD6858);
        if (env->open(ebx, esi) != -1) return;
    }
    env->sprintf(0xD6858);
    env->open(ebx, esi);
}

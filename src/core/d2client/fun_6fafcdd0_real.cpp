// D2Client FUN_6fafcdd0: real function; the Svc form in d2client_batch_4cad0.cpp stays for old callers.
#include <cstdint>
#define W __attribute__((weak))
extern "C" {
W uint32_t DAT_6fb8601c[5];
W uint32_t Ordinal_10750(uint32_t p) __asm__("Ordinal_10750_6fafcdd0");
W uint32_t Ordinal_10867(uint32_t p) __asm__("Ordinal_10867_6fafcdd0");
W void Ordinal_10210(int32_t u) __asm__("Ordinal_10210_d2client");
W void FUN_6fb77b00();                                                                                                  // D2Client+0xC7B00
W int32_t FUN_6fb5ff10(uint32_t ecx, int32_t* unit, uint32_t a, uint32_t x, uint32_t y, uint32_t zero, uint32_t one);  // D2Client+0xAFF10

// 1.13c D2Client+0x4CDD0
// unaff_ESI = unit dword array. ECX into FUN_6fb5ff10 is a leftover register (5, or the ordinals' ECX): not recoverable, 5 passed.
// The decompile's k>4 fatal(0x174) branch is dead (k = r % 5).
void FUN_6fafcdd0(int32_t* unaff_ESI) {
    uint64_t r = (uint64_t)(uint32_t)unaff_ESI[8] * 0x6ac690c5ull + (uint32_t)unaff_ESI[9];
    unaff_ESI[8] = (int32_t)(uint32_t)r; unaff_ESI[9] = (int32_t)(uint32_t)(r >> 32);
    uint32_t a = DAT_6fb8601c[(uint32_t)r % 5];
    int32_t t = *unaff_ESI;
    bool direct = t == 2 || (t > 3 && t < 6);
    uint32_t p = (uint32_t)unaff_ESI[0xb];
    uint32_t y = direct ? *(const uint32_t*)(uintptr_t)(p + 0x10) : p ? Ordinal_10750(p) : 0;
    uint32_t x = direct ? *(const uint32_t*)(uintptr_t)(p + 0xc) : p ? Ordinal_10867(p) : 0;
    int32_t n = FUN_6fb5ff10(5, unaff_ESI, a, x, y, 0, 1);
    if (n) {
        Ordinal_10210(n); Ordinal_10210(n); FUN_6fb77b00();
        uint32_t q = *(uint32_t*)(uintptr_t)(n + 0x54);
        if (q) { uint32_t* f = *(uint32_t**)(uintptr_t)(q + 0x30); if (f) *f |= 0x10; }
        *(uint32_t*)(uintptr_t)(n + 200) |= 0x40000;
    }
}
}

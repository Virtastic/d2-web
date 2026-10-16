// Pure D2Client leaves. Structs are addressed by 1.13c byte offsets (32-bit pointer fields), so they work on wasm64 blobs.
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x13D0: eax=obj, ecx=mask, edx=on. obj->dwFlags(+0xC4) |= mask or &= ~mask; null obj is a no-op.
void D2Client_SetFlagC4(uint8_t* obj, uint32_t mask, uint32_t on) {
    if (!obj) return;
    uint32_t f; std::memcpy(&f, obj + 0xC4, 4);
    f = on ? (f | mask) : (f & ~mask);
    std::memcpy(obj + 0xC4, &f, 4);
}

// 1.13c D2Client+0x1E40: eax=unit, [esp+4]=mask. Player unit (dwType==1) with pData(+0x14): pData->word(+0x28) & mask, else 0.
uint32_t D2Client_PlayerWord28(const uint8_t* unit, uint16_t mask) {
    if (!unit) return 0;
    uint32_t t, p; std::memcpy(&t, unit, 4); std::memcpy(&p, unit + 0x14, 4);
    if (t != 1 || !p) return 0;
    uint16_t w; std::memcpy(&w, (const uint8_t*)(uintptr_t)p + 0x28, 2);
    return w & mask;
}

// 1.13c D2Client+0x2A40: eax=n, [esp+4]=out. n<0x15 -> highest set bit of n (0 -> 1); else out[+0xC]=0x15, return 0x15.
uint32_t D2Client_ClampPow2(uint32_t n, uint8_t* out) {
    if (n >= 0x15) { uint32_t v = 0x15; std::memcpy(out + 0xC, &v, 4); return 0x15; }
    while (n & (n - 1)) n &= n - 1;
    return n ? n : 1;
}

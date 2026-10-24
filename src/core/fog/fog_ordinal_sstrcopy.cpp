// 1.13c Fog/Storm Ordinal_501 SStrCopy(dst, src, cap): bounded copy, always terminated (cap 0 writes nothing).
#include <cstdint>
extern "C" void Fog_Ordinal_501_SStrCopy(char* dst, const char* src, uint32_t cap) {
    if (!cap) return;
    uint32_t i = 0;
    for (; i + 1 < cap && src[i]; ++i) dst[i] = src[i];
    dst[i] = 0;
}

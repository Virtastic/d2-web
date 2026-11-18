// src/core/fog/fog_batch_cf00.cpp -- three 1.13c Fog.dll alpha-lane rows, from
// build-wasm/ghidra-fog/decomp/0x<RVA>.c. Offsets address a raw byte block (layout kept 1:1).
#include <cstdint>
#include <cstring>

namespace {
template <class T> T& at(void* base, int off) { return *reinterpret_cast<T*>(static_cast<char*>(base) + off); }
}

// 1.13c Fog+0xCF00 -- append (key=param_3, value=param_2) to the 8-byte-entry table at +0x48 (count at +0x848, max 0x100); duplicate keys ignored.
void FogSub_CF00(void* self, int32_t value, int32_t key) {
    int32_t n = at<int32_t>(self, 0x848);
    if (n >= 0x100) return;
    for (int32_t i = 0; i < n; ++i)
        if (at<int32_t>(self, 0x48 + i * 8) == key) return;
    at<int32_t>(self, 0x48 + n * 8) = key;
    at<int32_t>(self, 0x4c + at<int32_t>(self, 0x848) * 8) = value;
    at<int32_t>(self, 0x848) += 1;
}

// 1.13c Fog+0xD040 -- swap slot [index] at +0xBA8; stores newValue only when non-zero, returns the old value.
int32_t FogSub_D040(void* self, int32_t index, int32_t newValue) {
    int32_t old = at<int32_t>(self, 0xba8 + index * 4);
    if (newValue != 0) at<int32_t>(self, 0xba8 + index * 4) = newValue;
    return old;
}

// 1.13c Fog+0xD070 -- latch four config words (and count-1) into module globals.
int32_t g_fogCfg0, g_fogCfg0Minus1, g_fogCfg1, g_fogCfg2, g_fogCfg3;
void FogSub_D070(const int32_t* p) {
    g_fogCfg0 = p[0];
    g_fogCfg0Minus1 = p[0] - 1;
    g_fogCfg1 = p[1];
    g_fogCfg2 = p[2];
    g_fogCfg3 = p[3];
}

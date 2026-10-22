// src/core/fog/fog_batch_b54a.cpp -- 1.13c Fog.dll rows from the cached Ghidra decompiles.
// Win32 / CPUID / sibling-Fog calls go through the seam table below (no direct Win32).
#include <cstdint>

struct FogB54aSeams {
    uint32_t (*waitForSingleObject)(uintptr_t handle, uint32_t ms);  // kernel32 WaitForSingleObject
    uint32_t (*fogWait)(void* a, uint32_t ms);                       // Fog+0x20AF0
    void (*validateCs)(void* cs);                                    // Fog+0x11690
    void (*enterCs)(void* cs);
    void (*leaveCs)(void* cs);
    void (*sub1D620)(uint32_t* a, uint32_t* b, uint32_t c, uint32_t d);  // Fog+0x1D620
    int32_t cpuVendor;                                               // Fog+0x1C5B0 result (1 Intel,2 AMD,3 Cyrix)
    uint32_t cpuSignature;                                           // Fog+0x1C020 low dword (CPUID.1 EAX)
    int32_t cpuLevel;                                                // DAT_6ff82f30
};
FogB54aSeams g_fogB54a;

static inline uint8_t* at(void* p, uint32_t o) { return static_cast<uint8_t*>(p) + o; }
static inline uint32_t rd(void* p, uint32_t o) { return *reinterpret_cast<uint32_t*>(at(p, o)); }

// 1.13c Fog+0xF6A0
extern "C" uint32_t fog_sub_F6A0(int32_t* p, uint32_t ms) {  // 1.13c Fog+0xF6A0
    if (p[0] != 0) return g_fogB54a.fogWait(p, ms);
    return g_fogB54a.waitForSingleObject(static_cast<uintptr_t>(static_cast<uint32_t>(p[0x2f5])), ms) == 0;
}

// 1.13c Fog+0xD2F0 -- count every node across all bucket chains (next at +0x5bc).
extern "C" int32_t fog_sub_D2F0(void* p) {  // 1.13c Fog+0xD2F0
    if (!p) return 0;
    void* cs = at(p, 0x854);
    g_fogB54a.validateCs(cs);
    g_fogB54a.enterCs(cs);
    int32_t n = 0;
    int32_t buckets = static_cast<int32_t>(rd(p, 0x87c));
    uint32_t* heads = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(rd(p, 0x870)));
    for (int32_t i = 0; i < buckets; i++)
        for (uint32_t node = heads[i]; node; node = rd(reinterpret_cast<void*>(static_cast<uintptr_t>(node)), 0x5bc)) n++;
    g_fogB54a.leaveCs(cs);
    return n;
}

// 1.13c Fog+0xDB70 -- chain length of bucket idx, -1 if out of range.
extern "C" int32_t fog_sub_DB70(void* p, int32_t idx) {  // 1.13c Fog+0xDB70
    if (idx >= static_cast<int32_t>(rd(p, 0x87c))) return -1;
    void* cs = at(p, 0x854);
    g_fogB54a.validateCs(cs);
    g_fogB54a.enterCs(cs);
    int32_t n = 0;
    uint32_t* heads = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(rd(p, 0x870)));
    for (uint32_t node = heads[idx]; node; node = rd(reinterpret_cast<void*>(static_cast<uintptr_t>(node)), 0x5bc)) n++;
    g_fogB54a.leaveCs(cs);
    return n;
}

// 1.13c Fog+0xD270 -- copy count 0xb0-byte records out under lock, then patch dword 0x2b of each from +0xba8.
extern "C" void fog_sub_D270(void* p, uint32_t* dst, int32_t count) {  // 1.13c Fog+0xD270
    void* cs = at(p, 0x8b4);
    g_fogB54a.validateCs(cs);
    g_fogB54a.enterCs(cs);
    const uint32_t* src = reinterpret_cast<uint32_t*>(at(p, 0x8cc));
    uint32_t* d = dst;
    for (uint32_t n = static_cast<uint32_t>(count * 0xb0) >> 2; n; n--) *d++ = *src++;
    g_fogB54a.leaveCs(cs);
    const uint32_t* s2 = reinterpret_cast<uint32_t*>(at(p, 0xba8));
    for (int32_t i = 0; i < count; i++) dst[0x2b + i * 0x2c] = s2[i];
}

// 1.13c Fog+0x1D700
extern "C" void fog_sub_1D700(uint32_t* a, uint32_t* b, uint32_t c) {  // 1.13c Fog+0x1D700
    g_fogB54a.sub1D620(a, b, c, 0x40);
}

// 1.13c Fog+0x1C640 -- CPU-family gate.
extern "C" uint32_t fog_sub_1C640() {  // 1.13c Fog+0x1C640
    if (g_fogB54a.cpuLevel < 5) return 1;
    int32_t v = g_fogB54a.cpuVendor;
    uint32_t sig = g_fogB54a.cpuSignature;
    uint32_t type = sig >> 12 & 3, fam = sig >> 8 & 0xf, mod = sig >> 4 & 0xf;
    if (v == 1) {
        if (type == 0) return fam == 5 || (fam == 6 && mod < 3);
        return type == 1 && fam == 5;
    }
    if (v == 2) {
        if (fam == 4) return 1;
        if (fam != 5) return 0;
        return mod <= 8;  // mod<6, <8, ==8 return 1; >=9 falls to the v==3 test (false)
    }
    return v == 3;
}

// 1.13c Fog+0x170F0 -- find entry with key; out two values; 0 ok, 0x16 missing.
extern "C" uint32_t fog_sub_170F0(int32_t key, uint8_t* hdr, int32_t* o1, int32_t* o2) {  // 1.13c Fog+0x170F0
    uint32_t n = *reinterpret_cast<uint32_t*>(hdr + 4);
    const int32_t* e = reinterpret_cast<int32_t*>(hdr + 8);
    for (uint32_t i = 0; i < n; i++, e += 3)
        if (e[0] == key) { *o1 = e[1]; *o2 = e[2]; return 0; }
    return 0x16;
}

// src/core/storm/storm_batch_18f90.cpp -- UNMAPPED 1.13c Storm.dll rows 0x18F90, 0x18FE0, 0x1AD80,
// 0x35C70, 0x35FC0, 0x365E0 (alpha lane: marker only). Retail globals are file statics tagged with
// their 1.13c address; unmapped callees and user32 imports are seams on StormBatch18f90Env.
// SEVT records keep retail's 4-byte field layout (as storm_batch_19040.cpp).
#include <cstdint>

struct StormBatch18f90Env {
    uint32_t (*get_window_long)(void* hwnd, int32_t idx);       // user32 GetWindowLongA
    int32_t  (*get_window_rect)(void* hwnd, int32_t rect[4]);   // user32 GetWindowRect l,t,r,b
    int32_t  (*get_system_metrics)(int32_t idx);                // user32 GetSystemMetrics
    void     (*mem_free)(void* p, const char* file, int32_t line);          // 0x1B960 SMemFree
    void*    (*mem_alloc)(uint32_t size, const char* file, int32_t line, uint32_t flags); // 0x1C580
    uint32_t (*hash_string)(const uint8_t* s, uint32_t type, uint32_t seed);  // 0x3AC10
};

namespace {
uint32_t g_palFlags[0x100];       // 0x6FC453F0  4-byte entries, byte 3 = flag
uint32_t g_evtTable18;            // 0x6FC45370  hash buckets
uint32_t g_evtSize18;             // 0x6FC45374  bucket count
uint32_t g_hashValid;             // 0x6FC41D68
const uint8_t* g_hashKey;         // 0x6FC45A80
uint32_t g_hashSeed;              // 0x6FC45A84
int32_t  g_hashFirst;             // 0x6FC45A88
uint32_t g_hashResult;            // 0x6FC45A7C
struct EvtNode18 { uint32_t key, seq, handler, next; };   // 0x10
template <typename T> T* P(uint32_t v) { return (T*)(uintptr_t)v; }
}

// 1.13c Storm+0x18F90: default palette flag table -- 0..9 -> 0, 10..245 -> 5, 246..255 -> 0.
extern "C" void Storm_Sub18F90(void) {
    int i = 0;
    for (; i < 10; i++) ((uint8_t*)&g_palFlags[i])[3] = 0;
    for (; i < 0xf6; i++) ((uint8_t*)&g_palFlags[i])[3] = 5;
    for (; i < 0x100; i++) ((uint8_t*)&g_palFlags[i])[3] = 0;
}

// 1.13c Storm+0x18FE0: window-state test (hwnd in EAX). 0 if style has WS_MAXIMIZE (bit 24);
// else 1 unless WS_EX_TOOLWINDOW-bit 8 is set and the window reaches the screen edge.
extern "C" uint32_t Storm_Sub18FE0(void* hwnd, const StormBatch18f90Env* env) {
    if ((env->get_window_long(hwnd, -0x10) & 0x1000000) == 0) {
        if ((env->get_window_long(hwnd, -0x14) & 8) == 0) return 1;
        int32_t r[4];
        env->get_window_rect(hwnd, r);
        if (r[2] < env->get_system_metrics(0x10) && r[3] < env->get_system_metrics(0x11)) return 1;
    }
    return 0;
}

// 1.13c Storm+0x1AD80: cached string hash (string in ESI, seed in EBX); result in 1..0x7FFFFFFF.
extern "C" uint32_t Storm_Sub1AD80(const uint8_t* s, uint32_t seed, const StormBatch18f90Env* env) {
    uint32_t h = 0;
    int32_t first = s ? *(const int32_t*)s : 0;
    if (g_hashValid && s == g_hashKey && seed == g_hashSeed) {
        if (first == g_hashFirst) return g_hashResult;
        g_hashValid = 0;
    }
    if (s) h = env->hash_string(s, 1, seed);
    g_hashResult = h & 0x7fffffff;
    if (g_hashResult == 0) g_hashResult = 1;
    g_hashFirst = first;
    g_hashKey = s;
    g_hashSeed = seed;
    return g_hashResult;
}

// 1.13c Storm+0x35C70: SEVT lookup of node (key ESI, seq EDX) in the bucket table.
extern "C" uint32_t* Storm_Sub35C70(uint32_t key, uint32_t seq) {
    if (!g_evtTable18 || !g_evtSize18) return nullptr;
    EvtNode18* n = P<EvtNode18>(P<uint32_t>(g_evtTable18)[(key ^ seq) & (g_evtSize18 - 1)]);
    // retail chains through word 4 (offset 0x10) of the node
    while (n && (((uint32_t*)n)[0] != key || ((uint32_t*)n)[1] != seq))
        n = P<EvtNode18>(((uint32_t*)n)[4]);
    return (uint32_t*)n;
}

// 1.13c Storm+0x35FC0: free a SEVT bucket-group (EDI = {buckets, size, ...}).
extern "C" void Storm_Sub35FC0(uint32_t* grp, const StormBatch18f90Env* env) {
    uint32_t* b = P<uint32_t>(grp[0]);
    for (uint32_t i = 0; i < grp[1]; i++) {
        while (uint32_t* n = P<uint32_t>(b[i])) {
            b[i] = n[3];
            env->mem_free(n, "delete", -1);
            b = P<uint32_t>(grp[0]);
        }
    }
    env->mem_free(P<void>(grp[0]), "..\\3rdParty\\STORM\\SOURCE\\SEVT.CPP", 0x58);
    env->mem_free(grp, "delete", -1);
}

// 1.13c Storm+0x365E0: deep-copy bucket-group src (EBX) into dst.
extern "C" void Storm_Sub365E0(uint32_t* dst, const uint32_t* src, const StormBatch18f90Env* env) {
    static const char* f = "..\\3rdParty\\STORM\\SOURCE\\SEVT.CPP";
    dst[1] = src[1];
    dst[2] = src[2];
    dst[0] = (uint32_t)(uintptr_t)env->mem_alloc(src[1] << 2, f, 0x3e, 0);
    for (uint32_t i = 0; i < src[1]; i++) {
        uint32_t* slot = P<uint32_t>(dst[0]) + i;
        for (const uint32_t* o = P<uint32_t>(P<uint32_t>(src[0])[i]); o; o = P<uint32_t>(o[3])) {
            uint32_t* c = (uint32_t*)env->mem_alloc(0x10, f, 0x45, 0);
            *slot = (uint32_t)(uintptr_t)c;
            c[0] = o[0]; c[1] = o[1]; c[2] = o[2]; c[3] = o[3];
            slot = c + 3;
        }
        *slot = 0;
    }
}

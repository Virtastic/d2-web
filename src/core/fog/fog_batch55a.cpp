// Fog batch b55-fog-a. Env seams stand in for retail internals (no Win32 calls).
#include <stdint.h>
#include <stddef.h>
#include <string.h>

struct fog_batch55a_Env {
    // Fog+0xD9E0: lookup, returns record ptr (or null) and a critical section to leave.
    uint32_t* (*lookup)(int a, uint32_t b, void** cs);
    void (*leaveCs)(void* cs);
    uint32_t (*ord15)(uint16_t v);                 // Fog ordinal 15
    int (*sub_1E880)(uint32_t a, uint32_t b);
    void (*sha1Transform)(uint32_t* state);        // Fog+0x13520, buffer at state+5 words (ctx+0x1c)
    const uint8_t* sha1Pad;                        // 64-byte padding table at Fog+0x7FFA0
    const int32_t* bitIndexTable;                  // 256 entries at Fog+0x77B50
};
static fog_batch55a_Env g_fog_batch55a_env;
void fog_batch55a_setEnv(const fog_batch55a_Env* e) { g_fog_batch55a_env = *e; }

static uint32_t fog_batch55a_field(int a, uint32_t b, size_t off, bool viaOrd15) {
    void* cs = 0;
    uint32_t* rec = g_fog_batch55a_env.lookup(a, b, &cs);
    uint32_t r = 0;
    if (rec) {
        const uint8_t* p = (const uint8_t*)rec + off;
        if (viaOrd15) { uint16_t v; memcpy(&v, p, 2); r = (uint16_t)g_fog_batch55a_env.ord15(v); }
        else { uint32_t v; memcpy(&v, p, 4); r = v; }
    }
    if (cs) g_fog_batch55a_env.leaveCs(cs);
    return r;
}

// 1.13c Fog+0xED70
uint16_t fog_sub_ED70(int a, uint32_t b) { return (uint16_t)fog_batch55a_field(a, b, 0x7a, true); }
// 1.13c Fog+0xED30
uint32_t fog_sub_ED30(int a, uint32_t b) { return fog_batch55a_field(a, b, 0x23 * 4, false); }
// 1.13c Fog+0xECE0
uint16_t fog_sub_ECE0(int a, uint32_t b) { return (uint16_t)fog_batch55a_field(a, b, 0x8a, true); }

// 1.13c Fog+0x139E0  (SHA-1 update; ctx: u32 count lo/hi, u32 state[5], u8 buf[64] at +0x1c)
void fog_sub_139E0(uint8_t* ctx, const uint8_t* data, uint32_t len) {
    uint32_t lo, hi; memcpy(&lo, ctx, 4); memcpy(&hi, ctx + 4, 4);
    uint32_t add = len << 3, idx = (lo >> 3) & 0x3f;
    uint32_t nlo = lo + add, nhi = hi + (len >> 29) + (nlo < lo);
    memcpy(ctx, &nlo, 4); memcpy(ctx + 4, &nhi, 4);
    uint32_t* st = (uint32_t*)(ctx + 8);
    if (idx) {
        len += idx;
        data -= idx;
        if (len < 0x40) { for (; idx < len; idx++) ctx[idx + 0x1c] = data[idx]; return; }
        for (; idx < 0x40; idx++) ctx[idx + 0x1c] = data[idx];
        g_fog_batch55a_env.sha1Transform(st);
        data += 0x40; len -= 0x40; idx = 0;
    }
    while (len >= 0x40) {
        g_fog_batch55a_env.sha1Transform(st);
        data += 0x40; len -= 0x40;
    }
    for (; idx < len; idx++) ctx[idx + 0x1c] = data[idx];
}

// 1.13c Fog+0x13A90  (SHA-1 final, writes 20 big-endian digest bytes)
void fog_sub_13A90(uint8_t* ctx, uint8_t* out) {
    uint32_t lo, hi; memcpy(&lo, ctx, 4); memcpy(&hi, ctx + 4, 4);
    uint8_t cnt[8] = { (uint8_t)(hi >> 24), (uint8_t)(hi >> 16), (uint8_t)(hi >> 8), (uint8_t)hi,
                       (uint8_t)(lo >> 24), (uint8_t)(lo >> 16), (uint8_t)(lo >> 8), (uint8_t)lo };
    fog_sub_139E0(ctx, g_fog_batch55a_env.sha1Pad, ((0u - (lo >> 3) - 9) & 0x3f) + 1);
    fog_sub_139E0(ctx, cnt, 8);
    for (int i = 0; i < 5; i++) {
        uint32_t w; memcpy(&w, ctx + 8 + 4 * i, 4);
        out[4 * i] = (uint8_t)(w >> 24); out[4 * i + 1] = (uint8_t)(w >> 16);
        out[4 * i + 2] = (uint8_t)(w >> 8); out[4 * i + 3] = (uint8_t)w;
    }
}

// 1.13c Fog+0x13B70  (index of lowest set bit, via byte table; -1 if zero)
int fog_sub_13B70(uint32_t v) {
    for (int i = 0; i < 4; i++) {
        uint32_t b = (v >> (8 * i)) & 0xff;
        if (b) return g_fog_batch55a_env.bitIndexTable[b] + 8 * i;
    }
    return -1;
}

// 1.13c Fog+0x1E910
int fog_sub_1E910(const uint8_t* obj, uint32_t a, uint16_t b) {
    if (obj) {
        const int* p; memcpy(&p, obj + 0x110, sizeof p);
        if (p && *p) return g_fog_batch55a_env.sub_1E880(a, b);
    }
    return -1;
}

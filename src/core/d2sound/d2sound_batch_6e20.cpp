// D2sound batch d2sound-batch-24: 0x6E20 (ordinal 10008), 0x9A10 (ordinal 10013).
// Externals are host seams, same pattern as d2sound_batch_67c0.cpp.
#include <cstdint>
#include <cstring>

struct d2snd6e20_Env {
    int32_t  masterVol;                                  // DAT_6f9c5994
    int32_t  mode;                                       // DAT_6f9c6598
    int32_t  optMusic;                                   // DAT_6f9c64b4
    uint16_t (*attenuate)(float a, float b, float c);    // 0x8830
    int32_t  (*round)(void);                             // 0xACCC (x87 ST0 -> int)
    void     (*fatal)(int line);                         // Ordinal_10265(0x2cb) + Ordinal_10024 + _exit(-1)
    void     (*regWrite)(const char*, const char*, int, int); // Ordinal_426
};
static d2snd6e20_Env* g_d2snd6e20;
void d2snd6e20_SetEnv(d2snd6e20_Env* e) { g_d2snd6e20 = e; }

// 1.13c D2sound+0x6E20 (ordinal 10008): ECX = sound s (guest layout: [0] = the DirectSound interface, whose vtable +0x3C is
// SetVolume(iface, v), stdcall; +0x34 flags, +0x38 stored volume, +0x3C float f), EDX = vol 0..255.
//   v = masterVol * vol / 255 (C division); unless (flags & 4) && mode == 2: v = trunc((1.0 - f) * v)  (x87 fsubr/fimul, 0xACCC);
//   SetVolume(iface, trunc(0x8830(0, 255, (float)v))).
// A null s or a null interface is fatal (0x2CB, _exit(-1)).
static inline uint32_t d2s6e_g32(const void* s, uint32_t off) { uint32_t v; std::memcpy(&v, (const uint8_t*)s + off, 4); return v; }
void d2snd6e20_setVolume(void* s, int32_t vol) {
    d2snd6e20_Env* e = g_d2snd6e20;
    const uint32_t iface = s ? d2s6e_g32(s, 0) : 0;
    if (!iface) { e->fatal(0x2cb); return; }
    int32_t v = (int32_t)((uint32_t)e->masterVol * (uint32_t)vol);
    std::memcpy((uint8_t*)s + 0x38, &vol, 4);
    v /= 0xff;
    if (!(d2s6e_g32(s, 0x34) & 4) || e->mode != 2) {
        uint32_t fb = d2s6e_g32(s, 0x3c); float f; std::memcpy(&f, &fb, 4);
        v = (int32_t)((1.0 - (double)f) * (double)v);
    }
    e->attenuate(0.0f, 255.0f, (float)v);
    const int32_t out = e->round();
    const uint32_t fn = d2s6e_g32((const void*)(uintptr_t)d2s6e_g32((const void*)(uintptr_t)iface, 0), 0x3c);
    ((void (*)(uint32_t, int32_t))(uintptr_t)fn)(iface, out);
}

// 1.13c D2sound+0x9A10
int32_t d2snd6e20_setOptMusic(int32_t v) {
    d2snd6e20_Env* e = g_d2snd6e20;
    e->regWrite("Diablo II", "Options Music", 0, v);
    e->optMusic = v;
    return v;
}

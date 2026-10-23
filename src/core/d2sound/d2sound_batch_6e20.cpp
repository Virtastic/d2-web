// D2sound batch d2sound-batch-24: 0x6E20 (ordinal 10008), 0x9A10 (ordinal 10013).
// Externals are host seams, same pattern as d2sound_batch_67c0.cpp.
#include <cstdint>

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

struct d2snd6e20_Snd { int32_t* dsbuf; int32_t pad[12]; uint32_t flags; int32_t vol; };

// 1.13c D2sound+0x6E20
void d2snd6e20_setVolume(d2snd6e20_Snd* s, int32_t vol, void (*setDs)(int32_t* buf, int32_t v)) {
    d2snd6e20_Env* e = g_d2snd6e20;
    if (!s || !s->dsbuf) { e->fatal(0x2cb); return; }
    int32_t v = e->masterVol * vol;
    s->vol = vol;
    v /= 0xff;
    if (!(s->flags & 4) || e->mode != 2) v = e->round();
    e->attenuate(0.0f, 255.0f, (float)v);
    setDs(s->dsbuf, e->round());
}

// 1.13c D2sound+0x9A10
int32_t d2snd6e20_setOptMusic(int32_t v) {
    d2snd6e20_Env* e = g_d2snd6e20;
    e->regWrite("Diablo II", "Options Music", 0, v);
    e->optMusic = v;
    return v;
}

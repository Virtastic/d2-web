// src/core/d2sound/d2sound_batch_9a40.cpp -- 1.13c D2Sound.dll option/version helpers (0x9A40, 0x9B00, 0x9B30, 0x8A50).
// Fog ordinals 423 (registry read) / 426 (registry write) and the DAT_ globals are host seams.
// 0x8830 is x87 (result in ST0, decompile discards it): spec only.
#include <cstdint>

struct D2SndOptSeams {
    int (*regRead)(const char* app, const char* key, uint32_t def, uint32_t* out);   // Fog ord 423
    void (*regWrite)(const char* app, const char* key, uint32_t def, uint32_t val);  // Fog ord 426
};
struct D2SndOpts { uint32_t mixer, master, music, bias, npc, optMusic; };           // 6f9c64a0..64b4

// 1.13c D2sound+0x9A40
uint32_t D2sound_9A40(const D2SndOptSeams& s, D2SndOpts& o, uint32_t v) {
    s.regWrite("Diablo II", "NPC Speech", 0, v);
    return o.npc = v;
}

// 1.13c D2sound+0x9B00
uint32_t D2sound_9B00(const D2SndOptSeams& s, D2SndOpts& o, uint32_t v) {
    s.regWrite("Diablo II", "Sound Mixer", 0, v);
    return o.mixer = v;
}

// 1.13c D2sound+0x9B30
void D2sound_9B30(const D2SndOptSeams& s, D2SndOpts& o) {
    o.music = 50; o.bias = 50; o.mixer = 0; o.master = 100; o.npc = 2; o.optMusic = 1;
    uint32_t v;
    if (s.regRead("Diablo II", "Sound Mixer", 0, &v) && v < 3) o.mixer = v;
    if (s.regRead("Diablo II", "Master Volume", 0, &v) && v < 0x65) o.master = v;
    if (s.regRead("Diablo II", "Music Volume", 0, &v) && v < 0x65) o.music = v;
    if (s.regRead("Diablo II", "Positional Bias", 0, &v) && v < 0x65) o.bias = v;
    if (s.regRead("Diablo II", "NPC Speech", 0, &v) && v < 3) o.npc = v;
    if (s.regRead("Diablo II", "Options Music", 0, &v) && v < 2) o.optMusic = v;
}

// 0x8A50 moved to d2sound_8a50.cpp

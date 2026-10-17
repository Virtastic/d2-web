// src/core/d2sound/ -- 1.13c D2Sound.dll, reimplemented from the decompile.
#include <cstdint>

// 1.13c D2Sound+0x164A8 -- the module's stored music volume.
static uint32_t g_musicVolume;

// 1.13c D2Sound+0x9AA0, export ordinal 10004.
// The body is identity: the new volume is stored and returned unclamped; the registry
// write goes out through Storm #426 (SRegSaveValue) and is recorded, not modelled here.
uint32_t D2Sound_MusicVolumeSet(uint32_t vol) {
    g_musicVolume = vol;
    return vol;
}

// The FN probe reads the module global back out (the capture's gdump compares it).
uint32_t D2Sound_MusicVolumeGet(void) {
    return g_musicVolume;
}

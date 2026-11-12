// 1.13c D2sound+0x8F10 -- stdcall(guid, desc, module, ctx) ret 16: the DirectSoundEnumerate callback. A null guid or an already-seen pass (g65a4) returns 1 untouched.
// Otherwise it compares the module string with "EMU10K1.VXD" (-> g65ac = 1), "es197x.vxd" (-> g65b0 = 1), the description with "Crystal SoundFusion(tm)" (-> g65b0 = 1),
// the module with "YDSXG.VXD" and "RIPTIDE.VXD" (-> g5900 = 0), sets g65a4 = 1 and returns 1. Each compare is the first n bytes of the literal, NUL included (repe cmpsb).
#include <cstdint>

static bool d2snd_eq(const char* s, const char* lit, uint32_t n)
{
    for (uint32_t i = 0; i < n; ++i)
        if (s[i] != lit[i]) return false;
    return true;
}

// 1.13c D2sound+0x8F10
extern "C" uint32_t d2sound_8f10(uint32_t guid, const char* desc, const char* mod,
                                 uint32_t* g65a4, uint32_t* g65ac, uint32_t* g65b0, uint32_t* g5900)
{
    if (!guid || *g65a4) return 1;
    if (d2snd_eq(mod, "EMU10K1.VXD", 12)) *g65ac = 1;
    if (d2snd_eq(mod, "es197x.vxd", 11)) *g65b0 = 1;
    if (d2snd_eq(desc, "Crystal SoundFusion(tm)", 24)) *g65b0 = 1;
    if (d2snd_eq(mod, "YDSXG.VXD", 10)) *g5900 = 0;
    if (d2snd_eq(mod, "RIPTIDE.VXD", 12)) *g5900 = 0;
    *g65a4 = 1;
    return 1;
}

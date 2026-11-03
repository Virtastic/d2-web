// 1.13c D2Multi+0x14680 -- the "LVL_REST" registry setting. The host calls go through the env struct the probe passes;
// the Storm string arguments ("Diablo II", "LVL_REST", the 0x74E8 table) are retail image constants and are not carried.
#include <cstdint>

struct D2m14680Env {
    uint32_t (*storm423)(uint32_t* out);                       // Storm 423 (app, key, 0, out): nonzero = found, writes *out
    void (*storm426)(uint32_t value);                          // Storm 426 (app, key, 0, value): write the setting
    void (*storm578)(uint32_t* buf, uint32_t size, uint32_t value);   // Storm 578, cdecl, 4 words: (buf, 4, table 0x74E8, value)
    void (*d2win7c44)(uint32_t g9d3c, uint32_t* buf);          // thunk 0x7C44: ECX = global 0x9D3C, EDX = buf
    void (*d2win7c1a)(uint32_t g9d40, uint32_t flag);          // thunk 0x7C1A: ECX = global 0x9D40, EDX = flag
    void (*sub13de0)(void);
    uint32_t g9d3c, g9d40;                                     // D2Multi globals 0x39D3C / 0x39D40
};

extern "C" void d2multi_14680_env(const D2m14680Env* e) {
    uint32_t v[2] = { 4, 0 };                                  // v[0] = setting (default 4), v[1] = the 0x34 word
    if (!e->storm423(&v[0])) { e->storm426(v[0]); return; }
    v[1] = 0x34;
    if (v[0] == 0x29a) { e->d2win7c1a(e->g9d40, 0); e->sub13de0(); return; }
    e->storm578(&v[1], 4, v[0]);
    e->d2win7c44(e->g9d3c, &v[1]);
    e->d2win7c1a(e->g9d40, 1);
    e->sub13de0();
}

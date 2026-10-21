// D2Client+0x38E80: build the ".key" file path (base dir, optional subdir) and call Ordinal_10107; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c38E80Env {
    void (*o10115)(char* base110);                              // Ordinal_10115 (fills base dir; arg unrecovered)
    void (*o10114)();                                           // Ordinal_10114
    void (*fmt)(char* dst, int32_t cap, const char* a, const char* b);  // Ordinal_578 (snprintf-like)
    int32_t (*o10107)(const char* path);                        // Ordinal_10107(0,0)
    const char* sub;                                            // DAT_6fbc9860 (subdir, "" = none)
    const char* name;                                           // DAT_6fbc9928
};
extern "C" {
// 1.13c D2Client+0x38E80
EMSCRIPTEN_KEEPALIVE void d2c_38e80(const D2c38E80Env* e) {
    char base[0x105] = {0}, dir[0x105] = {0}, key[0x105] = {0};
    e->o10115(base);
    if (e->sub[0]) {
        e->fmt(dir, 0x104, base, e->sub);          // "%s%s\\"
        e->o10114();
        e->fmt(key, 0x104, dir, e->name);          // "%s%s.key"
        if (e->o10107(key) != -1) return;
    }
    e->fmt(key, 0x104, base, e->name);             // "%s%s.key"
    e->o10107(key);
}
}

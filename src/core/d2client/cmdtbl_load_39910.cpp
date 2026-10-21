// D2Client+0x39910: load default key-command table (CmdTbl.cpp); env-seam style.
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2c39910Env {
    int32_t (*ord10004)(char* name, int32_t zero, int32_t* flag);   // Ordinal_10004; flag = iStack_4
    void (*ord10116)();
    int32_t (*ord10107)(int32_t, int32_t, int32_t, int32_t, int32_t);
    void (*ord10110)(int32_t size, char* path, int32_t zero);
    void (*ord10109)();
    int32_t (*ord10110ret)();                                       // ESI after 10110/10109
    int16_t* (*ord10045)(const char* file, int32_t line, int32_t zero);  // alloc
    int32_t (*ord10016)(int32_t);
    int16_t* (*f_d810)(int32_t h, const char* path);                // D2Client+0xD810
    void (*ord10043)(int32_t msg, int32_t zero);
    int16_t* tmp490;      // 0x47a-byte staged data (asStack_490)
    uint32_t* dat4a98;    // DAT_6fba4a98, 0x11d dwords
};
extern "C" {
// 1.13c D2Client+0x39910
EMSCRIPTEN_KEEPALIVE void d2c_39910(const D2c39910Env* e) {
    char name[248]; int32_t flag = 0; int16_t* p = nullptr; bool got = false;
    e->ord10004(name, 0, &flag);
    if (flag) {
        e->ord10116();
        strcat(name, "default_key");
        if (e->ord10107(0, 0, 3, 0x80, 0) != -1) {
            e->ord10110(0x47a, name, 0);
            e->ord10109();
            if (e->ord10110ret() == 0x47a) {
                p = e->ord10045("..\\Source\\D2Client\\UI\\CmdTbl.cpp", 0x527, 0);
                memcpy(p, e->tmp490, 0x47a);
                got = true;
            }
        }
    }
    if (!got) {
        char path[256];
        snprintf(path, sizeof path, "%s\\CMD\\%s\\default.key", "DATA\\LOCAL", name);
        p = e->f_d810(e->ord10016(0), path);
    }
    if (p) {
        int32_t m = 0x549;
        if (p[0] == 0x5357 && p[1] == 0x25 && p[2] == 0x47a) {
            memcpy(e->dat4a98, p + 3, 0x474);
            m = 0x544;
        }
        e->ord10043(m, 0);
    }
}
}

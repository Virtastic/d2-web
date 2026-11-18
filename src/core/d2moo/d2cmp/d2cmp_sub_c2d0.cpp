// 1.13c D2CMP+0x0C2D0 (sub_C2D0): build the DCC/DC6 asset path for a cached-graphic descriptor.
// ours: src/core/d2cmp/d2cmp_sub_c2d0.cpp:d2cmp_sub_c2d0
// Stateful (global DAT_6fe32cc8, Ordinal_509, descriptor flags written back); NAMED only, no vector.
// Implicit regs: ESI = descriptor (d), EBX = name/token string; param_1 = out buffer.
// ponytail: decompile shows 3 wsprintfA args for a 4-%s format on the token path (lossy); unverified, resolve in a trace.
#include <cstdint>
#include <cstdio>

struct D2Cmp0C2D0Env {
    int32_t useDcc;                 // DAT_6fe32cc8
    int32_t (*ord509)();            // Ordinal_509
};

#define TOK(s, o) do { for (int k_ = 0; k_ < 3; k_++) (o)[k_] = (s)[k_] == 0x20 ? 0 : (char)(s)[k_]; (o)[3] = 0; } while (0)

uint32_t d2cmp_sub_c2d0(char* out, const char* name, uint8_t* d, D2Cmp0C2D0Env& e) {
    auto i32 = [&](int o) -> int32_t& { return *(int32_t*)(d + o); };
    uint32_t& fl = *(uint32_t*)(d + 0x44);
    int32_t useDcc = e.useDcc;
    const char* dir; const char* fmt;
    if (*name == 0) { *out = 0; return 0; }
    int32_t t = i32(8);
    if (t == 3) {
        if (!i32(0x2c)) { *out = 0; return 0; }
        dir = "DATA\\GLOBAL\\MISSILES";
        if (e.useDcc) { sprintf(out, "%s\\%s.dcc", dir, name); fl |= 1; return 1; }
    } else if (t == 4) {
        if (!i32(0x2c) || (i32(0x10) != 4 && i32(0x10) != 3)) { *out = 0; return 0; }
        sprintf(out, "%s\\items\\%s.dc6", "DATA\\GLOBAL", name); return 1;
    } else if (t != 6) {
        if (e.useDcc && e.ord509() == 0) { useDcc = 0; fl &= ~1u; }
        t = i32(8);
        if (t == 0) dir = "DATA\\GLOBAL\\CHARS";
        else if (t == 1) {
            int32_t id = i32(0xc);
            dir = "DATA\\GLOBAL\\MONSTERS";
            bool no = false;
            switch (id) {
            case 0xf2: case 0xfb: case 0x16f: case 0x209: case 0x2c0: no = true; break;
            case 0xf3: case 0x11c: case 0x14d: case 0x220: case 0x22f: case 0x23a: case 0x2c1: case 0x2c5:
                no = i32(0x10) == 0; break;
            }
            if (no) useDcc = 0;
        } else if (t == 2) {
            if (i32(0xc) == 0x156 || i32(0xc) == 0x233) useDcc = 0;
            dir = "DATA\\GLOBAL\\OBJECTS";
        } else { *out = 0; return 0; }
        char a[4], b[4]; TOK(d + 0x18, a); TOK(d + 0x1c, b);
        if (useDcc) { sprintf(out, "%s\\%s\\%s\\%s.dcc", dir, a, b, name); fl |= 1; return 1; }
        sprintf(out, "%s\\%s\\%s\\%s.dc6", dir, a, b, name); return 1;
    } else {
        if (!(d[4] & 2)) { *out = 0; return 0; }
        dir = "DATA\\GLOBAL\\OVERLAYS";
        if (i32(0x14) != 0x7a && e.useDcc) { sprintf(out, "%s\\%s.dcc", dir, name); fl |= 1; return 1; }
    }
    fmt = "%s\\%s.dc6"; sprintf(out, fmt, dir, name); return 1;
}

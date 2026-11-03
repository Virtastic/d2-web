// 1.13c D2Client+0x61490 (736 B): automap colour for a unit. EAX = unit (may be 0), EBX = out byte pointer, no stack args; returns 1 and writes *out
// (or 0 and leaves it, or 1 and leaves it unwritten: the relation switch default). The colour bytes are the globals C210..C218.
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32;
struct D2c61490Env {
    u32 (*c1f0)(u32 a);                          // stdcall 1
    u32 (*c1de)(u32 a);                          // stdcall 1
    u32 (*f1760)(u32 type, u32 group);           // EAX = type, EDX = 9
    void (*c658)(u32 unit, i32* kind, i32* type, i32* mode, u32 file, u32 line);   // stdcall 6
    u32 (*cbda)(u32 unit, u32 what);             // stdcall 2
    u32 (*f560d0)(u32 unit);                     // D2Client+0xA60D0, EAX = unit
    u32 (*f55b60)(u32 unit);                     // D2Client+0xA5B60 (party id), EAX = unit, low 16 bits used
    u32 (*f219f0)(u32 v);                        // EAX = v
    u32 (*cc52)(u32 p);                          // stdcall 1
};
static inline u32 r32(u32 a) { return *(const u32*)(uintptr_t)a; }
// 1.13c D2Client+0x61490
u32 d2c_61490(u32 unit, uint8_t* out, u32 g_bbfc, u32 g_f16b8, const uint8_t* col /* C210..C218 */, const D2c61490Env* e) {
    i32 k = unit ? (i32)r32(unit) : 6;
    if (k == 0) {
        if (unit && r32(unit + 0x10) == 0x11) {
            u32 p = r32(unit + 0x60);
            if (!p) return 0;
            if (e->cc52(p) != r32(unit + 0xc)) return 0;
            *out = col[2]; return 1;
        }
        if (unit == g_bbfc) { *out = col[0]; return 1; }
        u32 di = e->f55b60(g_bbfc) & 0xffff;
        if (di != 0xffff && di == (e->f55b60(unit) & 0xffff)) { *out = col[3]; return 1; }
        *out = col[1]; return 1;
    }
    if (k == 1) {
        i32 type;
        if (!unit) type = -1;
        else {
            if (r32(unit + 0x10) == 0xc) return 0;
            if ((r32(unit + 0xc4) >> 0x15) & 1) return 0;
            type = (i32)r32(unit + 4);
        }
        if (e->f1760((u32)type, 9)) {
            if (unit) { u32 t = r32(unit + 4); if (t == 0x219 || t == 0x21a || t == 0x21b) return 0; }
            *out = col[6]; return 1;
        }
        i32 kind = unit ? (i32)r32(unit) : 6, ty = unit ? (i32)r32(unit + 4) : -1, mode = unit ? (i32)r32(unit + 0x10) : 0;
        e->c658(unit, &kind, &ty, &mode, 0x6fb8537c, 0x92d);
        if (kind == 0 && e->cbda(unit, 0x25) && e->f560d0(unit)) {
            u32 di = e->f55b60(g_bbfc) & 0xffff;
            if (di != 0xffff && di == (e->f55b60(unit) & 0xffff)) { *out = col[4]; return 1; }
            *out = col[1]; return 1;
        }
        u32 rel = e->f219f0(unit ? r32(unit + 0xc) : 0xffffffffu);
        if (rel > 3) return 1;
        if (rel == 0 || rel == 3) return 0;
        if (rel == 1) { *out = col[4]; return 1; }
        if (!g_f16b8) return 0;
        *out = col[5]; return 1;
    }
    if (k == 2) {
        i32 type = (i32)r32(unit + 4);
        if (type == 0x3b) { *out = col[8]; return 1; }
        if (type == 0x3c) {
            u32 v = e->c1de(e->c1f0(unit));
            if (v == 0x7d || v == 0x7e || v == 0x7f || v == 0x6f || v == 0x70 || v == 0x75) return 0;
            *out = col[8]; return 1;
        }
        if (type == 0x10b) { *out = 0; return 1; }
        return 0;
    }
    return 0;
}

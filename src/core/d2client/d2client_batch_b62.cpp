// D2Client batch b62-cli-r: register-arg ports (EAX etc. become explicit leading params; globals/callees injected).
#include <cstdint>
#include <cstring>
typedef int32_t i32; typedef uint32_t u32;

// 1.13c D2Client+0x63000  (EAX = mode; DAT_6fbcc098 = narrow flag; DAT_6fb8bc48/4c = screen w/h)
struct B62Menu { i32 x[8], y[8], a[8], idx[8]; };   // DAT_6fbc54f0 / 5510 / 5530 / 5570
void d2c_63000(i32 mode, B62Menu* m, i32 narrow, i32 scrW, i32 scrH) {
    i32 n = narrow ? 7 : 8, extra = 0, extra2 = 0;
    if (!mode) return;
    i32 half = scrW / 2, x = half - (narrow ? 0x4a : 0x54), y = scrH - 0x32;
    if (mode == 1) x = half - 0xca;
    else if (mode == 3) { x = half + 0x39; if (!narrow) x = half + 0x23; }
    m->a[0] = 0; m->idx[0] = 0; m->x[0] = x; m->y[0] = y;
    for (i32 i = 1; i < n; i++) {
        x += 0x15; m->x[i] = x; m->y[i] = y;
        i32 v;
        if (!narrow) { m->a[i] = i; v = i * 2; }
        else {
            if (i == 3) { extra += 2; extra2++; }
            m->a[i] = extra2 + i; v = extra + i * 2;
        }
        m->idx[i] = v;
    }
}

// 1.13c D2Client+0x644F0  (EAX = mode; mouseX=DAT_6fbcb828, mouseY=DAT_6fbcb824)
void d2c_644F0(i32 mode, const B62Menu* m, i32 narrow, i32 scrW, i32 scrH, i32 mouseX, i32 mouseY,
               void (*f63940)(i32, i32)) {
    i32 n = narrow ? 7 : 8, off = 0;
    if (!mode) return;
    if (mode == 1) off = -0x76; else if (mode == 3) off = 0x77;
    i32 c = scrW / 2 + off;
    if (c - 0x54 < mouseX && mouseX < c + 0x59 && scrH - 0x45 < mouseY && mouseY < scrH - 0x2f)
        for (i32 i = 0; i < n; i++)
            if (m->x[i] < mouseX && mouseX < m->x[i] + 0x14) f63940(m->x[i], m->y[i]);
}

// 1.13c D2Client+0x63B20  (EAX = mode; flag[]=DAT_6fbc5550; cel=DAT_6fbcc084)
void d2c_63B20(i32 mode, const B62Menu* m, const i32* flag, i32 narrow, u32 cel,
               void (*ord10041)(const u32*, i32, i32, i32, i32, i32)) {
    if (!mode) return;
    for (i32 i = 0, n = narrow ? 7 : 8; i < n; i++) {
        u32 rec[18] = {0};
        rec[0] = (u32)m->idx[i] + (flag[i] == 1 ? 1 : 0);
        rec[13] = cel;
        ord10041(rec, m->x[i], m->y[i], -1, 5, 0);
    }
}

// 1.13c D2Client+0x68FD0  (EAX = owner; list head at +0x2c, next at +0xa4, flag at +0x40)
void d2c_68FD0(const uint8_t* owner, void (*f58630)(const uint8_t* node), void (*ord10043)(i32, i32),
               const uint8_t* (*next)(const uint8_t*)) {
    // ponytail: node deref injected via `next` (wasm64 vs 32-bit pointers); head fetched through it too.
    for (const uint8_t* n = next(owner); n; n = next(n)) {   // caller: next(owner)=+0x2c, next(node)=+0xa4
        i32 flag; memcpy(&flag, n + 0x40, 4);
        if (flag) f58630(n);
        ord10043(0x690, 0);
    }
}

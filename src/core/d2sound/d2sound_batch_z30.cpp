// D2sound batch z30. Channel = uint32_t[] retail struct view (32-bit links); retail globals/callees are seams.
#include <cstdint>
#include <cstdlib>
typedef uint32_t* z30_ch;
extern "C++" {
// seams
[[noreturn]] void z30_fatal(uint32_t strId);              // Ordinal_10265 + Ordinal_10024 + _exit(-1)
void z30_lock(); void z30_unlock();                        // CritSect 6f9c6530
void z30_ds_call(z30_ch c, uint32_t slot, uint32_t a, uint32_t b); // vtable call on *c (IDirectSoundBuffer), slot = byte offset
void z30_ds3d_call(z30_ch c, uint32_t slot, uint32_t a, uint32_t b); // vtable call on c[3]
void z30_6680(z30_ch c); void z30_68d0(z30_ch c, int a, int b, int d); void z30_6ce0(z30_ch c, float f);
void z30_69f0(); void z30_7090(uint32_t flags); void z30_7bc0(); void z30_6fa0(z30_ch c);
int  z30_66f0(z30_ch c, int a, int b);
int  z30_baccc(float f); float z30_8830(float a, float b, float c); float z30_9cf0(float f);
uint32_t z30_tick(); void z30_reg_set(const char* app, const char* key, uint32_t def, uint32_t v); // Ordinal_426
void z30_ord10043(uint32_t a, uint32_t b);
void z30_enter_b(); void z30_leave_b(); void z30_delete_b(); // CritSect 6f9c6548
// globals
extern uint32_t g_5900, g_5948, g_5990, g_5994, g_658c, g_65b4, g_65b8, g_65bc, g_65c0, g_65c4, g_65dc, g_65e0, g_6598, g_64b4;
extern z30_ch g_6580; extern z30_ch g_6560[8];
extern float g_970, g_974, g_978, g_97c;
uint32_t* z30_music_ent(int set, int idx); // {name, played}; set!=0 -> introedit table @5950 else @5908
z30_ch z30_chan_at(uint32_t link); // 32-bit link -> channel

// 1.13c D2sound+0x06980
void z30_6980(z30_ch c) {
    if (!c || !c[0]) z30_fatal(0x292);
    z30_lock();
    if (c[0x12]) z30_6680(c);
    z30_ds_call(c, 0x48, 0, 0);
    c[7] = 0;
    z30_unlock();
}
// 1.13c D2sound+0x08020
void z30_8020() {
    if (!g_65b8) return;
    for (int i = 0; i < 8; i++) {
        z30_ch c = g_6560[i];
        if (!c || !c[0]) z30_fatal(0x292);
        z30_lock();
        if (c[0x12]) z30_6680(c);
        z30_ds_call(c, 0x48, 0, 0);
        c[7] = 0;
        z30_unlock();
    }
}
// 1.13c D2sound+0x067C0
bool z30_67c0(z30_ch c) { if (!c) z30_fatal(0x37a); return c[0x12] != 0; }
// 1.13c D2sound+0x06920
uint32_t z30_6920(z30_ch c) { if (!c) z30_fatal(0x2e4); return c[0xe]; }
// 1.13c D2sound+0x06E20
void z30_6e20(z30_ch c, int v) {
    if (!c || !c[0]) z30_fatal(0x2cb);
    int t = (int)g_5994 * v;
    c[0xe] = v;
    t /= 0xff;
    if (!(c[0xd] & 4) || g_6598 != 2) t = z30_baccc((float)t);
    z30_ds_call(c, 0x3c, (uint32_t)z30_baccc(z30_8830(0.0f, 255.0f, (float)t)), 0);
}
// 1.13c D2sound+0x07650
void z30_7650(z30_ch c) {
    if (!c || !c[0]) z30_fatal(0x226);
    if (c[0x12]) z30_fatal(0x227);
    z30_lock();
    if (!g_5900) { uint32_t f = c[0xd]; z30_69f0(); z30_7090(f); }
    else { c[5]=c[6]=c[8]=c[9]=c[0xb]=c[0x10]=c[0x11]=0; }
    z30_ds_call(c, 0x34, 0, 0);
    if (c[3]) { z30_ds3d_call(c, 0x40, 0x42c80000, 0); z30_ds3d_call(c, 0x44, 0x40000000, 0); }
    if (!(c[0xd] & 2)) z30_ds_call(c, 0x40, 0, 0); else z30_68d0(c, 0, 0, 0);
    z30_6ce0(c, 0.0f);
    z30_6e20(c, 0xff);
    z30_unlock();
}
// 1.13c D2sound+0x087D0
float z30_87d0(float p) {
    if (g_978 < p + g_97c) return g_974;
    if (p - g_97c < g_970) return g_978;
    return z30_9cf0(p);
}
// 1.13c D2sound+0x09A10
uint32_t z30_9a10(uint32_t v) { z30_reg_set("Diablo II", "Options Music", 0, v); g_64b4 = v; return v; }
// 1.13c D2sound+0x07FE0
void z30_7fe0() {
    if (g_6580) { g_65c0 = 0; if (g_6580[0x12]) z30_6980(g_6580); }
}
// 1.13c D2sound+0x06B80
void z30_6b80(z30_ch c, uint32_t target, int ms) {
    if (!c) z30_fatal(0x39e);
    if (!g_658c) return;
    z30_lock();
    c[0x13] = 1; c[0x14] = c[0xe]; c[0x15] = target;
    uint32_t t = z30_tick(); c[0x16] = t; c[0x17] = t + ms;
    z30_unlock();
}
// 1.13c D2sound+0x08160
void z30_8160() {
    if (!g_65b8) return;
    z30_enter_b();
    z30_8020();
    for (int i = 0; i < 8; i++) { z30_6fa0(g_6560[i]); g_6560[i] = 0; }
    while (g_65bc) {
        z30_ch n = z30_chan_at(g_65bc);
        uint32_t next = n[0x17];
        if (n[0x14]) z30_ord10043(0x67, 0);
        z30_ord10043(0x68, 0);
        g_65bc = next;
    }
    g_65b8 = 0;
    z30_delete_b();
}
// 1.13c D2sound+0x07D30
static void z30_pick_check(uint32_t cnt, int idx) {
    if (cnt != g_5990) z30_fatal(0x52);
    if (idx >= (int)cnt) z30_fatal(0x53);
}
static void z30_music_init() { if (!g_65e0) { g_65e0 = 1; g_65dc = g_65b4; } }
uint32_t z30_7d30() {
    z30_lock();
    if (!g_6580 || !g_658c || !g_5994 || g_6580[0x12]) { z30_unlock(); return 0; }
    bool retried = false;
    uint32_t* e;
    for (;;) {
        int found = 0;
        for (int i = 0; i < (int)g_5948; i++) {
            z30_music_init(); z30_pick_check(g_5948, i);
            if (!z30_music_ent(g_65dc != 0, i)[1]) { found = 1; break; }
        }
        if (!found) {
            if (retried) { g_65c0 = 0; z30_unlock(); return 0; }
            z30_7bc0(); retried = true;
        }
        if (!g_65c4) {
            int i = rand() % (int)g_5948;
            for (;;) {
                z30_music_init(); z30_pick_check(g_5948, i);
                e = z30_music_ent(g_65dc != 0, i);
                if (!e[1]) break;
                if (++i >= (int)g_5948) i = 0;
            }
        } else {
            z30_music_init(); z30_pick_check(g_5948, 0);
            e = z30_music_ent(g_65dc != 0, 0);
            g_65c4 = 0;
        }
        e[1] = 1;
        if (g_6580 && g_658c && g_5994 && !g_6580[0x12] && e[0]) {
            z30_7650(g_6580); z30_6e20(g_6580, 0x6e);
            if (z30_66f0(g_6580, 0, 0)) { z30_unlock(); return 1; }
        }
    }
}
}

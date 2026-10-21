// Env-seam port of the two-layout label screen drawn by D2Client+0x37C80.
// Every callee goes through one call hook in retail order; ordinals are tagged 0x10000|ordinal, others are RVAs.
// A visible argument the decompile does not show (register-passed) is passed as 0.
#include <cstdint>
#include <initializer_list>
struct Ops37C80 {
    virtual uint32_t call(uint32_t id, uint32_t a, uint32_t b, uint32_t c) = 0;
    virtual void clearBuf100(int slot) = 0;   // 100x Unicode default-constructor closure on stack slot (0 = 338, 1 = 1b4, 2 = fbe8, 3 = 294, 4 = 134)
};
struct State37C80 {   // globals read
    uint32_t bad_state;      // DAT_6fbaadd4 (0x2F9DD4 style offset; see spec)
    int32_t panelCount;      // DAT_6fb8d5d0, must be 0x26
    int32_t fb3a68;          // DAT_6fbc3a68
    uint8_t slotActive[0x26];// DAT_6fbaad80[]
    int32_t slotMode[0x26];  // *(PTR_DAT_6fb8d538[i] + 0x54)
    int32_t fbc9721;         // DAT_6fbc9721
    const int32_t* unitFlag; // DAT_6fbcbbfc (null or ptr to word)
    int32_t fbcc44c;         // DAT_6fbcc44c, set to 1
};
static const int O = 0x10000;
struct Row { uint16_t a, b, s; };   // fn36580(a,b); ord10003(s); ord10150
static void rows(Ops37C80& o, const Row* r, int n) {
    for (int i = 0; i < n; i++) { o.call(0x36580, r[i].a, r[i].b, 0); o.call(O | 10003, r[i].s, 0, 0); o.call(O | 10150, 0, 0, 0); }
}
static void sp(Ops37C80& o, int slot, uint32_t y) {
    o.clearBuf100(slot);
    uint32_t p = o.call(0x391E0, 1, 0, 0);
    p = o.call(O | 10003, p, 0, 0);
    o.call(O | 10002, 100, p, slot);   // Unicode::sprintf(100, buf, p)
    o.call(O | 10150, y, 0, 0);
}
static const Row kA[] = {{0x106f,0x3b,0x1e1},{0x1070,0x86,0x19a},{0x1071,0x86,0x1a9},{0x1072,0x86,0x1b8},{0xf92,0xde,0x177},{0x1074,0x108,0x1fa},{0x1075,0x108,0x209},{0x1076,0x13a,0x1e1},{0x1077,0x171,0x1f9},{0x1078,0x171,0x208},{0x1079,0x1c1,0x191},{0x107a,0x1c1,0x1a0},{0x107b,0x1c1,0x1af},{0x107c,0x1c1,0x1be},{0x107d,0x216,0x208},{0xf93,0x23c,0x177},{0x1073,0x2a2,0x19a},{0x1071,0x2a2,0x1a9},{0x1072,0x2a2,0x1b8},{0x107e,0x2e8,0x1e1}};
static const Row kB[] = {{0x106f,0x37,0x168},{0x1070,0x83,0x122},{0x1071,0x83,0x131},{0x1072,0x83,0x140},{0x1074,0xb6,0x181},{0x1075,0xb6,400},{0x1076,0xe7,0x168},{0x1077,0x120,0x181},{0x1078,0x120,400},{0x1079,0x162,0x113},{0x107a,0x162,0x122},{0x107b,0x162,0x131},{0x107c,0x162,0x140},{0x107d,0x1c1,400},{0x1073,0x1fb,0x122},{0x1071,0x1fb,0x131},{0x1072,0x1fb,0x140},{0x107e,0x24a,0x168}};

// 1.13c D2Client+0x37C80
extern "C" void d2client_sub_37C80(Ops37C80& o, State37C80& g) {
    if (!g.bad_state) {
        o.call(O | 10227, 0, 0, 0);
        if (g.panelCount != 0x26) { o.call(O | 10265, 0, 0, 0); o.call(O | 10024, 0, 0, 0); o.call(0x3664, 0xFFFFFFFFu, 0, 0); return; }
        if (!g.fb3a68) {
            for (int i = 0; i < g.panelCount; i++) {
                if (g.slotActive[i] != 1) continue;
                int m = g.slotMode[i];
                if (m == 1) o.call(0xC2790, i, 1, 0);
                else if (m == 4 && g.fbc9721) o.call(0x47B20, 0, 0, 0);
            }
            if (!g.unitFlag || (*g.unitFlag == 0 && o.call(0xA5440, 0, 0, 0) == 0)) {
                g.bad_state = 1;
                o.call(O | 10048, 0, 0, 0);
                o.call(0x3CCC0, 0, 0, 0);
            }
        }
        g.fbcc44c = 1;
    }
    bool a = o.call(O | 10031, 0, 0, 0) == 2;
    if (a) { o.call(0x361D0, 0, 0, 0); o.call(0x36660, 0, 0, 0); o.call(0x36170, 0, 0, 0); o.call(0x359B0, 0, 0, 0); }
    else   { o.call(0x362E0, 0, 0, 0); o.call(0x36660, 0, 0, 0); o.call(0x36170, 0, 0, 0); o.call(0x35DF0, 0, 0, 0); }
    uint16_t t0 = a ? 0x17c : 0x131, t1 = a ? 0x2bd : 0x21d;
    o.call(0x36580, 0x1066, t0, 0); o.call(O | 10003, 0, 0, 0); o.call(O | 10150, 0, 0, 0);
    o.call(0x36580, 0x1030, t1, 0); o.call(O | 10003, 0, 0, 0); o.call(O | 10150, 0, 0, 0);
    o.call(O | 10184, 0, 0, 0);
    sp(o, 0, 0x4e); sp(o, 1, 0x62); sp(o, 2, 0x76); sp(o, 3, 0x8a);
    for (uint32_t s : {0x9eu, 0xb2u, 0xc6u}) { o.call(O | 10003, s, 0, 0); o.call(O | 10150, 0, 0, 0); }
    sp(o, 4, 0xda);
    if (a) rows(o, kA, sizeof kA / sizeof *kA); else rows(o, kB, sizeof kB / sizeof *kB);
    o.call(O | 10184, 0, 0, 0);
}

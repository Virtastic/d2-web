// D2Client rva 0x7F780: the speech/chat packet handler. __thiscall(this, pkt).
// Callees are injected; Unicode strings are 16-bit.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2Client7F780Callees {
    int32_t (*c4D630)(uint32_t self, uint32_t v);                       // 0x9D630
    uint32_t (*c4DD80)();                                               // 0x9DD80
    uint32_t (*c2B4F0)(uint32_t self, const char* text);                // 0x7B4F0 (low byte tested)
    void (*c2D850)();                                                   // 0x7D850
    int32_t* (*c54E20)(uint32_t unit, uint32_t table, uint32_t idx);    // 0xA4E20
    void (*win2Unicode)(uint16_t* dst, const char* src, uint32_t n);
    uint16_t* (*str10003)(uint16_t* a, uint16_t* b);                    // D2Lang ordinal 10003 (args as seen)
    void (*usprintf)(uint32_t n, uint16_t* dst, uint16_t* fmt);         // Unicode::sprintf (varargs lost in decompile)
    void (*c6E760)(uint16_t* s);                                        // 0xBE760
    void (*c6F5F0)(uint32_t v);                                         // 0xBF5F0
    void (*c2B470)(uint32_t self, const char* text);                    // 0x7B470
    uint32_t (*ord10009)();
    void (*c47A70)(uint32_t self, uint16_t* s, uint32_t v);             // 0x97A70
    void (*mbToWide)(const char* src, uint16_t* dst, uint32_t n);      // MultiByteToWideChar(0,1,..)
};

static void ustrcpy(uint16_t* d, const uint16_t* s) { while ((*d++ = *s++) != 0) {} }
static void ustrcat(uint16_t* d, const uint16_t* s) { while (*d) d++; ustrcpy(d, s); }

// 1.13c D2Client+0x7F780
extern "C" void d2_client_7f780(uint32_t self, const uint8_t* p, uint32_t base, const D2Client7F780Callees& k) {
    uint32_t p4; memcpy(&p4, p + 4, 4);
    if (!(p[3] != 0 || k.c4D630(self, p4) == 0 || k.c4DD80() == 0)) return;
    uint16_t d90[512] = {}, b90[1224] = {}, c4c8[1224] = {}, dd0[32] = {}, db0[32] = {}, w6c8[256] = {};
    const char* text = (const char*)(p + 0x1a);
    const char* name = (const char*)(p + 10);
    if ((uint8_t)k.c2B4F0(self, text) == 0) return;
    if (p[1] == 4) { k.c2D850(); return; }
    int32_t* unit = k.c54E20(p4, base + 0x10A608 + (uint32_t)p[3] * 0x80, p[3]);
    const int nameLen = (int)strlen(name), textLen = (int)strlen(text);
    switch (p[1]) {
    case 6:
        k.win2Unicode(c4c8, text, 0x100);
        k.win2Unicode(dd0, name, 0x10);
        k.usprintf(0x264, b90, k.str10003(dd0, c4c8));
        k.c6E760(b90); k.c2D850(); return;
    case 2:
        if (nameLen > 0 && nameLen < 0x10) {
            k.win2Unicode(dd0, name, 0x10);
            ustrcpy(b90, dd0);
            ustrcat(b90, k.str10003(nullptr, nullptr));
            ustrcpy(c4c8, d90);
            ustrcat(b90, c4c8);
            k.c2D850(); return;
        }
        if (textLen < 0x132) { ustrcpy(b90, d90); k.c2D850(); return; }
        return;
    case 1:
        if (p[3] == 0 || p[3] == 1) { k.c2D850(); return; }
        if (nameLen > 0 && nameLen < 0x10) {
            k.win2Unicode(db0, name, 0x10);
            ustrcpy(b90, db0);
            k.c6F5F0(4);
            ustrcpy(c4c8, k.str10003(nullptr, nullptr));
            ustrcat(c4c8, d90);
            k.c6F5F0(0);
            ustrcat(b90, c4c8);
            k.c2D850(); return;
        }
        if (textLen < 0x132) { ustrcpy(b90, d90); k.c2D850(); return; }
        return;
    case 5:
        if (unit) k.c2B470(self, text);
        return;
    case 7:
        k.mbToWide(text, w6c8, 0x100);
        (void)k.ord10009();   // the retail result only gates a no-op scan to the terminator
        k.c47A70(self, w6c8, p[8]);
        return;
    }
}

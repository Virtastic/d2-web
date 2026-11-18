// 1.13c D2Client+0x70140: chat edit-line frame (IME mode label, input frame, wrapped typed text, caret).
#include <cstdint>
#include "d2client_callee.h"
struct D2c70140G {
    const uint8_t* imeComp; const uint8_t* ime; uint32_t* frame; const uint32_t* imeMode; const uint32_t* editOpen;
    const uint32_t* scrW; const uint32_t* scrH; const uint32_t* dd5d0;
    uint32_t *t3498, *t349c, *c434, *fad94; const uint32_t* fada8; const uint32_t* modal[9]; const uint16_t* line;
};
extern "C" void d2c_70140_svc(const D2cSvc& v, const D2c70140G& g) {
    auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const uint32_t W = *g.scrW, H = *g.scrH;
    const bool imeOn = *g.ime != 0, comp = *g.imeComp != 0;
    const uint32_t old = d2c_call(v, 0xd3a8, 0, 0xd, 0, 0, 0);
    if (imeOn) {
        uint16_t label[40] = {};
        bool draw = true;
        switch (*g.imeMode) {
        case 0: { static const char s[] = "Alphanumeric"; for (int i = 0; s[i]; ++i) label[i] = (uint8_t)s[i]; break; }
        case 1: label[0] = 0xd55c; label[1] = 0xae00; break;
        case 2: case 3: label[0] = 0x65e5; label[1] = 0x672c; label[2] = 0x8a9e; break;
        case 4: case 5: label[1] = 0x6f22; break;
        default: draw = false;
        }
        if (draw) {
            d2c_call(v, 0xd37e, 0, P(label), 0x14, 0, 0, H - 0x80, 0, 0);
            if (*g.imeMode >= 4) d2c_call(v, 0x6f260, 0, 0, 0, 0, 0);
        }
    }
    ++*g.frame;
    d2c_call(v, 0xd0ba, 0, 0, 0, 0, 0, 0x7f, H - 0x67, W - 0x80, H - 0x38, 0, 1);
    if (*g.editOpen) {
        const uint32_t saved = *g.fad94;
        d2c_call(v, 0xbf8c, 0, 0, 0, 0, 0);
        if (*g.dd5d0 != 0x26) return;   // retail shows the fatal-error box and exits
        *g.fad94 = 0;
        if (saved) {
            d2c_call(v, 0x6f4a0, 0, 0, 0, 0, 0);
            bool allZero = true;
            for (int i = 0; i < 9; ++i) if (*g.modal[i]) { allZero = false; break; }
            if (allZero) {
                *g.c434 = 1;
                if (*g.t349c == 0) {
                    *g.t349c = 1;
                    if (*g.t3498) {
                        d2c_call(v, 0x3cc60, 2, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0), 0xf51ec);
                        *g.t3498 = 0;
                    }
                    d2c_call(v, 0x3ccc0, 4, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0), 0xf51bc);
                }
            }
            if (*g.fada8) d2c_call(v, 0x3ccc0, 5, 0, 0, 0, 0, d2c_call(v, 0xd0b4, 0, 0, 0, 0, 0), 0xd0780);
        }
    }
    int row = 0;
    uint32_t width = 0;
    uint32_t n = 0; while (g.line[n]) ++n;
    if (n > 0) {
        uint16_t copy[257] = {};
        for (uint32_t i = 0; i < 0x100 && g.line[i]; ++i) copy[i] = g.line[i];
        uint32_t out[2] = {};
        const uint32_t list = d2c_call(v, 0xd378, 0, P(copy), P(out), 0, 0, W - 0x109);
        uint32_t it = list;
        int cnt = 0;
        uint32_t y = 0;
        while (it) {
            const uint32_t text = *(const uint32_t*)(uintptr_t)it;
            if (y < 0x2d) {
                d2c_call(v, 0xd37e, 0, text, 0x83, 0, 0, H - 0x58 + y, 0, 0);
                width = d2c_call(v, 0xd35a, 0, text, 0, 0, 0);
            }
            it = *(const uint32_t*)(uintptr_t)(it + 4);
            if (!it && imeOn && comp) d2c_call(v, 0x6f3e0, d2c_call(v, 0xd35a, 0, text, 0, 0, 0) + 0x83, 0, 0, 0, 0, H - 0x58 + y);   // the caret column is measured again (width keeps the loop's value)
            ++cnt; y += 0xf;
        }
        d2c_call(v, 0xd3f6, 0, list, 0, 0, 0);
        if (cnt > 0) row = cnt - 1 > 2 ? 2 : cnt - 1;
    } else if (imeOn && comp) d2c_call(v, 0x6f3e0, 0x83, 0, 0, 0, 0, H - 0x58);
    if (!(imeOn && comp)) d2c_call(v, 0x6ef20, 0, H - 0x58 + row * 0xf, width, 0, 0, 0);
    d2c_call(v, 0xd3a8, 0, old, 0, 0, 0);
}

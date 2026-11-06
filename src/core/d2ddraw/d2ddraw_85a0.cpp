// src/core/d2ddraw/d2ddraw_85a0.cpp -- 1.13c D2DDraw.dll display open: set the cooperative level and the 8-bit display mode,
// create the primary surface (flipping with two, then one back buffer, then single buffered), fetch the back buffer, create
// the system-memory render surface and attach the palette. Decompile-derived, checked against the disassembly (the surface
// descriptions are only visible there). The module globals arrive as pointers; the COM methods, the error report (0x7690),
// Fog's log and the fatal path are seams.
#include <cstdint>

struct D2DDrawOpenDisplayEnv {
    uint32_t* opened;            // DAT 0x10204
    uint32_t* active;            // DAT 0x10208
    uint32_t* width;             // DAT 0x101C8 (640 or 800)
    uint32_t* width2;            // DAT 0x101EC (the same)
    uint32_t* height;            // DAT 0x101D8 (480 or 600)
    uint32_t* modeH;             // DAT 0x101CC := table 0xEB80[mode]
    uint32_t* modeW;             // DAT 0x101D4 := table 0xEB70[mode]
    uint32_t* zero;              // DAT 0x101E8 := 0
    uint32_t* backCount;         // DAT 0x10234: the back buffers of the primary that was created
    const uint32_t* tableW;      // 0xEB70
    const uint32_t* tableH;      // 0xEB80
    const uint32_t* dd;          // DAT 0x10210: the DirectDraw object
    uint32_t* primary;           // DAT 0x10218
    uint32_t* back;              // DAT 0x1021C
    uint32_t* render;            // DAT 0x10220
    const uint32_t* palette;     // DAT 0x10230
    const uint32_t* hwnd;        // DAT 0xFDC4
    uint32_t msgCoop, msgMode, msgSingle, msgAttach, msgDesc, msgRender, msgPalette, msgOpen, fatalFmt;   // module strings
    uint32_t (*setCoop)(uint32_t dd, uint32_t hwnd, uint32_t flags);                           // IDirectDraw +0x50
    uint32_t (*setMode)(uint32_t dd, uint32_t w, uint32_t h, uint32_t bpp);                    // IDirectDraw +0x54
    uint32_t (*createSurface)(uint32_t dd, uint32_t* desc, uint32_t* out, uint32_t outer);     // IDirectDraw +0x18
    uint32_t (*getAttached)(uint32_t surf, uint32_t* caps, uint32_t* out);                     // IDirectDrawSurface +0x30
    uint32_t (*getDesc)(uint32_t surf, uint32_t* desc);                                        // IDirectDrawSurface +0x58
    uint32_t (*setPalette)(uint32_t surf, uint32_t pal);                                       // IDirectDrawSurface +0x7C
    void (*report)(uint32_t hr, uint32_t msg);   // D2DDraw 0x7690 (ECX = hr)
    void (*fogLog)(uint32_t msg);                // Fog 10029
    uint32_t (*langString)(uint32_t id);         // D2Lang lookup
    void (*fatal)(uint32_t fmt, uint32_t text);  // Fog 10024
    void (*exitProcess)(int32_t code);           // D2DDraw 0x1279
};

// 1.13c D2DDraw+0x85A0 (EAX = mode; returns 1 or 0). Already open is fatal (string 0x13F). Mode 0 is 640x480, any other 800x600;
// the two mode words come from the 0xEB70/0xEB80 tables. SetCooperativeLevel(hwnd, 0x11): any result but 0 or 0x88760245 is
// reported and returns 0. No DirectDraw object is fatal (string 0x1D7). SetDisplayMode(w, h, 8). The primary is tried with
// caps 0x4218 and 2 then 1 back buffers (flags 0x21), then single buffered (flags 1, caps 0x4200); with back buffers the back
// buffer is fetched (caps 4). The primary's description is read back and reused, flags 7 and caps 0x840, for the render
// surface; then the palette. A failure past the cooperative level is reported, logged "Opening DirectDraw window failed!" and
// returns 0. Success marks 0x10204 open, clears 0x10208, returns 1.
extern "C" uint32_t d2ddraw_open_display_85a0(const D2DDrawOpenDisplayEnv* e, uint32_t mode) {
    if (*e->opened) {
        e->fatal(e->fatalFmt, e->langString(0x13F));
        e->exitProcess(-1);
        return 0;
    }
    uint32_t w = mode == 0 ? 0x280 : 0x320;
    *e->height = mode == 0 ? 0x1E0 : 0x258;
    *e->modeH = e->tableH[mode];
    *e->width = w;
    *e->width2 = w;
    *e->modeW = e->tableW[mode];
    *e->zero = 0;
    uint32_t hr = e->setCoop(*e->dd, *e->hwnd, 0x11);
    if (hr && hr != 0x88760245u) { e->report(hr, e->msgCoop); return 0; }
    if (!*e->dd) {
        e->fatal(e->fatalFmt, e->langString(0x1D7));
        e->exitProcess(-1);
        return 0;
    }
    uint32_t msg = 0;
    uint32_t desc[27];
    hr = e->setMode(*e->dd, *e->modeW, *e->modeH, 8);
    if (hr) { msg = e->msgMode; goto fail; }
    {
        static const uint32_t tries[3][3] = { { 0x21, 2, 0x4218 }, { 0x21, 1, 0x4218 }, { 1, 0, 0x4200 } };
        int t = 0;
        for (;; ++t) {
            for (uint32_t& d : desc) d = 0;
            desc[0] = 0x6C; desc[1] = tries[t][0]; desc[5] = tries[t][1]; desc[26] = tries[t][2];
            *e->backCount = tries[t][1];
            hr = e->createSurface(*e->dd, desc, e->primary, 0);
            if (!hr) break;
            if (t == 2) { msg = e->msgSingle; goto fail; }
        }
    }
    if (*e->backCount) {
        uint32_t caps = 4;
        hr = e->getAttached(*e->primary, &caps, e->back);
        if (hr) { msg = e->msgAttach; goto fail; }
    }
    for (uint32_t& d : desc) d = 0;
    desc[0] = 0x6C;
    hr = e->getDesc(*e->primary, desc);
    if (hr) { msg = e->msgDesc; goto fail; }
    desc[1] = 7; desc[26] = 0x840;
    hr = e->createSurface(*e->dd, desc, e->render, 0);
    if (hr) { msg = e->msgRender; goto fail; }
    hr = e->setPalette(*e->primary, *e->palette);
    if (hr) { msg = e->msgPalette; goto fail; }
    *e->opened = 1;
    *e->active = 0;
    return 1;
fail:
    e->report(hr, msg);
    e->fogLog(e->msgOpen);
    return 0;
}

// src/core/d2ddraw/d2ddraw_8490.cpp -- 1.13c D2DDraw.dll cutscene display open: switch the DirectDraw object to 640x480 at
// the deepest colour depth the mode mask allows and create the primary surface. The DirectDraw methods, the error report and
// the Fog calls are seams, so the function runs against fixed answers.
#include <cstdint>

struct D2DDrawCutsceneOpenEnv {
    uint32_t* opened;           // DAT 0x10204: set on success; already set is fatal
    uint32_t* active;           // DAT 0x10208: set on success
    uint32_t* width1;           // DAT 0x101C8
    uint32_t* width2;           // DAT 0x101D4
    uint32_t* height1;          // DAT 0x101D8
    uint32_t* height2;          // DAT 0x101CC
    const uint32_t* bitMasks;   // Fog's gdwBitMasks (the data import at 0xB054)
    uint32_t modeMask;          // DAT 0xFCB0
    uint32_t dd;                // DAT 0x10210: the DirectDraw object
    uint32_t* surface;          // DAT 0x10218: receives the primary surface
    uint32_t msgMode, msgSurface, msgFail;   // module strings 0xCDA0, 0xCC9C, 0xCC6C
    uint32_t fatalFmt;                       // module string 0xCE62
    uint32_t (*setDisplayMode)(uint32_t dd, uint32_t w, uint32_t h, uint32_t bpp);                 // vtable +0x54
    uint32_t (*createSurface)(uint32_t dd, uint32_t* desc, uint32_t* out, uint32_t outer);        // vtable +0x18
    void (*report)(uint32_t hr, uint32_t msg);                                                     // D2DDraw 0x7690 (ECX = hr)
    void (*fogLog)(uint32_t msg);                                                                  // Fog 10029
    uint32_t (*langString)(uint32_t id);                                                           // D2Lang 0x267 lookup
    void (*fatal)(uint32_t fmt, uint32_t text);                                                    // Fog 10024
    void (*exitProcess)(int32_t code);                                                             // D2DDraw 0x1279 (_exit)
};

// 1.13c D2DDraw+0x8490 (no arguments; 1 on success, 0 on a DirectDraw failure).
extern "C" uint32_t d2ddraw_cutscene_open_8490(const D2DDrawCutsceneOpenEnv* e) {
    if (*e->opened) {
        e->fatal(e->fatalFmt, e->langString(0x267));
        e->exitProcess(-1);
        return 0;
    }
    *e->width1 = 640; *e->width2 = 640; *e->height1 = 480; *e->height2 = 480;
    uint32_t bpp = 16;
    if (e->bitMasks[1] & e->modeMask) bpp = 32;
    else if (e->bitMasks[2] & e->modeMask) bpp = 24;
    uint32_t hr = e->setDisplayMode(e->dd, 640, 480, bpp), msg = e->msgMode;
    if (hr == 0) {
        uint32_t desc[27] = {};
        desc[0] = 0x6C;
        desc[1] = 1;              // DDSD_CAPS
        desc[26] = 0x4200;        // ddsCaps
        hr = e->createSurface(e->dd, desc, e->surface, 0);
        if (hr == 0) { *e->opened = 1; *e->active = 1; return 1; }
        msg = e->msgSurface;
    }
    e->report(hr, msg);
    e->fogLog(e->msgFail);
    return 0;
}

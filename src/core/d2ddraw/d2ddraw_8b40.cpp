// src/core/d2ddraw/d2ddraw_8b40.cpp -- 1.13c D2DDraw.dll cutscene teardown: clear the render surface and present it three
// times, release the render, back and primary surfaces, call the display-restore hook and mark the cutscene display closed.
// Decompile-derived, checked against the disassembly. The module globals arrive as pointers; the COM methods, the present
// (0x82F0), the error report (0x7690) and the restore hook (*DAT 0x101DC) are seams.
#include <cstdint>

struct D2DDrawCutsceneTeardownEnv {
    uint32_t* opened;          // DAT 0x10204
    uint32_t* active;          // DAT 0x10208
    uint32_t* render;          // DAT 0x10220
    uint32_t* back;            // DAT 0x1021C
    uint32_t* primary;         // DAT 0x10218
    const uint32_t* width;     // DAT 0x101C8
    const uint32_t* height;    // DAT 0x101D8
    uint8_t* const* screen;    // DAT 0xF8A8, the software frame buffer
    uint32_t msgBlt;           // module string 0xCE64 "Blit (screen clear) failed!"
    uint32_t (*blt)(uint32_t surf, uint32_t dst, uint32_t src, uint32_t srcRect, uint32_t flags, uint32_t* fx);  // vtable +0x14
    uint32_t (*release)(uint32_t obj);                                                                          // vtable +0x08
    uint32_t (*present)();                                                                                      // D2DDraw 0x82F0
    void (*report)(uint32_t hr, uint32_t msg);                                                                  // D2DDraw 0x7690 (ECX = hr)
    void (*restore)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1);                                     // *DAT 0x101DC
};

// 1.13c D2DDraw+0x8B40 (no arguments, no result). Nothing unless 0x10204 is set. Three rounds of: a colour-fill Blt of the
// render surface (DDBLT_COLORFILL 0x400, a 100-byte DDBLTFX with every field 0) when there is one; DDERR 0x887601AE clears
// width*height bytes of the frame buffer instead, any other failure is reported with 0xCE64; then the present. Then the render,
// back and primary surfaces are released (each when set) and zeroed, the restore hook is called with (0, 0) in ECX/EDX and two
// zero stack words, and 0x10204/0x10208 are cleared.
extern "C" void d2ddraw_cutscene_teardown_8b40(const D2DDrawCutsceneTeardownEnv* e) {
    if (!*e->opened) return;
    for (int round = 3; round; --round) {
        uint32_t fx[25] = {};
        fx[0] = 100;
        if (*e->render) {
            uint32_t hr = e->blt(*e->render, 0, 0, 0, 0x400, fx);
            if (hr == 0x887601AEu) {
                uint8_t* p = *e->screen;
                for (uint32_t n = *e->height * *e->width; n; --n) *p++ = 0;
            } else if (hr) {
                e->report(hr, e->msgBlt);
            }
        }
        e->present();
    }
    uint32_t* const order[3] = { e->render, e->back, e->primary };
    for (uint32_t* s : order)
        if (*s) { e->release(*s); *s = 0; }
    e->restore(0, 0, 0, 0);
    *e->opened = 0;
    *e->active = 0;
}

// src/core/d2ddraw/d2ddraw_70e0.cpp -- 1.13c D2DDraw.dll cutscene play loop (open the Bink file from the archive, centre it,
// pump window messages and decode frames until an input or the abort word ends it), from the retail disassembly. Every import
// and the frame decode are seams, so the loop runs against fixed answers.
#include <cstdint>

struct D2DDrawWinMsg { uint32_t hwnd, message, wParam, lParam, time, x, y; };

struct D2DDrawBinkPlayEnv {
    uint32_t* bink;             // DAT 0x10244: the open Bink handle (a pointer: +0 width, +4 height)
    const uint32_t* abort;      // DAT 0x10238
    uint32_t surface;           // DAT 0x10218: the primary surface handed to BinkDDSurfaceType
    uint32_t* surfaceType;      // DAT 0x1024C
    uint32_t screenW, screenH;  // DAT 0x101C8 / 0x101D8
    uint32_t* destX;            // DAT 0x101E4 (rounded up to a multiple of 4)
    uint32_t* destY;            // DAT 0x101E0
    uint32_t* quitSeen;         // DAT 0x10250: WM_QUIT seen (the loop then runs out a 500 ms grace)
    uint32_t* quitExit;         // DAT 0x10254: exit the process after the movie
    uint32_t* lastInput;        // DAT 0x10258: tick of the key or click that ended the movie
    uint32_t openDirectSound;   // the value of the BinkOpenDirectSound import slot, handed to BinkSetSoundSystem
    uint32_t (*surfaceTypeOf)(uint32_t surface);                      // _BinkDDSurfaceType@4
    void (*setSoundSystem)(uint32_t open, uint32_t param);            // _BinkSetSoundSystem@8
    uint32_t (*directMode)();                                         // Fog 10117
    void (*openFile)(uint32_t mpq, uint32_t name, uint32_t flags, uint32_t* handle);   // Storm 268
    uint32_t (*binkOpen)(uint32_t handle, uint32_t flags);            // _BinkOpen@8
    void (*closeHandle)(uint32_t handle);                             // kernel32 CloseHandle
    void (*binkClose)(uint32_t bink);                                 // _BinkClose@4
    uint32_t (*tick)();                                               // kernel32 GetTickCount
    uint32_t (*peek)(D2DDrawWinMsg* m, uint32_t hwnd, uint32_t lo, uint32_t hi, uint32_t remove);   // user32 PeekMessageA
    void (*translate)(D2DDrawWinMsg* m);                              // user32 TranslateMessage
    void (*dispatch)(D2DDrawWinMsg* m);                               // user32 DispatchMessageA
    uint32_t (*binkWait)(uint32_t bink);                              // _BinkWait@4
    void (*frame)();                                                  // D2DDraw 0x6FF0
    void (*sleep)(uint32_t ms);                                       // kernel32 Sleep
    void (*soundVolume)(uint32_t v);                                  // D2sound 10061 (fastcall ECX)
    void (*exitProcess)(int32_t code);                                // D2DDraw 0x1279 (_exit; does not return)
};

// 1.13c D2DDraw+0x70E0 (stdcall: name, mode, per-iteration callback; ret 0xC).
extern "C" void d2ddraw_bink_play_70e0(const D2DDrawBinkPlayEnv* e, uint32_t name, uint32_t mode, void (*callback)()) {
    uint32_t handle = 0, stop = 0;
    *e->quitSeen = 0;
    if (*e->bink || *e->abort) return;
    *e->surfaceType = e->surfaceTypeOf(e->surface);
    e->setSoundSystem(e->openDirectSound, 0);
    const uint32_t flags = e->directMode() ? 5 : 4;
    e->openFile(0, name, flags, &handle);
    if (handle) {
        const uint32_t open = mode == 1 ? 0x30800000u : mode == 2 ? 0x10800000u : mode == 3 ? 0x40800000u : 0x800000u;
        *e->bink = e->binkOpen(handle, open);
        if (*e->bink) {
            const uint32_t* b = (const uint32_t*)(uintptr_t)*e->bink;
            uint32_t x = (e->screenW - b[0]) >> 1;
            if (x & 3) x = (x | 3) + 1;
            *e->destX = x;
            *e->destY = (e->screenH - b[1]) >> 1;
            const uint32_t until = e->tick() + 500;
            for (;;) {
                if (*e->quitSeen && e->tick() >= until) break;
                if (*e->abort) break;
                if (callback) callback();
                if (stop) break;
                D2DDrawWinMsg m;
                if (e->peek(&m, 0, 0, 0, 1)) {
                    const uint32_t id = m.message;
                    if (id == 0x12) { *e->quitSeen = 1; *e->quitExit = 1; }
                    else if (id == 0x102 || id == 0x201 || id == 0x204 || id == 0x207) { stop = 1; *e->lastInput = e->tick(); }
                    e->translate(&m);
                    e->dispatch(&m);
                } else if (e->binkWait(*e->bink) == 0) {
                    e->frame();
                } else {
                    e->sleep(1);
                }
            }
            if (*e->bink) {
                e->soundVolume(0);
                e->binkClose(*e->bink);
                e->closeHandle(handle);
                e->soundVolume(0xFF);
                *e->bink = 0;
            }
            if (*e->quitExit) e->exitProcess(-1);
            return;
        }
        e->closeHandle(handle);
    }
    if (*e->bink) { e->binkClose(*e->bink); *e->bink = 0; }
}

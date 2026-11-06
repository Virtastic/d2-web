// src/core/d2ddraw/d2ddraw_6ff0.cpp -- 1.13c D2DDraw.dll cutscene frame copy (decode one Bink frame into the locked primary
// surface), from the retail disassembly: the decompile names EDI as the pitch argument, but retail passes the locked surface
// description's lPitch (+0x10) and lpSurface (+0x24).
#include <cstdint>
#include <cstring>

struct D2DDrawBinkFrameEnv {
    const uint32_t* playing;      // DAT 0xFDC4 (the cutscene window)
    uint32_t bink;                // DAT 0x10244: the Bink handle (+4 height, +8 frame count, +0xC current frame)
    uint32_t lock;                // DAT 0x1023C: the critical section
    uint32_t surface;             // DAT 0x10218: the primary surface
    int32_t destX, destY;         // DATs 0x101E4, 0x101E0
    uint32_t copyFlags;           // DAT 0x1024C
    uint32_t* done;               // DAT 0x10250: set when the last frame was shown
    void (*doFrame)(uint32_t bink);
    void (*nextFrame)(uint32_t bink);
    int32_t (*copyToBuffer)(uint32_t bink, uint32_t dest, uint32_t pitch, uint32_t height, int32_t x, int32_t y, uint32_t flags);
    void (*enter)(uint32_t cs);
    void (*leave)(uint32_t cs);
    uint32_t (*lockSurface)(uint32_t surf, const void* rect, uint32_t* desc, uint32_t flags, uint32_t event);   // vtable +0x64
    uint32_t (*unlockSurface)(uint32_t surf, const void* rect);                                                // vtable +0x80
    void (*fatal)(uint32_t stringId);   // the missing-lock path: error box with D2Lang string 0x42, then _exit(-1)
};

// 1.13c D2DDraw+0x6FF0 (no arguments). Nothing while no cutscene is playing. Otherwise BinkDoFrame, then under the lock
// (a missing one is fatal) the primary surface is locked with a zeroed 0x6C-byte description (dwSize 0x6C, flags 0x21:
// wait | write-only); on success BinkCopyToBuffer(bink, lpSurface, lPitch, bink height, x, y, flags) and Unlock(null).
// After the lock is left, the last frame (current == count) sets the done flag; any other frame advances with BinkNextFrame.
extern "C" void d2ddraw_bink_frame_6ff0(const D2DDrawBinkFrameEnv* e) {
    if (!*e->playing) return;
    e->doFrame(e->bink);
    if (!e->lock) { e->fatal(0x42); return; }
    e->enter(e->lock);
    uint32_t desc[27];
    memset(desc, 0, sizeof desc);
    desc[0] = 0x6C;
    if (e->lockSurface(e->surface, nullptr, desc, 0x21, 0) == 0) {
        uint32_t height; memcpy(&height, (const uint8_t*)(uintptr_t)e->bink + 4, 4);
        e->copyToBuffer(e->bink, desc[9], desc[4], height, e->destX, e->destY, e->copyFlags);
        e->unlockSurface(e->surface, nullptr);
    }
    e->leave(e->lock);
    uint32_t count, cur;
    memcpy(&count, (const uint8_t*)(uintptr_t)e->bink + 8, 4);
    memcpy(&cur, (const uint8_t*)(uintptr_t)e->bink + 0xC, 4);
    if (cur == count) { *e->done = 1; return; }
    e->nextFrame(e->bink);
}

// src/core/fog/fog_sub_20010.cpp -- Fog+0x20010, async-file read kick-off (custom convention: EDI = file object).
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

struct Fog20010Env {
    void* ctx;
    void (*cs)(void*, void* cs, bool enter);
    void (*e550)(void*, uint32_t id);
    void (*e420)(void*, void* file);
    void (*fatal)(void*, int line);
    uint32_t (*read)(void*, void* file, void* buf, uint32_t room);
    uint32_t (*lastError)(void*);
};
namespace {
// Fog async file object, offsets used by this row (retail dword offsets). Buffer capacity is 0x5B4 bytes at +0xB0.
struct FogAsyncFile {
    uint32_t id;         // +0x00 connection id (the error latch's argument)
    uint8_t b04[0x04];
    int32_t pending;     // +0x08 outstanding-read count (InterlockedIncrement)
    uint8_t b0c[0x04];
    int32_t hFile;       // +0x10 file handle, -1 = closed
    uint8_t b14[0x18];
    uint8_t cs[0x18];    // +0x2C CRITICAL_SECTION
    uint8_t overlapped[0x10]; // +0x44 OVERLAPPED (its hEvent word is +0x54)
    int32_t result;      // +0x54 must be 0 here
    uint32_t flags;      // +0x58 bit 2 = read in flight
    uint8_t b5c[0x4C];
    uint32_t filled;     // +0xA8 bytes already in buffer
    uint32_t nRead;      // +0xAC lpNumberOfBytesRead
    uint8_t buf[0x5B4];  // +0xB0
};
[[noreturn]] void seam_fatal(void*, int line) { std::fprintf(stderr, "Unrecoverable internal error (line %d)\n", line); std::exit(-1); }
void seam_cs(void*, void*, bool) {}          // 0x11690 validate then Enter/LeaveCriticalSection
void seam_e550(void*, uint32_t) {}           // 0xE550 error latch, EAX = *file
void seam_e420(void*, void*) {}              // 0xE420, EAX = file
uint32_t seam_read(void*, void*, void*, uint32_t) { return 1; }  // ReadFile BOOL
uint32_t seam_lastError(void*) { return 0; }
Fog20010Env gDefault = { nullptr, seam_cs, seam_e550, seam_e420, seam_fatal, seam_read, seam_lastError };
const Fog20010Env* gEnv = &gDefault;
}  // namespace

// Probe seam: nullptr restores the defaults.
extern "C" void fog_sub_20010_seams(const Fog20010Env* e) { gEnv = e ? e : &gDefault; }

// 1.13c Fog+0x20010 (EDI = file object). Returns 0 when the buffer is full (0x5B4 filled) or on failure, else the ReadFile BOOL.
int fog_sub_20010(void* self) {
    auto* f = static_cast<FogAsyncFile*>(self);
    const Fog20010Env* e = gEnv;
    e->cs(e->ctx, f->cs, true);
    uint32_t filled = f->filled;
    uint32_t room = 0x5B4 - filled;
    if (room == 0) { e->e550(e->ctx, f->id); e->cs(e->ctx, f->cs, false); return 0; }
    if (f->flags & 4) e->fatal(e->ctx, 0x3ee);
    f->flags |= 4;
    if (f->result != 0) e->fatal(e->ctx, 0x3f1);
    if (f->hFile == -1) {
        uint32_t id = f->id;
        e->cs(e->ctx, f->cs, false); e->e550(e->ctx, id); e->e420(e->ctx, f);
        return 0;
    }
    __atomic_add_fetch(&f->pending, 1, __ATOMIC_SEQ_CST);
    uint32_t r = e->read(e->ctx, f, f->buf + filled, room);
    if (r == 0) {
        if (e->lastError(e->ctx) != 0x3E5) { e->e420(e->ctx, f); e->e550(e->ctx, f->id); return 0; }  // retail leaves the cs held on this path
        r = 1;
    }
    e->cs(e->ctx, f->cs, false);
    return (int)r;
}

// src/core/fog/fog_sub_20010.cpp -- Fog+0x20010, async-file read kick-off (custom convention: EDI = file object).
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace {
// Fog async file object, offsets used by this row. Buffer capacity is 0x5B4 bytes at +0xB0.
struct FogAsyncFile {
    uint8_t b00[0x08];
    int32_t pending;     // +0x08 outstanding-read count (InterlockedIncrement)
    uint8_t b0c[0x04];
    int32_t hFile;       // +0x10 file handle, -1 = closed
    uint8_t b14[0x18];
    uint8_t cs[0x18];    // +0x2C CRITICAL_SECTION
    uint8_t overlapped[0x14]; // +0x44
    int32_t result;      // +0x54 must be 0 here
    uint32_t flags;      // +0x58 bit 2 = read in flight
    uint8_t b5c[0x4C];
    uint32_t filled;     // +0xA8 bytes already in buffer
    uint32_t nRead;      // +0xAC lpNumberOfBytesRead
    uint8_t buf[0x5B4];  // +0xB0
};
// Host seams for Win32 / separate Fog rows (+0x11690 cs validate, +0xE550/+0xE420 error latch,
// +0xFC20/+0x10F30/+0x184A0 fatal report). ReadFile returns 1 ok, 0 fail, 2 = ERROR_IO_PENDING.
void seam_cs(void*, bool) {}
void seam_latch(FogAsyncFile*) {}
[[noreturn]] void seam_fatal(int line) { std::fprintf(stderr, "Unrecoverable internal error (line %d)\n", line); std::exit(-1); }
int seam_read(FogAsyncFile*, void*, uint32_t) { return 1; }
}  // namespace

// 1.13c Fog+0x20010
// Returns 0 when the buffer is full (0x5B4 filled) or on failure, else 1.
int fog_sub_20010(void* self) {
    auto* f = static_cast<FogAsyncFile*>(self);
    seam_cs(f->cs, true);
    uint32_t filled = static_cast<uint32_t>(f->filled);
    uint32_t room = 0x5B4 - filled;
    if (room == 0) { seam_latch(f); seam_cs(f->cs, false); return 0; }
    if (f->flags & 4) seam_fatal(0x3ee);
    f->flags |= 4;
    if (f->result != 0) seam_fatal(0x3f1);
    if (f->hFile == -1) { seam_cs(f->cs, false); seam_latch(f); return 0; }
    f->pending++;
    int r = seam_read(f, f->buf + filled, room);
    if (r == 0) { seam_latch(f); return 0; }  // retail leaves the cs held on this path
    seam_cs(f->cs, false);
    return 1;  // r == 2 (ERROR_IO_PENDING 0x3E5) is treated as success
}

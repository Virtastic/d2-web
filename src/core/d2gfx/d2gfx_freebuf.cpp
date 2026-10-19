// Prior art: D2MOO's D2Gfx module supplies renderer functions, not this VS2003 CRT helper.
// Cross-check: D2Client+0x0617B has the same buffer-release rule; this implementation follows
// D2Gfx's cached 1.13c decompile, including its low-word flag update.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GfxFreebufFile {
    uint32_t ptr;
    int32_t cnt;
    uint32_t base;
    uint32_t flag;
    int32_t file;
    int32_t charbuf;
    int32_t bufsiz;
    uint32_t tmpfname;
};
static_assert(sizeof(D2GfxFreebufFile) == 0x20);
static_assert(offsetof(D2GfxFreebufFile, ptr) == 0x00);
static_assert(offsetof(D2GfxFreebufFile, cnt) == 0x04);
static_assert(offsetof(D2GfxFreebufFile, base) == 0x08);
static_assert(offsetof(D2GfxFreebufFile, flag) == 0x0C);

// 1.13c D2Gfx+0x075F8
// __freebuf: release only an active stream's owned buffer. The free callback is the
// D2Gfx+0x02BD4 allocator seam and runs before the FILE is changed. Retail pointers remain 32-bit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_75f8(
    D2GfxFreebufFile* stream, void (*free_)(void*)) {
    if ((stream->flag & 0x83u) != 0 && (stream->flag & 8u) != 0) {
        free_(reinterpret_cast<void*>(static_cast<uintptr_t>(stream->base)));
        // memcpy preserves the 16-bit store without a strict-aliasing violation.
        const uint16_t flags = static_cast<uint16_t>(stream->flag) & 0xFBF7u;
        std::memcpy(&stream->flag, &flags, sizeof(flags));
        stream->ptr = 0;
        stream->base = 0;
        stream->cnt = 0;
    }
}

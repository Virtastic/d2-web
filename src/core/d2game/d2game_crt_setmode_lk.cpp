// MSVC 2003 CRT descriptor mode update. The caller owns the CRT descriptor
// table and errno; descriptor records retain their retail 0x24-byte layout.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct D2GameModeIo {
    uint32_t handle;
    uint8_t flags;
    uint8_t rest[31];
};
static_assert(sizeof(D2GameModeIo) == 0x24);
static_assert(offsetof(D2GameModeIo, flags) == 4);

struct D2GameModeEnv {
    D2GameModeIo** blocks;
    int32_t* error;
};

// 1.13c D2Game+0x0A131
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_a131(
    uint32_t descriptor, int32_t mode, const D2GameModeEnv* env) {
    uint8_t& flags = env->blocks[descriptor >> 5][descriptor & 31].flags;
    const int32_t previous = (flags & 0x80) ? 0x4000 : 0x8000;
    if (mode == 0x8000) flags &= static_cast<uint8_t>(~0x80);
    else if (mode == 0x4000) flags |= 0x80;
    else {
        *env->error = 0x16;
        return -1;
    }
    return previous;
}

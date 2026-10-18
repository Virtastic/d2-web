// Prior art: the 1.13c D2Client cached decompile and the sibling CRT wrapper
// src/core/d2client/crt_read_8383.cpp. D2MOO has no D2Client module.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_fdcount;
extern uint32_t g_pioinfo[0x20];
extern "C" int32_t* d2_client_579c();
extern "C" int32_t* d2_client_57a5();

struct D2ClientSeekIoInfo {
    uint32_t os_handle;
    uint8_t flags;
    uint8_t lookahead;
    uint8_t rest[0x1e];
};
static_assert(sizeof(D2ClientSeekIoInfo) == 0x24);
static_assert(offsetof(D2ClientSeekIoInfo, flags) == 4);

struct D2ClientSeekEnv {
    void (*lock_fhandle)(int32_t);                  // 0x0A67B
    int32_t (*lseek_lk)(int32_t, int32_t, int32_t); // 0x08678
    void (*unlock_fhandle)();                      // 0x08773 finally thunk
};

static int32_t d2_client_seek_bad_fd() {
    *d2_client_579c() = 9;
    *d2_client_57a5() = 0;
    return -1;
}

// 1.13c D2Client+0x086EC
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_86ec(
    int32_t fd, int32_t offset, int32_t origin, const D2ClientSeekEnv* env) {
    const uint32_t handle = static_cast<uint32_t>(fd);
    if (handle >= g_fdcount) return d2_client_seek_bad_fd();
    const uint32_t index = handle & 31u;
    auto flags = [&]() -> uint8_t {
        auto* block = reinterpret_cast<D2ClientSeekIoInfo*>(
            static_cast<uintptr_t>(g_pioinfo[handle >> 5]));
        return block[index].flags;
    };
    if ((flags() & 1u) == 0) return d2_client_seek_bad_fd();
    env->lock_fhandle(fd);
    struct Unlock {
        const D2ClientSeekEnv* env;
        ~Unlock() { env->unlock_fhandle(); }
    } unlock{env};
    if ((flags() & 1u) == 0) return d2_client_seek_bad_fd();
    return env->lseek_lk(fd, offset, origin);
}

#include <cstddef>
#include <cstdint>

struct D2GfxCommitIo {
    uint32_t handle;
    uint8_t flags;
    uint8_t remainder[0x1f];
};
static_assert(offsetof(D2GfxCommitIo, handle) == 0);
static_assert(offsetof(D2GfxCommitIo, flags) == 4);
static_assert(sizeof(D2GfxCommitIo) == 0x24);

// Native callbacks model the CRT descriptor table, lock and kernel32 seams.
struct D2GfxCommitEnv {
    uint32_t count;
    D2GfxCommitIo* const* blocks;
    void (*lock)(uint32_t);
    void (*unlock)();
    uint32_t (*get_osfhandle)(uint32_t);
    int32_t (*flush_file_buffers)(uint32_t);
    uint32_t (*get_last_error)();
    int32_t* error;
    uint32_t* dos_error;
};

// 1.13c D2Gfx+0x07043
extern "C" uint32_t d2gfx_commit(uint32_t fd, const D2GfxCommitEnv* env) {
    if (fd >= env->count) {
        *env->error = 9;
        return UINT32_MAX;
    }
    auto* record = &env->blocks[fd >> 5][fd & 31];
    if (!(record->flags & 1)) {
        *env->error = 9;
        return UINT32_MAX;
    }
    env->lock(fd);
    uint32_t result = UINT32_MAX;
    // The CRT descriptor can change while its lock is being acquired; retail
    // re-reads the block for the protected open-bit check.
    record = &env->blocks[fd >> 5][fd & 31];
    if (record->flags & 1) {
        const uint32_t handle = env->get_osfhandle(fd);
        const uint32_t error = env->flush_file_buffers(handle) ? 0 : env->get_last_error();
        if (error == 0) result = 0;
        else *env->dos_error = error;
    }
    if (result != 0) *env->error = 9;
    env->unlock();
    return result;
}

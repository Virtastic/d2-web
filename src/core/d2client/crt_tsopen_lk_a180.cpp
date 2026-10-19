#include <cstdint>
#include <emscripten.h>

struct D2ClientTsopenLkEnv {
    uint32_t (*alloc_osfhnd)();
    void (*set_osfhnd)(uint32_t fd, uint32_t handle);
    uint32_t (*create_file)(const char* path, uint32_t access, uint32_t share,
                            const void* security, uint32_t creation, uint32_t attributes,
                            uint32_t template_handle);
    uint32_t (*get_file_type)(uint32_t handle);
    void (*close_handle)(uint32_t handle);
    uint32_t (*get_last_error)();
    void (*dosmaperr)(uint32_t error);
    uint32_t (*lseek_lk)(uint32_t fd, int32_t offset, uint32_t origin);
    int32_t (*read_nolock)(uint32_t fd, void* dst, uint32_t count);
    int32_t (*close_lk)(uint32_t fd);
    uint32_t (*open_mode)(void);       // DAT_6fbaac7c
    uint32_t (*textmode)(void);
    uint32_t (*share_default)(void);   // DAT_6fbaa8a4
};

extern "C" int32_t* d2_client_579c();
extern "C" int32_t* d2_client_57a5();
extern "C" uint8_t* d2_client_ioinfo_record(int32_t);

// 1.13c D2Client+0x0A180 (__tsopen_lk, 743 B, caller 0xA467): translate the
// MSVC open flags, allocate a descriptor, create the OS file, and initialize
// its ioinfo record. Kernel calls and still-unmapped CRT leaves are explicit
// seams; the control flow and flag transformations follow the cached body.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a180(
    void* share, uint32_t* out_created, uint32_t* out_fd, const char* filename,
    uint32_t flags, uint8_t mode, const D2ClientTsopenLkEnv* env) {
    const auto invalid = []() -> uint32_t {
        *d2_client_579c() = 0x16;
        *d2_client_57a5() = 0;
        return UINT32_MAX;
    };
    (void)share;
    uint8_t dev_flags = (int8_t)flags < 0 ? 0x10 : 0;
    const bool inheritable = (int8_t)flags >= 0;
    if ((flags & 0x8000u) == 0 && ((flags & 0x4000u) != 0 || env->open_mode() != 0x8000u))
        dev_flags |= 0x80;

    uint32_t access;
    switch (flags & 3u) {
    case 0: access = 0x80000000u; break;
    case 1: access = 0x40000000u; break;
    case 2: access = 0xC0000000u; break;
    default: return invalid();
    }
    uint32_t share_mode;
    switch ((uintptr_t)share) {
    case 0x10: share_mode = 0; break;
    case 0x20: share_mode = 1; break;
    case 0x30: share_mode = 2; break;
    case 0x40: share_mode = 3; break;
    default: return invalid();
    }
    uint32_t creation;
    switch (flags & 0x700u) {
    case 0x000: case 0x400: creation = 3; break;
    case 0x100: creation = 4; break;
    case 0x200: case 0x600: creation = 5; break;
    case 0x300: case 0x500: case 0x700: creation = 1; break;
    default: return invalid();
    }
    uint32_t attributes = 0x80;
    if ((flags & 0x100u) != 0 && (int8_t)(~(uint8_t)env->textmode() & mode) >= 0)
        attributes = 1;
    if ((flags & 0x40u) != 0) {
        access = (access & 0xFF000000u) | 0x10000u;
        attributes |= 0x04000000u;
        if (env->share_default() == 2) share_mode |= 4;
    }
    if ((flags & 0x1000u) != 0) attributes |= 0x100u;
    if ((flags & 0x20u) != 0) attributes |= 0x08000000u;
    else if ((flags & 0x10u) != 0) attributes |= 0x10000000u;

    const uint32_t fd = env->alloc_osfhnd();
    if (fd == UINT32_MAX) {
        *d2_client_579c() = 0x18;
        *d2_client_57a5() = 0;
        return UINT32_MAX;
    }
    *out_created = 1;
    *out_fd = fd;
    struct SecurityAttributes { uint32_t length; uint32_t descriptor; int32_t inherit; } security = {
        0xC, 0, inheritable ? 1 : 0
    };
    const uint32_t handle = env->create_file(filename, access, share_mode, &security,
                                              creation, attributes, 0);
    if (handle == UINT32_MAX) {
        env->dosmaperr(env->get_last_error());
        return UINT32_MAX;
    }
    const uint32_t type = env->get_file_type(handle);
    if (type == 0) {
        env->close_handle(handle);
        env->dosmaperr(env->get_last_error());
        return UINT32_MAX;
    }
    if (type == 2) dev_flags |= 0x40;
    else if (type == 3) dev_flags |= 8;
    env->set_osfhnd(fd, handle);
    uint8_t* record = d2_client_ioinfo_record((int32_t)fd);
    const uint8_t initialized_flags = dev_flags | 1;
    record[4] = initialized_flags;
    if ((dev_flags & 0x48) == 0 && (int8_t)initialized_flags < 0 && (flags & 2u) != 0) {
        const uint32_t end = env->lseek_lk(fd, -1, 2);
        if (end == UINT32_MAX) {
            if (*d2_client_579c() != 0x83) goto append_done;
        } else {
            char last = 0;
            const int32_t got = env->read_nolock(fd, &last, 1);
            if (got != 0 || last != 0x1A ||
                env->lseek_lk(fd, (int32_t)end, 0) != UINT32_MAX)
                goto append_done;
        }
        env->close_lk(fd);
        return UINT32_MAX;
    }
append_done:
    if ((dev_flags & 0x48) != 0 || (flags & 8u) == 0) return fd;
    record[4] |= 0x20;
    return fd;

}

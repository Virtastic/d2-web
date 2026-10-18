// MSVC 2003 CRT file-open policy; platform calls and module state are caller-owned,
// as in d2game_crt_free_osfhnd.cpp. OS handles stay explicit retail 32-bit tokens.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2GameOpenSecurity {
    uint32_t length, descriptor, inherit;
};
static_assert(sizeof(D2GameOpenSecurity) == 12);
static_assert(offsetof(D2GameOpenSecurity, inherit) == 8);
struct D2GameOpenIo {
    uint32_t handle;
    uint8_t flags;
    uint8_t rest[31];
};
static_assert(sizeof(D2GameOpenIo) == 0x24);
static_assert(offsetof(D2GameOpenIo, flags) == 4);
struct D2GameTsopenEnv {
    int32_t* error;
    uint32_t* dosError;
    uint32_t defaultMode, umaskValue, osVersion;
    D2GameOpenIo** blocks;
    uint32_t (*allocate)();
    uint32_t (*create)(const char*, uint32_t, uint32_t, const D2GameOpenSecurity*,
                       uint32_t, uint32_t, uint32_t);
    uint32_t (*fileType)(uint32_t);
    void (*setHandle)(uint32_t, uint32_t);
    uint32_t (*seek)(uint32_t, int32_t, uint32_t);
    int32_t (*read)(uint32_t, char*, uint32_t);
    int32_t (*resize)(uint32_t, uint32_t);
    void (*closeFd)(uint32_t);
    void (*closeHandle)(uint32_t);
    uint32_t (*lastError)();
    void (*mapError)(uint32_t);
};

// 1.13c D2Game+0x083E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_83E0(
    uint32_t share, uint32_t* allocated, uint32_t* descriptor,
    const char* filename, uint32_t flags, uint8_t mode, const D2GameTsopenEnv* e) {
    const auto fail = [e](int32_t error) { *e->error = error; *e->dosError = 0; return UINT32_MAX; };
    D2GameOpenSecurity security{12, 0, (flags & 0x80) == 0 ? 1u : 0u};
    uint8_t ioFlags = security.inherit ? 0 : 0x10;
    if (!(flags & 0x8000) && ((flags & 0x4000) || e->defaultMode != 0x8000)) ioFlags |= 0x80;
    uint32_t access;
    switch (flags & 3) {
    case 0: access = 0x80000000; break;
    case 1: access = 0x40000000; break;
    case 2: access = 0xc0000000; break;
    default: return fail(0x16);
    }
    uint32_t sharing;
    switch (share) {
    case 0x10: sharing = 0; break;
    case 0x20: sharing = 1; break;
    case 0x30: sharing = 2; break;
    case 0x40: sharing = 3; break;
    default: return fail(0x16);
    }
    uint32_t disposition;
    switch (flags & 0x700) {
    case 0: case 0x400: disposition = 3; break;
    case 0x100: disposition = 4; break;
    case 0x200: case 0x600: disposition = 5; break;
    case 0x300: disposition = 2; break;
    case 0x500: case 0x700: disposition = 1; break;
    default: return fail(0x16);
    }
    uint32_t attributes = 0x80;
    if ((flags & 0x100) && !(static_cast<uint8_t>(~e->umaskValue & mode) & 0x80)) attributes = 1;
    if (flags & 0x40) {
        access = (access & 0xff000000) | 0x10000;
        attributes |= 0x4000000;
        if (e->osVersion == 2) sharing |= 4;
    }
    if (flags & 0x1000) attributes |= 0x100;
    if (flags & 0x20) attributes |= 0x8000000;
    else if (flags & 0x10) attributes |= 0x10000000;
    const uint32_t fd = e->allocate();
    if (fd == UINT32_MAX) return fail(0x18);
    *allocated = 1;
    *descriptor = fd;
    const uint32_t handle = e->create(filename, access, sharing, &security, disposition, attributes, 0);
    if (handle != UINT32_MAX) {
        const uint32_t type = e->fileType(handle);
        if (type != 0) {
            if (type == 2) ioFlags |= 0x40;
            else if (type == 3) ioFlags |= 8;
            e->setHandle(fd, handle);
            auto& io = e->blocks[fd >> 5][fd & 31];
            io.flags = ioFlags | 1;
            const bool special = (ioFlags & 0x48) != 0;
            if (!special && (io.flags & 0x80) && (flags & 2)) {
                const uint32_t end = e->seek(fd, -1, 2);
                bool okay;
                if (end == UINT32_MAX) okay = *e->dosError == 0x83;
                else {
                    char last = 0;
                    const int32_t count = e->read(fd, &last, 1);
                    // Preserve the cached retail branch literally: nonzero read result
                    // bypasses resize, even when the last byte is Ctrl-Z.
                    okay = (count != 0 || last != 0x1a || e->resize(fd, end) != -1)
                        && e->seek(fd, 0, 0) != UINT32_MAX;
                }
                if (!okay) { e->closeFd(fd); return UINT32_MAX; }
            }
            if (!special && (flags & 8)) io.flags |= 0x20;
            return fd;
        }
        e->closeHandle(handle);
    }
    e->mapError(e->lastError());
    return UINT32_MAX;
}

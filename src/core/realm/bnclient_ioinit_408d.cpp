// BNClient's VC7.1 stdio table setup. Prior art: the byte-identical CRT body in
// src/core/realm/mcp_crt_batch_b56.cpp (D2MCPClient+0x01CFC), and the same
// `__ioinit` algorithm in src/core/d2gfx/d2gfx_ioinit_2d1c.cpp. This copy keeps
// BNClient module state explicit for deterministic function-mode capture.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct BnIoRecord {
    int32_t handle;
    uint8_t flags;
    uint8_t token;
    uint16_t padding;
    int32_t lock_initialized;
    uint8_t critical_section[24];
};
static_assert(sizeof(BnIoRecord) == 0x24);

struct BnIoStartupInfo {
    uint16_t reserved_size;
    const uint8_t* reserved;
};
struct BnIoInitEnv {
    void* (*allocate)(uint32_t);
    void (*startup_info)(BnIoStartupInfo*);
    uint32_t (*file_type)(int32_t);
    int32_t (*standard_handle)(int32_t);
    int32_t (*initialize_lock)(void*, uint32_t);
    uint32_t (*set_handle_count)(uint32_t);
    uint32_t count;                 // 1.13c DAT_6FF40BE8
    BnIoRecord* blocks[64];         // 1.13c DAT_6FF40C00
};

static uint32_t bn_io_word(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

static BnIoRecord* bn_io_allocate(BnIoInitEnv* e) {
    auto* block = static_cast<BnIoRecord*>(e->allocate(0x480));
    if (block == nullptr) return nullptr;
    for (uint32_t i = 0; i != 32; ++i) {
        block[i].handle = -1;
        block[i].flags = 0;
        block[i].token = 10;
        block[i].lock_initialized = 0;
    }
    return block;
}

// 1.13c BNClient+0x0408D (510 B, caller 0x01FDD __CRT_INIT): initialise the
// first 32-entry ioinfo block, adopt valid inherited handles from STARTUPINFOA,
// seed stdin/stdout/stderr when absent, and publish the handle count. Retail
// returns -1 on allocation or critical-section failure and 0 on success.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_bnclient_ioinit(BnIoInitEnv* e) {
    auto* first = bn_io_allocate(e);
    if (first == nullptr) return -1;
    e->count = 0x20;
    e->blocks[0] = first;

    BnIoStartupInfo startup{};
    e->startup_info(&startup);
    if (startup.reserved_size != 0 && startup.reserved != nullptr) {
        const uint32_t original_count = bn_io_word(startup.reserved);
        const uint8_t* flags = startup.reserved + 4;
        const uint8_t* handles = flags + original_count;
        uint32_t requested = static_cast<int32_t>(original_count) > 0x7ff ? 0x800 : original_count;
        uint32_t inherited = requested;
        for (uint32_t slot = 1; static_cast<int32_t>(e->count) < static_cast<int32_t>(requested); ++slot) {
            auto* block = bn_io_allocate(e);
            if (block == nullptr) { inherited = e->count; break; }
            e->count += 0x20;
            e->blocks[slot] = block;
        }
        for (uint32_t i = 0; static_cast<int32_t>(i) < static_cast<int32_t>(inherited); ++i) {
            const int32_t handle = static_cast<int32_t>(bn_io_word(handles + i * 4));
            const uint8_t mode = flags[i];
            if (handle == -1 || (mode & 1) == 0 || ((mode & 8) == 0 && e->file_type(handle) == 0)) continue;
            BnIoRecord& rec = e->blocks[i >> 5][i & 0x1f];
            rec.handle = handle;
            rec.flags = mode;
            if (e->initialize_lock(rec.critical_section, 4000) == 0) return -1;
            ++rec.lock_initialized;
        }
    }

    for (uint32_t i = 0; i != 3; ++i) {
        BnIoRecord& rec = first[i];
        if (rec.handle == -1) {
            rec.flags = 0x81;
            const int32_t handle = e->standard_handle(-10 - static_cast<int32_t>(i));
            const uint32_t type = handle == -1 ? 0 : e->file_type(handle);
            if (handle == -1 || type == 0) {
                rec.flags |= 0x40;
            } else {
                rec.handle = handle;
                if ((type & 0xff) == 2) rec.flags |= 0x40;
                else if ((type & 0xff) == 3) rec.flags |= 8;
                if (e->initialize_lock(rec.critical_section, 4000) == 0) return -1;
                ++rec.lock_initialized;
            }
        } else {
            rec.flags |= 0x80;
        }
    }
    e->set_handle_count(e->count);
    return 0;
}

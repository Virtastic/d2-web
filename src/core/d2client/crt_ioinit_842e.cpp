// Prior art: src/core/realm/mcp_crt_batch_b56.cpp (__ioinit), cross-checked against the
// cached 510-byte D2Client 1.13c body. D2MOO has no D2Client module.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

struct D2ClientIoRecord {
    int32_t handle;
    uint8_t flags, lookahead;
    uint16_t padding;
    uint32_t lock_initialized;
    uint8_t critical_section[24];
};
static_assert(sizeof(D2ClientIoRecord) == 0x24);
static_assert(offsetof(D2ClientIoRecord, flags) == 4);
static_assert(offsetof(D2ClientIoRecord, lookahead) == 5);
static_assert(offsetof(D2ClientIoRecord, lock_initialized) == 8);
static_assert(offsetof(D2ClientIoRecord, critical_section) == 12);

// Native pointers describe the host seam; inherited handles remain serialized x86 words.
struct D2ClientIoStartup { uint16_t reserved_size; const uint8_t* reserved; };
struct D2ClientIoInitEnv {
    void* (*allocate)(uint32_t);                       // D2Client+0x908F malloc
    void (*startup_info)(D2ClientIoStartup*);          // GetStartupInfoA reserved fields
    uint32_t (*file_type)(int32_t);
    int32_t (*standard_handle)(int32_t);
    int32_t (*initialize_lock)(void*, uint32_t);       // D2Client+0x7D5F
    uint32_t (*set_handle_count)(uint32_t);
    uint32_t count;                                   // global +0x123990
    D2ClientIoRecord* blocks[64];                     // global +0x1239A0
};

static D2ClientIoRecord* client_io_allocate(D2ClientIoInitEnv* e) {
    auto* block = static_cast<D2ClientIoRecord*>(e->allocate(0x480));
    if (block) {
        for (uint32_t i = 0; i != 32; ++i) {
            block[i].handle = -1;
            block[i].flags = 0;
            block[i].lookahead = 10;
            block[i].lock_initialized = 0;
        }
    }
    return block;
}
static uint32_t client_io_word(const uint8_t* p) {
    uint32_t value;
    std::memcpy(&value, p, sizeof(value));
    return value;
}

// 1.13c D2Client+0x0842e
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_ioinit(D2ClientIoInitEnv* e) {
    auto* first = client_io_allocate(e);
    if (!first) return -1;
    e->count = 32;
    e->blocks[0] = first;
    D2ClientIoStartup startup{};
    e->startup_info(&startup);
    if (startup.reserved_size && startup.reserved) {
        uint32_t requested = client_io_word(startup.reserved);
        const uint8_t* flags = startup.reserved + 4;
        const uint8_t* handles = flags + requested; // offset uses the original count
        if (static_cast<int32_t>(requested) > 2047) requested = 2048;
        uint32_t inherited = requested;
        for (uint32_t slot = 1; static_cast<int32_t>(e->count) < static_cast<int32_t>(requested); ++slot) {
            auto* block = client_io_allocate(e);
            if (!block) { inherited = e->count; break; }
            e->count += 32;
            e->blocks[slot] = block;
        }
        for (int32_t i = 0; i < static_cast<int32_t>(inherited); ++i) {
            const int32_t handle = static_cast<int32_t>(client_io_word(handles + uint32_t(i) * 4));
            const uint8_t mode = flags[i];
            if (handle == -1 || !(mode & 1)) continue;
            if (!(mode & 8) && e->file_type(handle) == 0) continue;
            auto& rec = e->blocks[uint32_t(i) >> 5][i & 31];
            rec.handle = handle;
            rec.flags = mode;
            if (!e->initialize_lock(rec.critical_section, 4000)) return -1;
            ++rec.lock_initialized;
        }
    }
    for (uint32_t i = 0; i != 3; ++i) {
        auto& rec = first[i];
        if (rec.handle != -1) { rec.flags |= 0x80; continue; }
        rec.flags = 0x81;
        const int32_t handle = e->standard_handle(-10 - static_cast<int32_t>(i));
        const uint32_t type = handle == -1 ? 0 : e->file_type(handle);
        if (!type) { rec.flags |= 0x40; continue; }
        rec.handle = handle;
        if ((type & 255) == 2) rec.flags |= 0x40;
        else if ((type & 255) == 3) rec.flags |= 8;
        if (!e->initialize_lock(rec.critical_section, 4000)) return -1;
        ++rec.lock_initialized;
    }
    e->set_handle_count(e->count);
    return 0;
}

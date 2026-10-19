#include "d2game_ioinit.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace {
void initializeBlock(d2game_crt::Descriptor* block) {
    for (uint32_t i = 0; i < 32; ++i) {
        block[i].handle = UINT32_MAX;
        block[i].flags = 0;
        block[i].pipeCharacter = 10;
        block[i].lockInitialized = 0;
    }
}
}

// 1.13c D2Game+0x060EF
// Host calls replace Win32 imports; state represents this module's __pioinfo
// and _nhandle globals. No OS handle is widened to a native pointer.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_60ef(
    d2game_crt::State* state, const d2game_crt::Startup* startup,
    const d2game_crt::Host* host) {
    auto* first = host->allocateBlock(host->context);
    if (!first) return -1;
    state->capacity = 32;
    state->blocks[0] = first;
    initializeBlock(first);
    if (startup->present) {
        int32_t count = startup->count;
        if (count > 2047) count = 2048;
        uint32_t blockIndex = 1;
        while (state->capacity < count) {
            auto* block = host->allocateBlock(host->context);
            if (!block) {
                count = state->capacity;
                break;
            }
            state->capacity += 32;
            state->blocks[blockIndex++] = block;
            initializeBlock(block);
        }
        for (int32_t i = 0; i < count; ++i) {
            uint32_t handle = startup->handles[i];
            uint8_t flags = startup->flags[i];
            if (handle == UINT32_MAX || !(flags & 1)) continue;
            if (!(flags & 8) && !host->fileType(host->context, handle)) continue;
            auto& descriptor = state->blocks[uint32_t(i) >> 5][i & 31];
            descriptor.handle = handle;
            descriptor.flags = flags;
            if (!host->initializeLock(host->context, descriptor.criticalSection, 4000)) return -1;
            ++descriptor.lockInitialized;
        }
    }
    for (uint32_t i = 0; i < 3; ++i) {
        auto& descriptor = first[i];
        if (descriptor.handle != UINT32_MAX) {
            descriptor.flags |= 0x80;
            continue;
        }
        descriptor.flags = 0x81;
        uint32_t handle = host->standardHandle(host->context, uint32_t(-10) - i);
        uint32_t type = handle == UINT32_MAX ? 0 : host->fileType(host->context, handle);
        if (!type) {
            descriptor.flags |= 0x40;
            continue;
        }
        descriptor.handle = handle;
        if ((type & 0xff) == 2) descriptor.flags |= 0x40;
        else if ((type & 0xff) == 3) descriptor.flags |= 8;
        if (!host->initializeLock(host->context, descriptor.criticalSection, 4000)) return -1;
        ++descriptor.lockInitialized;
    }
    host->setHandleCount(host->context, uint32_t(state->capacity));
    return 0;
}

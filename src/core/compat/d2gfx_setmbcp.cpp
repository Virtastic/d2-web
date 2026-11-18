#include <cstdint>
#include <cstring>
#include <emscripten.h>

// D2Gfx's MSVC CRT state touched by __setmbcp at 1.13c RVA 0x5720.
struct D2GfxSetmbcpState {
    uint32_t pseudo_codepage; // DAT_6fa910dc
    uint32_t codepage;        // DAT_6fa9d7c4
    uint32_t max_char_size;   // DAT_6fa9d6b4
    uint32_t locale;          // DAT_6fa9d6ac
    uint16_t cp_extra[5];     // DAT_6fa9d7d0
    uint8_t classes[257];     // DAT_6fa9d6c0
    uint8_t lower[256];       // DAT_6fa9d7e0
    uint32_t* saved_block;    // DAT_6fa9d6b0
};

struct D2GfxSetmbcpEnv {
    void (*lock)(uint32_t);
    void (*unlock)(uint32_t);
    uint32_t (*get_oem_cp)();
    uint32_t (*get_acp)();
    uint32_t oem_cp;
    void* (*allocate)(uint32_t);
    void (*release)(void*);
    int32_t (*select_codepage)(uint32_t, uint32_t);
};

// 1.13c D2Gfx+0x05720 (__setmbcp). CRT wrapper: resolve -2/-3/-4 through the
// corresponding Windows codepage source, avoid reallocating an active snapshot,
// ask the sibling selector at +0x558C to rebuild multibyte tables, and preserve
// the prior CRT fields in the 0x220-byte block when selection succeeds.
// 1.13c D2Gfx+0x05720
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_gfx_setmbcp_5720(
    int32_t codepage, D2GfxSetmbcpState* state, const D2GfxSetmbcpEnv* env) {
    int32_t result = -1;
    uint32_t* memory = nullptr;

    env->lock(0xD);
    state->pseudo_codepage = 0;
    if (codepage == -2) {
        state->pseudo_codepage = 1;
        codepage = static_cast<int32_t>(env->get_oem_cp());
    } else if (codepage == -3) {
        state->pseudo_codepage = 1;
        codepage = static_cast<int32_t>(env->get_acp());
    } else if (codepage == -4) {
        state->pseudo_codepage = 1;
        codepage = static_cast<int32_t>(env->oem_cp);
    }

    if (static_cast<uint32_t>(codepage) == state->codepage) {
        result = 0;
    } else {
        if (state->saved_block == nullptr || state->saved_block[0] != 0)
            memory = static_cast<uint32_t*>(env->allocate(0x220));
        else
            memory = state->saved_block;

        if (memory != nullptr &&
            (result = env->select_codepage(static_cast<uint32_t>(codepage), state->pseudo_codepage)) == 0) {
            auto* bytes = reinterpret_cast<uint8_t*>(memory);
            memory[0] = 0;
            memory[1] = state->codepage;
            memory[2] = state->max_char_size;
            memory[3] = state->locale;
            std::memcpy(bytes + 0x10, state->cp_extra, sizeof state->cp_extra);
            std::memcpy(bytes + 0x1C, state->classes, sizeof state->classes);
            state->saved_block = memory;
            std::memcpy(bytes + 0x11D, state->lower, sizeof state->lower);
        }
        if (result == -1 && memory != state->saved_block) env->release(memory);
    }
    env->unlock(0xD);
    return result;
}

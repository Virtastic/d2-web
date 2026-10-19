#include "mcp_setmbcp_338e.h"
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x0338E (__setmbcp). The lower selector and OS code-page
// queries are explicit seams; the saved block is the caller-owned CRT snapshot.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_setmbcp_338e(
    int32_t codepage, D2McpSetmbcpState* state, const D2McpSetmbcpEnv* env) {
    int32_t result = -1;
    uint32_t* memory = nullptr;

    env->lock(0xD);
    uint32_t pseudo = 0;
    if (codepage == -2) {
        pseudo = 1;
        codepage = static_cast<int32_t>(env->get_oem_cp());
    } else if (codepage == -3) {
        pseudo = 1;
        codepage = static_cast<int32_t>(env->get_acp());
    } else if (codepage == -4) {
        pseudo = 1;
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
            (result = env->select_codepage(static_cast<uint32_t>(codepage), pseudo)) == 0) {
            auto* bytes = reinterpret_cast<uint8_t*>(memory);
            memory[0] = 0;
            memory[1] = state->codepage;
            memory[2] = state->max_char_size;
            memory[3] = state->locale;
            std::memcpy(bytes + 0x10, state->cp_extra, 10);
            std::memcpy(bytes + 0x1C, state->classes, sizeof state->classes);
            state->saved_block = memory;
            std::memcpy(bytes + 0x11D, state->lower, sizeof state->lower);
        }
        if (result == -1 && memory != state->saved_block) env->release(memory);
    }
    env->unlock(0xD);
    return result;
}

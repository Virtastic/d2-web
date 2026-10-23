// D2MCPClient CRT callback-table append. Sizes use retail bytes; stored callbacks are native pointers.
#include "../realm/mcp_onexit.h"
#include <cstdlib>
#include <emscripten.h>

static uint32_t g_onexit_bytes;
static uint32_t onexit_size(void*) { return g_onexit_bytes; }
static void** onexit_realloc(void** block, uint32_t bytes) {
    auto* grown = static_cast<void**>(std::realloc(block, (bytes / 4) * sizeof(void*)));
    if (grown != nullptr) g_onexit_bytes = bytes;
    return grown;
}
D2McpOnexitLkEnv g_mcp_onexit{onexit_size, onexit_realloc, nullptr, nullptr};

// 1.13c D2MCPClient+0x015D4
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_onexit_lk(void* func, D2McpOnexitLkEnv* env) {
    const uint32_t size = env->msize(env->onexit_begin);
    const auto used = env->onexit_begin ? ((char*)env->onexit_end - (char*)env->onexit_begin) / env->slot : 0;
    if (size < static_cast<uint32_t>(used * 4 + 4)) {
        const uint32_t grow = size < 0x800 ? size : 0x800;
        void** block = env->realloc(env->onexit_begin, grow + size);
        if (block == nullptr) {
            block = env->realloc(env->onexit_begin, size + 0x10);
            if (block == nullptr) return nullptr;
        }
        env->onexit_end = (void**)((char*)block + used * env->slot);
        env->onexit_begin = block;
    }
    if (env->slot == 4) *(uint32_t*)env->onexit_end = (uint32_t)(uintptr_t)func;
    else *env->onexit_end = func;
    env->onexit_end = (void**)((char*)env->onexit_end + env->slot);
    return func;
}

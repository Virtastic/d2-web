// Object lookup and handle. Retail: fastcall, ECX = game, EDX = unit; no stack arguments.
// 0xA478 (stdcall, one word) is handed the unit. 0xB168 (fastcall, ECX = unit) returns a unit or 0; its id (or 0xFFFFFFFF) selects a
// bucket of the game hash at +0x1120 (id & 0x7F) and the chain (node +0xC = id, next +0xE4) is walked for that id. The found node
// (or 0) is put into a stack record {game, unit, found, game+0x10F0 dword} and passed to 0x6B570 (stdcall, record and 0).
// 1.13c D2Game+0x6B6F0
#include <cstdint>
#include <emscripten.h>

struct D2game6b6f0Env {
    void (*a478)(uint32_t unit);
    void* (*b168)(uint32_t unit);
    void (*b570)(void* rec, uint32_t zero);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2game_6b6f0(uint8_t* game, uint32_t unit, const D2game6b6f0Env* env) {
    env->a478(unit);
    const uint32_t g10f0 = *(const uint32_t*)(game + 0x10F0);
    const uint8_t* u = (const uint8_t*)env->b168(unit);
    const uint32_t id = u ? *(const uint32_t*)(u + 4) : 0xFFFFFFFFu;
    uint32_t node = *(const uint32_t*)(game + 0x1120 + (id & 0x7F) * 4);
    while (node) {
        const uint8_t* n = (const uint8_t*)(uintptr_t)node;
        if (*(const uint32_t*)(n + 0xC) == id) break;
        node = *(const uint32_t*)(n + 0xE4);
    }
    uint32_t rec[5] = { (uint32_t)(uintptr_t)game, unit, node, g10f0, 0 };
    env->b570(rec, 0);
}

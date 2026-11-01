// src/core/d2game/d2game_48300.cpp -- 1.13c D2Game.dll MONSTERAI_RemoveInteractInfoFor (0x48300), from the decompile and disassembly.
// Register convention: ESI = monster interact block (head of the node list at +0), EDI = unit to remove, one stack word = game (ret 4).
// A node is {unit +0, next +8}; the matching node is unlinked and freed with the Fog pool free (0xA2EC: ECX = pool game+0x1c, EDX = node,
// stack: file 0x6FD1C358, line 0x229, 0).
#include <cstdint>
#include <cstring>

struct D2Game48300Env {
    void (*fogFree)(void* pool, void* node, uint32_t file, uint32_t line, uint32_t zero);
};

namespace {
inline uint32_t rd(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void wr(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
}

// 1.13c D2Game+0x48300
extern "C" void D2Game_48300(void* esi, uint32_t edi, void* game, const D2Game48300Env* e) {
    if (!esi) return;
    const uint32_t head = rd(esi, 0);
    uint32_t node = head, prev = 0;
    while (node) {
        if (rd((void*)(uintptr_t)node, 0) == edi) {
            if (node == head) wr(esi, 0, rd((void*)(uintptr_t)node, 8));
            else wr((void*)(uintptr_t)prev, 8, rd((void*)(uintptr_t)node, 8));
            e->fogFree((void*)(uintptr_t)rd(game, 0x1c), (void*)(uintptr_t)node, 0x6fd1c358u, 0x229, 0);
            return;
        }
        prev = node;
        node = rd((void*)(uintptr_t)node, 8);
    }
}

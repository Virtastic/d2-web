// 1.13c D2Game+0xBD160 (image-layout port): EBX = game, EDI = NPC record, [esp+4] = vendor id, ret 4.
// Once no offered hireling is left unhired, free the hire list, clear the init flag and rebuild it.
#include <cstdint>
#include <cstdlib>
#include <cstring>
struct D2GameBD160Env {
    uint32_t (*getRec)(uint32_t expansion, uint32_t vendor);          // 0xAC40
    void (*freeMem)(uint32_t pool, uint32_t ptr, uint32_t line);      // 0xA2EC (ECX pool, EDX ptr, file, line, 0)
    void (*rebuild)(uint32_t npc, uint32_t game, uint32_t vendor);    // 0xBCED0 (EAX npc, game, vendor)
};
static uint32_t r32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint32_t r16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
extern "C" void d2g_bd160(uint32_t game, uint32_t npc, uint32_t vendor, const D2GameBD160Env* e)
{
    const uint32_t data = r32(npc + 0x10);
    uint32_t left = 0;
    const uint32_t rec = e->getRec(r32(game + 0x70), vendor);
    if (!rec) abort();
    int32_t n = (int32_t)(r16(rec + 0x116) - r16(rec + 0x114)) + 1;
    if (n == -1) abort();
    if (n > 0) {
        for (uint32_t p = data + 8; n; --n, p += 16)
            if (r32(p + 4) != 0 && r32(p) == 0) ++left;
        if (left) return;
    }
    e->freeMem(r32(game + 0x1c), r32(npc + 0x10), 0x638);
    const uint32_t z = 0; memcpy((void*)(uintptr_t)(npc + 0x10), &z, 4);
    *(uint8_t*)(uintptr_t)(npc + 0x21) = 0;
    e->rebuild(npc, game, vendor);
}

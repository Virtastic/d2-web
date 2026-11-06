// 1.13c D2Game+0x6BD60. Retail register layout: ECX = unit whose position is read, ESI = target unit, EDI = holder (owner) unit; plain ret.
// Callees (all recorded by the probe): 0xA544 (stdcall 2: unit, &xy), 0xA694 (stdcall 5: holder, &xy, 3, 0x1C09, 0), 0xE19C0
// (EAX = xy[1], ECX = xy[0], EDX = holder, stdcall 4: game = esi+0x80, esi, 0, 0), 0xA6D0 (stdcall 2: holder, &rec), 0xA4C0 (stdcall 1: esi),
// 0xA556 (stdcall 1: holder; AL used), 0x8A3E0 (EAX = ebx, stdcall 2: &packet, 6), 0xA4DE (stdcall 1: esi).
// A null ESI is the fatal path (log, exit(-1)); the probe never takes it.
#include <cstdint>
#include <emscripten.h>

struct D2game6bd60Env {
    void (*a544)(uint32_t unit, uint32_t* xy);
    uint32_t (*a694)(uint32_t holder, uint32_t* xy, uint32_t three, uint32_t code, uint32_t zero);
    uint32_t (*d19c0)(uint32_t y, uint32_t x, uint32_t holder, uint32_t game, uint32_t unit, uint32_t z0, uint32_t z1);
    void (*a6d0)(uint32_t holder, uint32_t* rec);
    uint32_t (*a4c0)(uint32_t unit);
    uint32_t (*a556)(uint32_t holder);
    void (*aa3e0)(uint32_t ebx, const uint8_t* packet, uint32_t len);
    void (*a4de)(uint32_t unit);
    void (*fatal)();
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2game_6bd60(uint32_t ecx, uint8_t* esi, uint32_t edi, const D2game6bd60Env* env) {
    uint32_t xy[2] = { 0, 0 };
    env->a544(ecx, xy);
    if (!env->a694(edi, xy, 3, 0x1C09, 0)) return;
    if (!esi) { env->fatal(); return; }
    const uint32_t unit = (uint32_t)(uintptr_t)esi;
    if (!env->d19c0(xy[1], xy[0], edi, *(uint32_t*)(esi + 0x80), unit, 0, 0)) return;
    uint32_t rec[6] = { 0, 0, 0, 0, 0, 0 };
    env->a6d0(edi, rec);
    uint32_t ebx = 0;
    if (*(uint32_t*)esi == 0) ebx = *(uint32_t*)(uintptr_t)(env->a4c0(unit) + 0x9C);
    const uint16_t cx = (uint16_t)rec[4], dx = (uint16_t)rec[5];
    uint8_t pkt[8] = { 7, (uint8_t)cx, (uint8_t)(cx >> 8), (uint8_t)dx, (uint8_t)(dx >> 8), 0, 0, 0 };
    pkt[5] = (uint8_t)env->a556(edi);
    env->aa3e0(ebx, pkt, 6);
    env->a4de(unit);
    *(uint32_t*)(esi + 0xC8) |= 0x10000;
}

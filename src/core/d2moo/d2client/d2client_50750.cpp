// 1.13c D2Client+0x50750 (174 B, stack arg pMsg, ret 4): pMsg+0xC = x word (kept in EBX). Returns at once when Ordinal 10031
// (thunk 0xD0AE) is 2 and DAT 0xFADA4 is set. Otherwise EBP = DAT 0x11C414; a second 0xD0AE == 2 selects 0x4FF10() else
// 0x4FF60(EAX = EBP, ECX = x). Zero: pMsg+0x18 := 0 and return. Nonzero: DAT 0x11C308 := 1, 0x88A70(EBX = 4, four zero words),
// 0x16360(ESI = pMsg); stop if EBP == 2 && 0x46150() != 0 && DAT 0x119663 (dword) == 1; else pMsg+0x18 := 1, pMsg+0x1C := 0 and
// Ordinal 511 (thunk 0xBE84, stack {pMsg}).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct D2c50750Env {
    uint32_t (*cd0ae)();
    uint32_t (*c4ff10)();
    uint32_t (*c4ff60)(uint32_t eax, uint32_t ecx);
    void (*c88a70)(uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void (*c16360)(const uint8_t* esi);
    uint32_t (*c46150)();
    void (*cbe84)(const uint8_t* msg);
};
extern "C" void d2c_port_50750(uint8_t* msg, const uint32_t* gada4, const uint32_t* gc414, uint32_t* gc308, const uint32_t* g9663, const D2c50750Env* e) {
    uint16_t xw; std::memcpy(&xw, msg + 0xC, 2);
    uint32_t ebx = xw;
    if (e->cd0ae() == 2 && *gada4 != 0) return;
    uint32_t ebp = *gc414;
    uint32_t r = e->cd0ae() == 2 ? e->c4ff10() : e->c4ff60(ebp, ebx);
    uint32_t zero = 0, one = 1;
    if (r == 0) { std::memcpy(msg + 0x18, &zero, 4); return; }
    *gc308 = 1;
    e->c88a70(4, 0, 0, 0, 0);
    e->c16360(msg);
    if (ebp == 2 && e->c46150() != 0 && *g9663 == 1) return;
    std::memcpy(msg + 0x18, &one, 4);
    std::memcpy(msg + 0x1C, &zero, 4);
    e->cbe84(msg);
}

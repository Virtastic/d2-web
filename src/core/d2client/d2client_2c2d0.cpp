// 1.13c D2Client+0x2C2D0
// Damage/modifier percent scaler. Registers (disassembly 0x6FADC2D0): EAX = optional source unit (kept in EBX), ESI = the unit; stack p1..p4, ret 0x10.
// p4 gates the whole function (and is the third argument of 0xC922); p1 receives the scaled value; p2 receives the 0/3/1 flag; p3 is forwarded to 0xCEAA. Every callee is a thunk with the
// stack arguments listed (stdcall), the decompile's missing arguments restored from the disassembly.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2c2c2d0Env {
    uint32_t (*cB640)();                                  // Ordinal_10858: a pointer to the mode word (read only, as int16)
    uint32_t (*cBEC8)(uint32_t esi);                       // Ordinal_10621: base value (stdcall, the unit)
    uint32_t (*cBA0C)(uint32_t unit60);                   // Ordinal_10061
    uint32_t (*cB220)(uint32_t ebx);                      // Ordinal_11088 (== 0x26 clears)
    uint32_t (*cB1FC)(uint32_t esi, uint32_t id);         // Ordinal_10494 (esi, 0x4E)
    uint32_t (*cB922)(uint32_t esi, uint32_t ebx, uint32_t p4, uint32_t zero);   // Ordinal_10804
    uint32_t (*cB454)(uint32_t esi, uint32_t id, uint32_t zero);                 // Ordinal_10910 (esi, 0x77, 0)
    uint32_t (*cBEAA)(uint32_t esi, uint32_t mode, uint32_t p3);                 // Ordinal_10653 (esi, (int16)*mode, p3)
    uint32_t (*cBEB0)(uint32_t esi);                      // Ordinal_10402
    uint32_t (*cBEBC)(uint32_t esi);                      // Ordinal_11038
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2c2d0(uint32_t eax, uint8_t* esi, int32_t* p1, int32_t* p2, uint32_t p3, uint32_t p4,
                                                         const D2c2c2d0Env* k) {
    if (p4 == 0) return 0;                                           // 0x6FADC2DE
    uint8_t* desc = (uint8_t*)(uintptr_t)k->cB640();
    if (desc == nullptr) return 0;
    uint32_t esi32 = (uint32_t)(uintptr_t)esi;
    *p1 = (int32_t)k->cBEC8(esi32);
    uint32_t ebx = eax;
    bool test = true;
    if (ebx == 0) {
        uint32_t u60; memcpy(&u60, esi + 0x60, 4);
        if (u60 == 0) test = false;
        else { ebx = k->cBA0C(u60); if (ebx == 0) test = false; }
    }
    if (test && k->cB220(ebx) == 0x26) { *p1 = 0; *p2 = 0; }
    uint32_t sum = 0;
    if (k->cB1FC(esi32, 0x4E) != 0) sum = k->cB922(esi32, ebx, p4, 0);
    sum += k->cB454(esi32, 0x77, 0);
    int16_t m; memcpy(&m, desc, 2);
    sum += k->cBEAA(esi32, (uint32_t)(int32_t)m, p3);
    int32_t base = *p1;
    int32_t prod = (int32_t)((uint32_t)base * sum);
    *p1 = prod / 100 + base;
    *p2 = 0;
    if (k->cBEB0(esi32) != 0) *p2 = 3;
    if (k->cBEBC(esi32) != 0) *p2 = 1;
    return 1;
}

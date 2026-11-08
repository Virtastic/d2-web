// 1.13c D2Game+0x6AB60 -- spawn effect 0xB from the effect table row, then notify the unit. Fastcall: ECX = effect context (+4 game, +8 unit),
// EDX = record (+0 byte table index, +0xC value). Written from the 1.13c disassembly (the Ghidra view drops the 0x6A960 register and stack arguments).
// 0x6A960(ECX = game, EDX = unit; stack a1 = record+0xC, 0, 0xB, table[idx*3] dword, 0x6A960's callback 0x6A930; ret 0x14); then 0xAEB6(unit);
// a non-null unit whose type dword (+0) is 0 goes to 0x96FA0(stack: unit+0x80, unit). The table row is read from the D2Game table at 0xFA600 (12 bytes a row).
#include "../fn/hookenv.h"
namespace d2g {
void f6ab60(HookEnv& e, uint32_t ecx, uint32_t edx, uint32_t table)
{
    const uint32_t game = e.rd32(ecx + 4), unit = e.rd32(ecx + 8);
    const uint32_t row = e.rd32(table + e.rd8(edx) * 12);
    e.h(0x6A960, 0, game, unit, 0, 0, e.rd32(edx + 0xC), 0, 0xB, row, 0);   // stack word 4 (the callback) is not logged
    e.h(0xAEB6, 0, 0, 0, 0, 0, unit);
    if (unit && e.rd32(unit) == 0) e.h(0x96FA0, 0, 0, 0, 0, 0, e.rd32(unit + 0x80), unit);
}
}

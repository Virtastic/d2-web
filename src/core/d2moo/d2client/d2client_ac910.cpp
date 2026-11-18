// D2Client rva 0xAC910: server command handler (ECX = packet). Looks the unit up by id through sub_A4E20 and, when
// found, forwards the packet's mode/frame words to an import, then clears a module flag. Callees in retail order.
#include <cstdint>
#include <cstring>
#include "d2client_callee.h"

// 1.13c D2Client+0xAC910. pk: +1 type byte, +2 b, +3 id dword, +7 u16 w, +9 c. tab = address of D2Client+0x10A608
// (rows 0x200 bytes indexed by type). flag = the module dword D2Client+0x11BFB8.
extern "C" void d2_client_ac910(const D2cSvc& v, const uint8_t* pk, uint32_t tab, uint32_t* flag) {
    uint32_t id; std::memcpy(&id, pk + 3, 4);
    uint16_t w; std::memcpy(&w, pk + 7, 2);
    const uint32_t n = d2c_call(v, 0xa4e20, id & 0x7f, id, tab + ((uint32_t)pk[1] << 9), 0, 0, pk[1]);
    if (!n) return;
    d2c_call(v, 0xc550, 0, 0, 0, 0, 0, n, w, pk[9], pk[2], 0xB2000000u | 0xd4034, 0xa8c);   // SCmd.cpp line 0xA8C
    *flag = 0;
}

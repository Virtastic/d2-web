// 1.13c D2Client+0x48590 -- EAX = index, stack byte (ret 4) = useCur. Callees (host seam): 46150 (current object, cdecl, no args), 48150, A3D20, 46E00,
// 7D3C0(obj, word) (stdcall, 2 words). Globals: [119661] byte, [119662] byte (read), [119767] dword, [11BF94] dword (set to the address of 46E60), [11BF98] dword,
// [119675] points at a record {+5 table ptr, +9 count, +0xD current, +0x11 byte}; the table is 15-byte entries whose first word is handed to 7D3C0.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_hcall.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_48590(D2cHost h, int32_t eax, uint8_t useCur, uint8_t* g119661, uint8_t g119662, uint32_t* g119767,
                                               uint32_t* g11bf94, uint32_t* g11bf98, uint8_t* rec, uint32_t fn46e60) {
    if (!d2cCall(h, 0x46150)) { d2cCall(h, 0x48150); return; }
    *g119661 = 1; *g11bf94 = fn46e60; *g11bf98 = 0;
    d2cCall(h, 0xA3D20);
    *g119767 = 0; rec[0x11] = 1;
    if (g119662 == 0) { d2cCall(h, 0x46E00); if (useCur) std::memcpy(&eax, rec + 0xD, 4); }
    int32_t count; uint32_t tbl; std::memcpy(&count, rec + 9, 4); std::memcpy(&tbl, rec + 5, 4);
    uint16_t w; std::memcpy(&w, (const uint8_t*)(uintptr_t)tbl + (uint32_t)((eax % count) * 15), 2);
    uint32_t obj = d2cCall(h, 0x46150);
    d2cCall(h, 0x7D3C0, obj, w);
}

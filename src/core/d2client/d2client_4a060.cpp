// 1.13c D2Client+0x4A060 -- EAX = menu index, stack dword (ret 4) = insert flag. Gate: BF8C() (D2Common thunk, host seam) non-zero and [119854] non-zero, else 0.
// Per index a 0x27-byte record at F3B90+idx*0x27: count dword at +0x14 (F3BA4), words at +0x18 (F3BA8), handler dwords at +0x22 (F3BB2).
// flag != 0: nothing when handler 46D10 is already listed; else the entry for 49440 (if listed) shifts up and 46D10 / word 0x1507 is inserted at its place,
// otherwise it goes in front of the last entry (the last one shifts up once). flag == 0: the 46D10 entry (first match) is removed by shifting the rest down. Returns 1.
// g points at the data address F3B90 (the reads below can reach one entry before the handler array, as the retail loop does).
// The insert/remove is npc_menu_resurrect.h, which the browser's NPC menu runs too (npc_menu_resurrect.cpp).
#include <cstdint>
#include <emscripten.h>
#include "d2client_hcall.h"
#include "npc_menu_resurrect.h"
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_4a060(D2cHost h, int32_t idx, uint32_t flag, uint8_t* g, uint32_t g119854, uint32_t fn46d10, uint32_t fn49440) {
    if (!d2cCall(h, 0xBF8C) || !g119854) return 0;
    d2client_npcmenu::resurrect(g + (int32_t)(idx * 0x27), flag, fn46d10, fn49440);
    return 1;
}

// Browser export of D2Client+0x4A060's insert: where the Resurrect row lands in an NPC menu. Entries are named by the handler dword the
// retail table holds (Hire's is 0x6FAF9440, Resurrect's 0x6FAF6D10); the insert's rule compares only those two, so any other entry
// may carry any other value. Returns the index the row takes in the new list, or -1 when the row is already listed.
#include <cstdint>
#include <cstring>
#include "npc_menu_resurrect.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_npc_menu_resurrect_at(uint32_t n, uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4) {
    constexpr uint32_t kBase = 0x6FAB0000u, kResurrect = kBase + 0x46D10, kHire = kBase + 0x49440;
    uint8_t rec[0x60] = {};
    const uint32_t h[5] = {h0, h1, h2, h3, h4};
    if (n > 5) return -1;
    d2client_npcmenu::wr4(rec + 0x14, n);
    for (uint32_t i = 0; i < n; ++i) d2client_npcmenu::wr4(rec + 0x22 + 4 * i, h[i]);
    d2client_npcmenu::resurrect(rec, 1, kResurrect, kHire);
    const uint32_t m = d2client_npcmenu::rd4(rec + 0x14);
    if (m == n) return -1;   // already listed: the insert changed nothing
    for (uint32_t i = 0; i < m && i < 6; ++i) if (d2client_npcmenu::rd4(rec + 0x22 + 4 * i) == kResurrect) return (int32_t)i;
    return -1;
}

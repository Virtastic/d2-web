// 1.13c D2Client+0x4BC80, the 0x2A transaction reply handler (reached through 0xAF9B0, ESI = the packet). It copies the
// 15 bytes to DAT 0x11977F..0x11978D and reads D2Common 10973(player, 0xE, 0) (gold) into DAT 0x11966B. With DAT 0x119663
// at 5 (the trade screen) it tail-calls 0x4B5D0 (d2client_4b5d0_refusal.cpp); at 10 (the confirm box) 0x4B420. Otherwise a
// result byte (+2) of 1 plays Sounds row 0xDD (item_gold) through 0x88A70 with EBX = 0xDD and no unit (four zero stack words).
// 0x4B5D0 (trade screen open) plays the same row for result 1 (its switch: case 1 at 0x6FAFB602, EBX = 0xDD), so a result
// of 1 is item_gold outside the confirm box either way.
// The session path never opens the trade screen or the confirm box, so only the sound branch is wired here. The capture log
// takes it at once (sound_pump.cpp); the play path takes the row from d2_client_4bc80_take_sound and plays it.
#include <cstdint>
#include "sound_unit_refs.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

static int32_t g_pendingGold = 0;   // item_gold submits since the play path last took them

extern "C" void d2_client_4bc80_reply(const uint8_t* p, uint32_t n) {
    if (!p || n < 3 || p[0] != 0x2A) return;
    if (p[2] == 1) { d2client_sound_nounit_submit_now(0xDD); if (g_pendingGold < 8) ++g_pendingGold; }
}

// The play path's drain: the Sounds row of one pending no-unit submit, 0 when none.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_4bc80_take_sound(void) {
    if (g_pendingGold <= 0) return 0;
    --g_pendingGold;
    return 0xDD;
}

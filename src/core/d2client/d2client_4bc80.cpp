// 1.13c D2Client+0x4BC80, the 0x2A transaction reply handler (reached through 0xAF9B0, ESI = the packet). It copies the
// 15 bytes to DAT 0x11977F..0x11978D and reads D2Common 10973(player, 0xE, 0) (gold) into DAT 0x11966B. With DAT 0x119663
// at 5 (the trade screen) it tail-calls 0x4B5D0 (d2client_4b5d0_refusal.cpp); at 10 (the confirm box) 0x4B420. Otherwise a
// result byte (+2) of 1 plays Sounds row 0xDD (item_gold) through 0x88A70 with EBX = 0xDD and no unit (four zero stack words).
// The session path never opens the trade screen or the confirm box, so only the sound branch is wired here.
#include <cstdint>
#include "sound_unit_refs.h"

extern "C" void d2_client_4bc80_reply(const uint8_t* p, uint32_t n) {
    if (!p || n < 3 || p[0] != 0x2A) return;
    if (p[2] == 1) d2client_sound_nounit_submit_now(0xDD);
}

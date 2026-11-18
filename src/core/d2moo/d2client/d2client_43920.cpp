// 1.13c D2Client+0x43920 -- net message pump.
// Takes system packets (D2Net #10020) into a 512-byte stack buffer until the list is empty: byte 0 0xAF sets
// DAT_6FBCC39C (+0x11C39C), 0xB0 clears it, 0xB3 goes to +0xACD70 (ESI = buffer), 0xB4 to +0xACBF0 (EAX = buffer).
// Then takes game packets (D2Net #10008) and hands every take, the final -1 included, to +0xACE20 (EAX = buffer,
// the size pushed). Each pass of both loops calls Sleep(0); a single-threaded module has nothing to yield to, so
// the call is left out (it is an import, never a logged callee).
#include "d2client_43920.h"

extern "C" {
int32_t g_d2c_11c39c = 0;

// 1.13c D2Client+0x43920
void d2c_43920(const D2c43920Env* e) {
    uint8_t buf[512];
    uint32_t n;
    do {
        n = e->take_system(buf, 0x200);
        if (n != 0xFFFFFFFFu) {
            switch (buf[0]) {
            case 0xaf: *e->flag = 1; break;
            case 0xb0: *e->flag = 0; break;
            case 0xb3: e->acd70(buf); break;
            case 0xb4: e->acbf0(buf); break;
            default: break;
            }
        }
    } while (n != 0xFFFFFFFFu);
    do {
        n = e->take_game(buf, 0x200);
        e->ace20(buf, static_cast<int32_t>(n));
    } while (n != 0xFFFFFFFFu);
}
}

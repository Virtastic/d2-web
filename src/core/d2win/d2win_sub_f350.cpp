// 1.13c D2Win+0x0F350 -- dispatch a window message to a type-6 control's handler (+0x24) and post-handler (+0x28).
#include <cstdint>

namespace {
struct F350Host {
    const char* (*str)(int id) = nullptr;                    // Ordinal_10265 (string table)
    void (*fatal)(const char* file, const char* msg) = nullptr;  // Ordinal_10024 then _exit(-1)
};
F350Host g_f350;
}
extern "C" { int d2win_f350_depth = 0; }   // DAT_6f9014a8 (D2Win+0x214A8), shared with the callback-depth counter

// msg = param_1: [0]=control ptr, [1]=message id, [2]=target id. Retail 32-bit layout; control words are u32.
// ponytail: handler pointers held as host-width via the callbacks passed in, no retail-memory sharing.
using Handler = uint32_t (*)(uint32_t* msg);
uint32_t d2win_sub_f350(uint32_t* msg, uint32_t* ctl, Handler h, Handler post) {
    if (ctl[0] != 6) { g_f350.fatal("", g_f350.str(0x259)); return 0; }
    if (msg[1] == 0x102 || ctl[0x14] != msg[2] || !h) return 0;
    ctl[0x13] = 1;
    if (d2win_f350_depth < 1) { g_f350.fatal("", g_f350.str(0x263)); return 0; }
    uint32_t r = h(msg);
    if (post) {   // retail hands the post-handler a 32-byte stack buffer whose first word is the control (the rest is residue, zero here)
        uint32_t ev[8] = { msg[0] };
        d2win_f350_depth++; post(ev); d2win_f350_depth--;
    }
    return r;
}

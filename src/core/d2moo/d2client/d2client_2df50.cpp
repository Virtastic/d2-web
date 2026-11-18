// 1.13c D2Client+0x2DF50 (268 B, ret 0xc, EBX = dst; stack sel, lo, hi): frames/25 range with one decimal.
// int = v / 25, frac = ((v % 25) * 10) / 25 (C division, truncating). lo int != 0: a nonzero lo frac or hi frac gives "%d.%d-%d.%d", else "%d-%d".
// lo int == 0: lo frac != 0 gives the four-value form; else hi int != 0 gives (hi frac != 0 ? four-value : "%d-%d"); else returns 0.
// strcat dst str(sel), local, str(0x10AC), str(0xF9E); returns 1.
#include "d2client_ustr_env.h"
#include "d2client_wfmt.h"
extern "C" uint32_t d2c_port_2df50(uint16_t* dst, uint32_t sel, int32_t lo, int32_t hi, const D2ClientUStrEnv& e) {
    using namespace d2client_wfmt;
    int32_t li = lo / 25, lf = ((lo % 25) * 10) / 25, hi_ = hi / 25, hf = ((hi % 25) * 10) / 25;
    bool four;
    if (li == 0) {
        if (lf != 0) four = true;
        else if (hi_ != 0) four = hf != 0;
        else return 0;
    } else four = lf != 0 || hf != 0;
    Buf o;
    putInt(o, li);
    if (four) { put(o, '.'); putInt(o, lf); }
    put(o, '-'); putInt(o, hi_);
    if (four) { put(o, '.'); putInt(o, hf); }
    uint16_t local[64] = {}; widen(o, local);
    e.strcat(dst, e.str(sel));
    e.strcat(dst, local);
    e.strcat(dst, e.str(0x10AC));
    e.strcat(dst, e.str(0xF9E));
    return 1;
}

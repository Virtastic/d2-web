// 1.13c D2Client+0x62AA0 -- table entry with one stack word: remembers the current selection (first dword of the
// record at 0x11C1C4, -1 without one), resolves the word to its record, selects that record's dword at +8, runs the
// three-word routine with (word, 1, the selection's answer) and restores the remembered selection.
#include "cl_tbl.h"
extern "C" uint32_t d2c_62aa0(uint32_t s0, const cl::Env& e) {
    const uint32_t cur = clt::G(e, 0x11C1C4);
    const uint32_t saved = cur ? e.R(cur) : 0xFFFFFFFFu;
    const uint32_t r = e.H(0xc1de, s0);
    const uint32_t rec = e.H(0xcc3a, 0, 0, 0, 0, 0, 0, r);
    const uint32_t a = e.H(0x62710, 0, 0, 0, 0, 0, e.R(rec + 8));
    uint32_t ret = e.H(0x62580, s0, 1, a);
    if (saved != 0xFFFFFFFFu) ret = e.H(0x62710, 0, 0, 0, 0, 0, saved);
    return ret;
}

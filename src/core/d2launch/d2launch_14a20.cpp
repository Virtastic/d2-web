// 1.13c D2Launch+0x14A20 -- build the 4-control screen: free the table, g25E40 = 1, g25E60 = 0; h0 (10196(h0,0), 10001(h0,1)), h1 and h2 (10001 each),
// h3 (not shown), then D2Win#10020(h2, D2Lang#10003(0x147B)) as a tail call.
#include "d2launch_scr.h"
extern "C" void d2l_14a20_core(const D2lScrG* g, const uint32_t t[4], const D2lScr* s) {
    bool ok = true;
    d2lScrFree(g, s); *g->g25e40 = 1; *g->g25e60 = 0;
    D2L_PUSH(h0, t[0]); s->win10196(h0, 0); s->win10001(h0, 1);
    D2L_PUSH(h1, t[1]); s->win10001(h1, 1);
    D2L_PUSH(h2, t[2]); s->win10001(h2, 1);
    D2L_PUSH(h3, t[3]); (void)h3;
    s->win10020(h2, s->lang10003(0x147b));
}

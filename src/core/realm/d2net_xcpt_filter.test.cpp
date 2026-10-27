// Host test: g++ -o /tmp/t src/core/realm/d2net_xcpt_filter.test.cpp && /tmp/t
#include "d2net_crt_n43.cpp"
#include <cassert>
using namespace d2net_n43;
static int sig, sub, calls;
static void act(int s, ...) { sig = s; calls++; }
static void actfpe(int s, int c) { sig = s; sub = c; calls++; }
static int unh(void*) { return 7; }
int main() {
    Heap h; h.xcpt_count = 4; h.unhandled = unh; h.fpe_first = 1; h.fpe_count = 2;
    XcptEntry t[4] = {{0xc0000005, 11, act}, {0xc000008e, 8, (void (*)(int, ...))actfpe},
                      {0xc0000090, 8, (void (*)(int, ...))actfpe}, {0xc0000094, 8, (void (*)(int, ...))5}};
    Ptd p; p.tab = t; int x;
    assert(xcpt_filter(h, p, 0x1234, &x) == 7);                       // unknown -> UnhandledExceptionFilter
    assert(xcpt_filter(h, p, 0xc0000094, &x) == 1 && !t[3].act);     // action 5 cleared
    assert(xcpt_filter(h, p, 0xc0000094, &x) == 7);                   // now null action
    assert(xcpt_filter(h, p, 0xc0000005, &x) == -1 && sig == 11 && !t[0].act && !p.initarg);
    assert(xcpt_filter(h, p, 0xc000008e, &x) == -1 && sig == 8 && sub == 0x83);
    assert(!t[1].act && !t[2].act && p.fpecode == 0);                 // FPE range cleared, fpecode restored
    assert(seh_filter(h, p, 0, &x) == 0);
    return 0;
}

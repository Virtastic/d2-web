// Release the delayed list: for each node, free its attachment (when present) and then the node itself; clear the head.
#include "d2client_light_env.h"
using namespace cllight;
extern "C" void d2c_a82b0(const cl::Env& e) {
    constexpr uint32_t kFile = 0xB2000000u | 0xD4104u;   // the source-file tag the free call is handed
    for (uint32_t n = e.R(kDelay); n;) {
        const uint32_t next = e.R(n + 0x28), att = e.R(n + 0x30);
        if (att) e.H(0xBF86, 0x57D, 0, 0, 0, 0, 0, att, kFile);
        e.H(0xBF86, 0x57F, 0, 0, 0, 0, 0, n, kFile);
        n = next;
    }
    e.W(kDelay, 0);
}

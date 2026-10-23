// 1.13c D2Net+0x137F D2Net+0x14B0 D2Net+0x18E0 D2Net+0x1A8C D2Net+0x1AC7: MSVC CRT stubs linked into D2Net.
// SEH prolog/epilog and the 14B0 fragment are register/frame-bound: no C form.
#include <cstdint>

// 1.13c D2Net+0x137F __initterm: call each non-null function pointer in [begin, end).
void d2_realm_net_initterm(void (**begin)(), void (**end)()) {
    for (; begin < end; ++begin)
        if (*begin) (*begin)();
}

// 1.13c D2Net+0x18E0 __lock: lock slot n (lazily created via __mtinitlocknum, failure -> __amsg_exit(0x11)).
// Single-threaded model: slots are created on demand and held as a depth count.
static int g_lockDepth[48];
int d2_realm_net_lock(int n) { return ++g_lockDepth[n]; }

#include <cstdint>

// 1.13c D2Common+0x019E0 __cinit(int): MSVC CRT init. Calls the float-math hook (if set) with the
// argument, runs the C initializer table (4 slots, stops at first nonzero return and returns it),
// then registers the terminator via atexit and runs the C++ initializer table (2 slots).
// Env seam: tables and atexit are owner-supplied.
struct D2CinitEnv {
    void (*fpmath)(int);          // PTR___fpmath_6fdef298, may be null
    int (**cInit)();              // DAT_6fde900c..901c (4 entries)
    void (**cppInit)();           // DAT_6fde9000..9008 (2 entries)
    void (*atexitTerm)();         // LAB_6fd53060
    void (*atexitFn)(void (*)()); // _atexit
};

int d2common_cinit_19e0(D2CinitEnv& e, int arg)
{
    if (e.fpmath) e.fpmath(arg);
    for (int i = 0; i < 4; i++) {
        if (e.cInit[i]) {
            int r = e.cInit[i]();
            if (r != 0) return r;
        }
    }
    e.atexitFn(e.atexitTerm);
    for (int i = 0; i < 2; i++)
        if (e.cppInit[i]) e.cppInit[i]();
    return 0;
}

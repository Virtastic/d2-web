// Prior art: none in D2MOO. Rules from cached 1.13c body (register arg ESI = Storm handle).
#include <cstdint>

struct D2Cmp8f70Ops {
    int32_t (*storm10106)(void);   // Ordinal_10106: -1 = failure
    void (*ord276)(void);
    void (*ord10265)(void);
    void (*ord10024)(const char* msg);
    void (*exit_)(int);            // _exit(-1), does not return
};

// 1.13c D2CMP+0x08F70
// Returns when handle nonzero and Ordinal_10106 != -1; otherwise reports and exits.
extern "C" void d2cmp_8f70(void* esi, const char* msg, const D2Cmp8f70Ops& ops) {
    if (esi) {
        if (ops.storm10106() != -1) return;
        ops.ord276();
    }
    ops.ord10265();
    ops.ord10024(msg);
    ops.exit_(-1);
}

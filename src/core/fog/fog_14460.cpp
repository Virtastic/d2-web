// 1.13c Fog+0x14460 -- SymGetLineFromAddr retry: probe addr, then addr-1 .. addr-99.
#include <cstdint>

namespace {
// Host seam for dbghelp SymGetLineFromAddr (Fog global 0x6FFA6E28); null when dbghelp is absent.
using SymGetLineFn = int (*)(uint32_t addr, uint32_t back);
SymGetLineFn g_symGetLine = nullptr;
}

// outDisp is the displacement slot the retail code receives in EDI; written only when a retry hit.
int fog_symGetLineRetry(uint32_t addr, uint32_t back, int* outDisp) {
    if (!g_symGetLine) return 0;
    if (g_symGetLine(addr, back)) return 1;
    for (int i = 1; i < 100; ++i)
        if (g_symGetLine(addr, back - i)) { *outDisp = i; return 1; }
    return 0;
}

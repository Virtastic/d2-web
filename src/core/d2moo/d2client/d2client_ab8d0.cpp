// 1.13c D2Client+0xAB8D0 loadRecordFile, written from the disassembly: no arguments. Opens "Record.dr1" (0xD4068; GENERIC_READ,
// OPEN_EXISTING, NORMAL), reads a 4-byte lead word into a local and the 0x14-byte header into 0xFCD78 {version, chunks, records, ..}.
// A version other than 1 clears 0x11BBE4 / 0x11BBE8 and closes. Otherwise per chunk i (i > 0 links a fresh 0x1F4C-byte chunk, Fog
// alloc line 0x35, after the current one at 0xDE694 +0x1F48 and makes it current, its count/next words zeroed) the record count
// (at most 500, stored at 0xFCD68; a count of 500 or less is fatal (line 0x100) unless this is the last chunk) is read as 16-byte
// records into the current chunk. Finally 0xDE694 is reset to the static chunk 0xFAE28 and the file closed.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_ab8d0(const Env& e, const GEnv& g) {
    const uint32_t ow[7] = { g.M(0xd4068), 0x80000000u, 0, 0, 3, 0x80, 0 };
    const uint32_t h = g.F(0xcefe0, ow);
    uint32_t lead = 0, got = 0;
    { const uint32_t sw[5] = { h, Stk(&lead), 4, Stk(&got), 0 }; g.F(0xcef60, sw, Stk(&lead)); }
    { const uint32_t sw[5] = { h, g.M(0xfcd78), 0x14, Stk(&got), 0 }; g.F(0xcef60, sw, g.G(0xfcd78)); }
    const uint32_t cw[1] = { h };
    if (e.R(g.G(0xfcd78)) != 1) { e.W(g.G(0x11bbe4), 0); e.W(g.G(0x11bbe8), 0); return g.F(0xcef54, cw); }
    for (uint32_t i = 0; (int32_t)i < (int32_t)e.R(g.G(0xfcd7c)); i++) {
        if (i) {
            const uint32_t n = e.H(0xbf7a, 0x35, 0, 0, 0, 0, 0, 0x1f4c, g.M(0xd4074));
            const uint32_t cur = e.R(g.G(0xde694));
            e.W(n + 0x1f40, 0); e.W(n + 0x1f44, 0); e.W(cur + 0x1f48, n); e.W(n + 0x1f48, 0); e.W(g.G(0xde694), n);
        }
        uint32_t t = e.R(g.G(0xfcd80));
        if ((int32_t)t <= 500) {
            e.W(g.G(0xfcd68), t);
            if (i + 1 != e.R(g.G(0xfcd7c))) { Fatal(e, g, 0x100); return 0; }
        } else { t = 500; e.W(g.G(0xfcd68), 500); }
        const uint32_t c = e.R(g.G(0xde694));
        const uint32_t sw[5] = { h, c, t << 4, Stk(&got), 0 }; g.F(0xcef60, sw, c);
    }
    e.W(g.G(0xde694), g.M(0xfae28));
    return g.F(0xcef54, cw);
}

// 1.13c D2Client+0xAC530, written from the disassembly: no arguments. Writes the received save image (pointer 0x11BBCC, length
// 0x11BBD0) to "<save dir><name>" (format 0xD405C, name 0x119928, mode "wb" 0xD4058). An empty name is fatal (line 0x25). The image
// must be at least 8 bytes and start 0xAA55AA55; version <= 0x5B needs 0x82 bytes and takes the short at +0x18, later versions
// 0x14F bytes and +0x24; that short goes to +0x1EB of the record at 0x11B980. Then 0xBF98 (ECX = dir buffer, EDX = 0x400),
// sprintf 0x3146, fopen 0x36F1, fwrite 0x380B (image, length, 1, fp) and fclose 0x38A3, all cdecl.
#include "cl_genv.h"
using namespace cl;
extern "C" void d2c13_ac530(const Env& e, const GEnv& g) {
    if (!B(g.G(0x119928))) { Fatal(e, g, 0x25); return; }
    const uint32_t len = e.R(g.G(0x11bbd0));
    if (len < 8) return;
    const uint32_t img = e.R(g.G(0x11bbcc));
    if (e.R(img) != 0xaa55aa55u) return;
    if (e.R(img + 4) <= 0x5b) { if (len < 0x82) return; S16(e.R(g.G(0x11b980)) + 0x1eb, W16(img + 0x18)); }
    else { if (len < 0x14f) return; S16(e.R(g.G(0x11b980)) + 0x1eb, W16(img + 0x24)); }
    uint8_t dir[0x400], path[0x1f0];
    e.H(0xbf98, 0, 0, 0, 0, 0, 0, Stk(dir), 0x400);
    e.H(0x3146, Stk(path), g.M(0xd405c), Stk(dir), g.M(0x119928));
    const uint32_t fp = e.H(0x36f1, Stk(path), g.M(0xd4058));
    if (!fp) return;
    e.H(0x380b, e.R(g.G(0x11bbcc)), e.R(g.G(0x11bbd0)), 1, fp);
    e.H(0x38a3, fp);
}

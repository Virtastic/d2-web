// 1.13c D2Client+0x7DC00 (502 B, fastcall ECX = unit, void): unit name label with the "\xFF\xFF<id>" coloured-text branch.
// Same preamble as 0x7DAA0 (y shifted by -0x1E). The id's text (0xCAF0 into a 0x108-byte buffer) starting FF FF with atol(buf+2) in
// 0x42D..0x50F, 0x5587..0x5615, 0x2AA4 or 0x5586 and a non-null 0xD1EC string draws that string (200 wchars). Otherwise a 256-wchar
// buffer goes to 0x7B4F0 (EAX = buffer, EDX = the text, EDI = movzx 0xCAFC(id)); on a true answer the last wchar is 0 and 0x7CA40 draws it.
#include "cl_namelabel.h"
// 1.13c D2Client+0x7DC00
extern "C" void d2c_7dc00(uint32_t unit, uint32_t ga, uint32_t gm, uint32_t gs, const cl::Env& e) {
    int32_t pos[2] = {0, 0};
    uint32_t id;
    if (!cl::labelPos(e, unit, ga, gm, gs, -0x1e, pos, id)) return;
    uint8_t nb[0x108] = {};
    const uint32_t nbp = (uint32_t)(uintptr_t)nb;
    e.H(0xcaf0, id, nbp);
    if (nb[0] == 0xff && nb[1] == 0xff) {
        const int32_t l = (int32_t)e.H(0x39c4, nbp + 2);
        if ((l >= 0x42d && l <= 0x50f) || (l >= 0x5587 && l <= 0x5615) || l == 0x2aa4 || l == 0x5586) {
            const uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, (uint32_t)l);
            if (s) {
                uint16_t buf[200];
                cl::labelDraw(e, unit, id, pos, s, 0, buf, 200);
                return;
            }
        }
    }
    uint16_t buf[256];
    for (uint32_t i = 0; i < 256; ++i) buf[i] = 0;
    const uint32_t edi = e.H(0xcafc, id) & 0xff;
    const uint32_t ok = e.H(0x7b4f0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)buf, 0, nbp, 0, 0);
    e.Edj(0x7b4f0, edi);
    if (!(ok & 0xff)) return;
    buf[255] = 0;
    e.H(0x7ca40, (uint32_t)(uintptr_t)buf, 0, 0, 0, 0, (uint32_t)(uintptr_t)pos, id, unit);
    if (e.blk) e.blk(e.ctx, 0x7ca40, (uint32_t)(uintptr_t)pos, 2);
}

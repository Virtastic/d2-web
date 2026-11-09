// 1.13c D2Client+0x83ec0 -- item packet "equip from server". EAX = packet (id dword at +4), [esp+4] = the decoded record
// (u16 +4, u16 +6, byte +8, dword +0xC, byte +0x10). player = *(D2Client+0x11BBFC) (null is fatal); the client item hash
// (D2Client+0x10AE08, 128 buckets, chain +0xE4, id +0xC, type +0) is walked for the packet id (a hit whose type is not 4 is fatal).
// D2Common 0xCA3C(rec[0xC], 1) == 0: update the found item in place (0x82820 with EAX = rec byte 8 and EDI = 0, then 0xCA24); else
// 0xA6EF0 (ECX = the hash, EDX = id & 0x7F) and a fresh item from 0x835C0 (EBX = 0; null is fatal). The stack slot that held the
// record pointer is scratch afterwards: a byte stored into it leaves the pointer's upper three bytes in the word later pushed.
#include "cl_env.h"
extern "C" void d2c_83ec0(uint32_t pkt, uint32_t rec, const cl::Env& e) {
    const uint32_t pl = e.self;
    uint32_t slot = rec;                                       // the scratch stack word
    const uint32_t id = e.R(pkt + 4);
    uint32_t node = 0;
    for (uint32_t n = e.R(e.gl + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) { if (e.R(n) != 4) return; node = n; break; }   // fatal in retail
    uint32_t it;
    if (!e.H(0xca3c, e.R(rec + 0xc), 1)) {
        const uint32_t dl = e.R8(rec + 0x10);
        if (!node) return;                                     // fatal in retail
        slot = (slot & ~0xffu) | dl;
        e.H(0x82820, 0, 0, 0, 0, 0, e.R8(rec + 8));
        e.Edj(0x82820, 0);
        e.H(0xca24, node, slot);
        it = node;
    } else {
        e.H(0xa6ef0, id, 4, 0, 0, 0, 0, 0xB2000000u | 0x10AE08u, id & 0x7f);
        it = e.H(0x835c0, pkt, rec, 0, 0, 0, 0, 0, 0, 0, 0);
        if (!it) return;                                       // fatal in retail
    }
    const uint32_t owner = e.R(pl + 0x60);
    if (!owner) return;                                        // fatal in retail
    e.H(0xca06, it, e.R(rec + 4) & 0xffff);
    e.H(0xca48, it, e.R(rec + 6) & 0xffff);
    if (!e.H(0xca6c, owner, it, 1)) return;
    slot = (slot & ~0xffu) | (e.H(0xc92e, it) & 0xff);
    if (e.R8(rec + 0x10) == 0xff) return;
    const uint32_t r0 = e.H(0xc8a4, pl, slot, e.R(e.gl + 512), 1, slot);
    const uint32_t r1 = e.H(0x1660, r0, 0, 0, 0, 0, 0, it);
    const uint32_t r2 = e.H(0x1630, r1, 0, 0, 0, 0, 0, it);
    if (!e.H(0xca9c, owner, it, r2)) return;                   // fatal in retail
    if (e.H(0xca30, it, pl)) e.H(0x81ff0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
    e.H(0xbe2b0);
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
}

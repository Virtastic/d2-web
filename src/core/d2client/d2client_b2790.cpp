// 1.13c D2Client+0xb2790 -- client missile: follow the owner while the owner keeps the spawning skill. ECX = unit, no return.
// Failure: 0xA6EF0(ECX = 0x109A08 + (type << 9), EDX = guid & 0x7F; guid, type) -- a null unit passes type 6, guid -1; a bad class,
// row +0x1E < 0, no owner (0xA60D0(EAX = unit)), 0xC310(owner) == 0 or 0xC42A(that, file, 0x8ED) != 0xC388(unit) pass the
// unit's type (+0) and guid (+0xC). Otherwise 0xC352(path) != 0: 0xB0450(ESI = unit; row +0x1E, 0, 0, 0xC388(unit),
// 0xC3FA(unit)) (0xC3FA called first); then 0xC346(unit, owner) and the 0xB17C0(ECX = unit) tail.
#include "cl_env.h"
namespace {
void fail(const cl::Env& e, uint32_t ty, uint32_t guid) {
    e.H(0xa6ef0, guid, ty, 0, 0, 0, 0, 0xB2000000u | ((ty << 9) + 0x109a08u), guid & 0x7f);
}
}
extern "C" void d2c_b2790(uint32_t u, const cl::Env& e) {
    if (!u) { fail(e, 6, 0xffffffffu); return; }
    uint32_t row = e.UnitRow(u);
    if (!row || e.R16s(row + 0x1e) < 0) { fail(e, e.R(u), e.R(u + 0xc)); return; }
    uint32_t own = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!own) { fail(e, e.R(u), e.R(u + 0xc)); return; }
    uint32_t sk = e.H(0xc310, own);
    if (!sk) { fail(e, e.R(u), e.R(u + 0xc)); return; }
    uint32_t id = e.H(0xc42a, sk, 0xB2000000u | 0xd4010u, 0x8ed);
    if (id != e.H(0xc388, u)) { fail(e, e.R(u), e.R(u + 0xc)); return; }
    if (e.H(0xc352, e.R(u + 0x2c))) {
        uint32_t c3fa = e.H(0xc3fa, u);
        uint32_t c388 = e.H(0xc388, u);
        e.H(0xb0450, (uint32_t)e.R16s(row + 0x1e), 0, 0, c388, c3fa, 0, 0, 0, 0, u);
    }
    e.H(0xc346, u, own);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}

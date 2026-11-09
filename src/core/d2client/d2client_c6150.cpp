// 1.13c D2Client+0xC6150, written from the disassembly. EAX = unit, stack (target, x, y), ret 0xC. target path = [target+0x2C] (null: return).
// rec = 0xA4E20(EAX = 10631(unit) & 0x7F, ECX = 10631(unit), EDX = 0x10A608 + (10179(unit) << 9); 10179(unit)) (null: return). Else: 10137(unit, 1),
// 10542(target, 0xC), [target+0x48] = ([target+0x48] & ~0xFF) + 0x100, 10338-ish 10054(target, 0), 10770(path), 10168(path, 5), 11005(path, 1),
// 10336(path, 0); the rec's size (+0xC / +0x10 of its path for types 2 and 4..5, else 10867 / 10750 of it) gives bx = 3x - 2w, di = 3y - 2h;
// 0x1400(EAX = 10331(target), EBX = bx) nonzero with 10057 of it nonzero returns, else 10286(path, bx, di), 10647(path, 0xB), 10527(path, 5), 10334(path, target, 0).
#include "cl_mb2.h"
// 1.13c D2Client+0xC6150
extern "C" uint32_t d2c_c6150(uint32_t u, uint32_t tg, uint32_t x, uint32_t y, const clm::Env& e) {
    const uint32_t path = e.R(tg + 0x2c);
    if (!path) return 0;
    const uint32_t kind = e.H(0xcb9e, 0, 0, 0, 0, 0, u);
    const uint32_t v = e.H(0xc79c, 0, 0, 0, 0, 0, u);
    const uint32_t rec = e.H(0xa4e20, v & 0x7f, v, clm::Env::Mod(0x10a608 + (kind << 9)), 0, 0, kind);
    if (!rec) return 0;
    e.H(0xc60a, 0, 0, 0, 0, 0, u, 1);
    e.H(0xcb56, 0, 0, 0, 0, 0, tg, 0xc);
    e.W(tg + 0x48, (e.R(tg + 0x48) & 0xffffff00u) + 0x100);
    e.H(0xc646, 0, 0, 0, 0, 0, tg, 0);
    e.H(0xcd2a, 0, 0, 0, 0, 0, path);
    e.H(0xc3b8, 0, 0, 0, 0, 0, path, 5);
    e.H(0xc682, 0, 0, 0, 0, 0, path, 1);
    e.H(0xc364, 0, 0, 0, 0, 0, path, 0);
    const int32_t t = (int32_t)e.R(rec);
    const bool box = t == 2 || (t > 3 && t <= 5);
    const uint32_t rp = e.R(rec + 0x2c);
    const uint32_t w = box ? e.R(rp + 0xc) : (rp ? e.H(0xc3be, 0, 0, 0, 0, 0, rp) : 0u);
    const uint32_t bx = 3 * x - 2 * w;
    const uint32_t h = box ? e.R(rp + 0x10) : (rp ? e.H(0xc2ec, 0, 0, 0, 0, 0, rp) : 0u);
    const uint32_t di = 3 * y - 2 * h;
    uint32_t r = e.H(0xc1f0, 0, 0, 0, 0, 0, tg);
    r = e.H(0x1400, r, 0, 0, bx);
    if (r && e.H(0xc208, 0, 0, 0, 0, 0, r)) return 0;
    e.H(0xc3ee, 0, 0, 0, 0, 0, path, bx, di);
    e.H(0xc38e, 0, 0, 0, 0, 0, path, 0xb);
    e.H(0xcd36, 0, 0, 0, 0, 0, path, 5);
    e.H(0xc376, 0, 0, 0, 0, 0, path, tg, 0);
    return 0;
}

// 1.13c D2Client+0x928C0, written from the disassembly: the set-item tooltip. stdcall(unit), ret 4. It composes the hovered set
// item's text (set name, the item's own lines, the requirement / damage / defence / socket lines, the set members it is worn with)
// into one wide string through Unicode strcpy/strcat (IAT 0xCEAB8 / 0xCEABC, run in place) and the colour prefixer 0xBF5F0, measures it
// with D2Win #10177 (thunk 0xD3B4 -> slot 0xCEBA0) and draws it with D2Win #10085 (thunk 0xD384 -> slot 0xCEBC8). Strings come from
// D2Lang #10003 (thunk 0xD1EC -> slot 0xCEACC, id in CX).
// The locals live in one frame laid out as retail's (esp-relative, 0x4F90 bytes plus the four saved registers): the Unicode buffers
// are constructed by a no-op ctor, so a buffer no callee filled is read as whatever the frame held; the frame here starts zeroed.
// e.gl = the module globals: +0 0x11BBFC (the player unit) .. +0x3C 0x11BC38 (the hovered item), i.e. 0x11BBFC..0x11BC3B
// (+0x2C 0x11BC28 and +0x30 0x11BC2C: the two panel gates, +0x38 0x11BC34: the trade/vendor mode); +0x40 0xE0EA4..0xE0EB7
// (+0x40 the vendor argument, +0x44 / +0x48 the right-anchored x and width, +0x4C / +0x50 the left x and y bias).
// e.dt = *sgptDataTables: setitems at dt+0xC18 (count +0xC1C, stride 0x1B8), sets at dt+0xC0C (count +0xC10, stride 0x128).
#include <cstdint>
#include <cstring>
#include "cl_env.h"
namespace {
using W = char16_t;
constexpr uint32_t kEmpty = 0xB20D359Fu, kInv = 0xB20D476Cu;   // "" and "inv.cpp" (module strings)
constexpr uint32_t S10003 = 0xCEACC, S10177 = 0xCEBA0, S10085 = 0xCEBC8;
uint32_t G(uint32_t rva) { return rva >= 0x11bbfc ? rva - 0x11bbfc : 0x40 + (rva - 0xe0ea4); }
uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
W* P(uint32_t a) { return (W*)(uintptr_t)a; }
void cpy(W* d, const W* s) { while ((*d++ = *s++)) {} }
void cat(W* d, const W* s) { while (*d) ++d; cpy(d, s); }
struct T {
    const cl::Env& e;
    W* text(uint32_t id) const { return P(e.H(S10003, 0, 0, 0, 0, 0, 0, id)); }
    // 0xBF5F0 (EDI = buf, byte colour, ret 4): a non-empty buf becomes text(0xF9A) + ('0' + colour) + buf; returns buf
    W* colour(W* buf, uint32_t c) const {
        if (!buf[0]) return buf;
        W tmp[0x400] = {};
        cpy(tmp, text(0xf9a));
        const W ch[2] = { (W)(uint8_t)(c + 0x30), 0 };
        cat(tmp, ch);
        cat(tmp, buf);
        cpy(buf, tmp);
        return buf;
    }
    uint32_t hE(uint32_t rva, uint32_t edi, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t a = 0, uint32_t b = 0) const {
        e.Ed(edi);
        const uint32_t r = e.H(rva, s0, s1, s2, 0, 0, a, 0, 0, b);
        e.Ej(rva, edi);
        return r;
    }
};
}  // namespace
// 1.13c D2Client+0x928C0
extern "C" void d2c_928c0(const cl::Env& e, uint32_t unit) {
    const T t{ e };
    const auto R = [&](uint32_t rva) { return e.R(e.gl + G(rva)); };
    static_assert(sizeof(W) == 2, "UCS-2");
    alignas(4) uint8_t F[0x4fa0];
    memset(F, 0, sizeof F);
    const auto B = [&](uint32_t off) { return (W*)(F + off); };
    const auto D = [&](uint32_t off) -> uint32_t& { return *(uint32_t*)(F + off); };
    // frame: 0x10 who, 0x14 sep, 0x18 rec, 0x1C set, 0x20/0x24/0x28 the three requirement colour flags, 0x2C unit+0x60,
    // 0x30/0x3C/0x38 the 10244 out-flags (0x34 a zero colour), 0x40/0x44 the measured size, then the UCS-2 buffers
    W* const b48 = B(0x48); W* const bc8 = B(0xc8); W* const b148 = B(0x148); W* const b348 = B(0x348); W* const bb48 = B(0xb48);
    W* const bd48 = B(0xd48); W* const bf48 = B(0xf48); W* const b1148 = B(0x1148); W* const b1348 = B(0x1348); W* const b1548 = B(0x1548);
    W* const b1748 = B(0x1748); W* const b1948 = B(0x1948); W* const b2148 = B(0x2148); W* const b2348 = B(0x2348); W* const b25a0 = B(0x25a0);
    W* const b27a0 = B(0x27a0); W* const b29a0 = B(0x29a0); W* const b2ba0 = B(0x2ba0); W* const b2fa0 = B(0x2fa0); W* const b33a0 = B(0x33a0);
    W* const b37a0 = B(0x37a0); W* const b3fa0 = B(0x3fa0);
    D(0x28) = 0; D(0x20) = 0; D(0x24) = 0;
    W* const sep = t.text(0xf9e);
    D(0x14) = A(sep);
    uint32_t who = unit;
    if (unit && e.R(unit) == 1 && !e.H(0xc54a, unit)) who = R(0x11bbfc);
    D(0x10) = who;
    D(0x2c) = e.R(unit + 0x60);                           // read before any gate: a null unit faults in retail
    if (!R(0x11bc2c) && !R(0x11bc28)) return;
    if (!R(0x11bc38)) return;
    if (e.H(0xc202, D(0x2c))) return;
    uint32_t item = R(0x11bc38);
    const uint32_t rec = e.H(0xc1ba, item ? e.R(item + 4) : 0xffffffffu);
    D(0x18) = rec;
    if (!rec) {
        const uint32_t m = e.H(0xbf92, 0xd56);
        e.H(0xbf80, kEmpty, m);
        e.H(0x3664, 0xffffffffu);
        return;                                           // the fatal exit does not return
    }
    item = R(0x11bc38);
    const int32_t si = (int32_t)e.H(0xc808, item);        // 0x1870: the setitems row
    if (si < 0 || si >= (int32_t)e.R(e.dt + 0xc1c)) return;
    const uint32_t row = e.R(e.dt + 0xc18) + (uint32_t)si * 0x1b8;
    if (!row) return;
    const int32_t sn = e.R16s(row + 0x2c);                // 0x1840: the sets row
    if (sn < 0 || sn >= (int32_t)e.R(e.dt + 0xc10)) return;
    const uint32_t set = e.R(e.dt + 0xc0c) + (uint32_t)sn * 0x128;
    if (!set) return;
    D(0x1c) = set;
    cpy(b1348, t.text(e.R(set + 2) & 0xffff));
    cat(b1348, sep);
    D(0x34) = 0;
    item = R(0x11bc38);
    e.Xs(A(F + 0x38));
    e.H(0xc898, item, who, 0, A(F + 0x30), A(F + 0x3c));
    e.H(0x90770, R(0x11bc38), 0, 0, 0, 0, 0, 0, 0, 0, A(b2148));
    const int32_t n = (int32_t)e.H(0xc8aa, R(0x11bc38), who);
    if (n > 1) {
        if (!D(0x38)) D(0x28) = 1;
        e.H(0x8de40, (uint32_t)n, 0, 0, 0, 0, 0, 0, 0, 0, A(b29a0));
    }
    if (e.H(0xc1cc, R(0x11bc38), 0x2d) || e.H(0xc1cc, R(0x11bc38), 0x32)) {
        uint32_t edi = 0, ebx = 0;
        const uint32_t p = e.H(0xc454, R(0x11bc38), 0x5b, 0);
        if (p) {
            edi = e.H(0x14a0, 0, 0, 0, 0, 0, p, 100, e.R(rec + 0x10a) & 0xffff);
            ebx = e.H(0x14a0, 0, 0, 0, 0, 0, p, 100, e.R(rec + 0x10c) & 0xffff);
        }
        if (e.H(0xc1a8, R(0x11bc38), 0x400000, 0xd88, kInv)) { edi -= 10; ebx -= 10; }
        if (e.R(rec + 0x10a) & 0xffff) {
            if (!D(0x30)) D(0x20) = 1;
            t.hE(0x8dd80, A(b27a0), rec, edi);
        }
        if (e.R(rec + 0x10c) & 0xffff) {
            if (!D(0x3c)) D(0x24) = 1;
            t.hE(0x8dcc0, A(b25a0), rec, ebx);
        }
    }
    cpy(b1948, t.colour(b29a0, D(0x28) & 0xff));
    cat(b1948, t.colour(b27a0, D(0x20) & 0xff));
    cat(b1948, t.colour(b25a0, D(0x24) & 0xff));
    const uint32_t kind = e.H(0xc9a0, R(0x11bc38));
    who = D(0x10);
    if (kind != 7) {
        cat(bb48, t.text(kind + 0x2aa5));
        cat(bb48, sep);
        const uint32_t c = (who && e.R(who) == 0 && e.R(who + 4) != kind) ? 1u : D(0x34);
        cat(b1948, t.colour(bb48, c & 0xff));
    }
    cat(b1948, t.colour(b2148, 0));
    if (e.H(0xc1cc, R(0x11bc38), 0x2d)) {
        e.H(0x8f9e0, A(b1148), 0, 0, 0, 0, R(0x11bc38));
        t.colour(b1148, 0);
        cat(b1948, b1148);
        if ((int32_t)e.H(0xc84a, R(0x11bc38), 0) >= 0 && (int32_t)e.H(0xc8ec, R(0x11bc38), 0) >= 0) {
            e.H(0x8ff20, R(0x11bc38), A(b1748));
            t.colour(b1748, 0);
            cat(b1948, b1748);
        }
    }
    if (e.H(0xc1cc, R(0x11bc38), 0x33)) {
        if (who && e.R(who) == 0 && e.R(who + 4) == 3 && (!e.H(0xc88c, R(0x11bc38)) || e.H(0xc9a0, R(0x11bc38)) == 3)) {
            t.hE(0x8d940, A(bf48), R(0x11bc38), rec);
            t.colour(bf48, 0);
            cat(b1948, bf48);
        }
        e.H(0x8fdc0, A(b1548), rec, 0, 0, 0, R(0x11bc38));
        t.colour(b1548, 0);
        cat(b1948, b1548);
    }
    if (e.H(0xc1cc, R(0x11bc38), 0x32) && (int32_t)e.H(0xc93a, R(0x11bc38)) > 0) {
        e.H(0x8fb80, who, R(0x11bc38), A(bd48));
        t.colour(bd48, 0);
        cat(b1948, bd48);
    }
    e.H(0x914f0, R(0x11bc38), A(b148), 0x100);
    t.colour(b148, e.H(0xc1a8, R(0x11bc38), 0x100, 0xddb, kInv) ? 1 : 2);
    cat(b1948, b148);
    b37a0[0] = 0;
    if (e.H(0xc1a8, R(0x11bc38), 0x800, 0xde4, kInv)) {
        e.H(0x8e180, R(0x11bc38), 0, 0, 0, 0, 0, 0, 0, 0, A(b2348));
        cpy(b37a0, b2348);
    }
    t.hE(0x560b0, A(b37a0), R(0x11bc38), 1, 0);
    item = R(0x11bc38);
    b2fa0[0] = 0;
    t.hE(0x55fb0, item, A(b2fa0), 0x200, 0, who, 0);      // EAX = the owner, EDI = the item
    b2ba0[0] = 0;
    const uint32_t it2 = R(0x11bc38);
    if (it2 && e.R(it2 + 0x10) == 1) t.hE(0x55f10, item, A(b2ba0), 0x200, 0, it2, who);
    for (int32_t i = 0; i < (int32_t)e.R(set + 0xc); ++i) {
        const uint32_t p = e.R(set + 0x110 + 4 * (uint32_t)i);
        if (!p) break;
        const uint32_t r = e.H(0x8d430, 0, 0, 0, 0, 0, D(0x2c), 0, 0, (uint32_t)e.R16s(p));
        const uint32_t nm = e.R(p + 0x24) & 0xffff;
        if (nm) {
            const W* t1 = t.text(nm);
            const W* t2 = t.text(0x2769);
            e.H(0x90cb0, A(b348), A(t2), A(t1), 0);
            cat(b348, sep);
        }
        t.colour(b348, r ? 2 : 1);
        cat(b33a0, b348);
    }
    cpy(b348, t.colour(b33a0, 2));
    cat(b348, t.colour(b1348, 4));
    if (b2ba0[0]) { cat(b348, sep); cat(b348, t.colour(b2ba0, 4)); }
    cat(b348, sep);
    if (b2fa0[0]) cat(b348, t.colour(b2fa0, 2));
    cat(b348, t.colour(b37a0, 3));
    cat(b348, b1948);
    const int32_t g = (int32_t)R(0x11bc34);
    if (g >= 1 && g <= 9) {
        const uint32_t v = e.H(0x46230, R(0xe0ea4), A(b48), 0x40, 0, 0, A(F + 0x2c), R(0x11bc38));
        cpy(b3fa0, b348);
        if (v) {
            if (b48[0]) cat(b3fa0, sep);
            cat(b3fa0, b48);
        } else if (R(0x11bc34) != 4) {
            cpy(bc8, t.text(0xd05));
            cat(b3fa0, sep);
            cat(b3fa0, t.colour(bc8, 1));
        }
    } else {
        cpy(b3fa0, b348);
    }
    e.H(S10177, A(F + 0x40), 0, 0, 0, 0, 0, A(b3fa0), A(F + 0x44));
    const uint32_t c = R(0xe0eac), h40 = D(0x40);
    if ((int32_t)(c - h40) > 0) e.H(S10085, c, 0, 1, 0, 0, 0, A(b3fa0), R(0xe0ea8));
    else e.H(S10085, h40 + R(0xe0eb4), 0, 1, 0, 0, 0, A(b3fa0), R(0xe0eb0));
}

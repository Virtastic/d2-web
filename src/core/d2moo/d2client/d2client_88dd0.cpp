// 1.13c D2Client+0x88DD0 sound-entry tick (stdcall now, ret 4). Walks the active-sound list at 0x11BF30 (nodes {entry,
// next}): bubble-sorts it, retires stopped entries, refreshes each entry's volume ramp, starts / stops the D2Sound voice
// against the listener range, frees the entries D2Sound handed back, then steps the two ambient fades (0xE1374 / 0xE1378).
// Module globals come in through gb[] (the six pinned ranges below); the inlined 0x88440 is part of the body.
// x87 at the retail 53-bit precision control is IEEE double here (every product of two floats is exact in a double,
// every rounding point the retail code stores to a float is a (float) cast).
#include <cstring>
#include "cl_env.h"
namespace {
// gb[]: 0 = 0x11BF30 (list, enable, -, lastTick, pending), 1 = 0x11C24C.., 2 = 0xFADA4.., 3 = 0xE1374.., 4 = 0x123710, 5 = 0x11BBFC
struct G {
    const uint32_t* b;
    uint32_t a(uint32_t rva) const {
        if (rva >= 0x11bf30 && rva < 0x11bf44) return b[0] + (rva - 0x11bf30);
        if (rva >= 0x11c24c && rva < 0x11c3a4) return b[1] + (rva - 0x11c24c);
        if (rva >= 0xfada4 && rva < 0xfadb0) return b[2] + (rva - 0xfada4);
        if (rva >= 0xe1374 && rva < 0xe137c) return b[3] + (rva - 0xe1374);
        if (rva == 0x123710) return b[4];
        return b[5];   // 0x11BBFC
    }
};
const uint32_t kMod = 0xB2000000u;
float F(uint32_t bits) { float f; memcpy(&f, &bits, 4); return f; }
uint32_t B(float f) { uint32_t u; memcpy(&u, &f, 4); return u; }
const float kRange[5] = { 400.0f, 700.0f, 1000.0f, 1500.0f, 2000.0f };   // 0xD7554, 0xD7544 (1 and default), 0xD7550, 0xD754C, 0xD7548
const float kStep = F(0x3d4ccccd);   // 0xD75AC (0.05)
const float kHalf = 0.5f;           // 0xD7568
uint32_t rec(const cl::Env& e, const G& g, uint32_t id) {
    const uint32_t t = e.R(g.a(0x11c24c));
    if (!t || (int32_t)id < 0 || (int32_t)id >= (int32_t)e.R(g.a(0x11c250))) return 0;
    return id * 0x8e + t;
}
void fatal(const cl::Env& e, uint32_t line) {
    const uint32_t s = e.H(0xbf92, line);
    e.H(0xbf80, kMod | 0xD359F, s);
    e.H(0x3664, 0xffffffffu);
}
void stop(const cl::Env& e, uint32_t ent) { e.H(0x880e0); e.Ej(0x880e0, ent); }   // EDI = entry
// 0x88440 (EAX = sound id, stack item): 0.5 when the item hears the sound, else 0
float hears(const cl::Env& e, const G& g, uint32_t id, uint32_t item) {
    const uint32_t r = rec(e, g, id);
    if (r) {
        if (e.R(r + 0x47) == 4) return 0.0f;
        if (e.R(r + 0x60) == 0xca) {
            const uint32_t k = e.H(0x5eb90);
            const uint32_t t = e.R(g.a(0x11c254));
            if (!t || (int32_t)k < 0 || (int32_t)k >= (int32_t)e.R(g.a(0x11c258)) || !(k * 0x58 + t)) return 0.0f;
            return e.R8(k * 0x58 + t + 0x18) ? kHalf : 0.0f;
        }
    }
    return e.H(0xc9be, e.R(g.a(0x11bbfc)), item, 2) ? kHalf : 0.0f;
}
}  // namespace
extern "C" void d2c_88dd0(uint32_t now, const uint32_t* gb, const cl::Env& e) {
    const G g{ gb };
    if (!e.R(g.a(0x11bf34))) return;
    uint32_t lst = 0;   // the D2Sound release list (stack local [esp+0x14])
    const uint32_t lp = (uint32_t)(uintptr_t)&lst;
    if (e.R(g.a(0x11bf40))) e.H(0x88d90);
    const uint32_t head = g.a(0x11bf30);
    for (bool done = false; !done;) {   // bubble sort: priority byte desc, distance asc, start tick desc, address desc
        done = true;
        for (uint32_t n = e.R(head); n; n = e.R(n + 4)) {
            const uint32_t nx = e.R(n + 4);
            if (!nx) continue;
            const uint32_t c = e.R(n), x = e.R(nx);
            const uint32_t bx = e.R8(x + 0x3c), bc = e.R8(c + 0x3c);
            bool sw;
            if (bx != bc) sw = bx > bc;
            else {
                const float fx = F(e.R(x + 0x18)), fc = F(e.R(c + 0x18));
                if (fx < fc) sw = true;
                else if (fx > fc) sw = false;
                else {
                    const uint32_t tx = e.R(x + 0x34), tc = e.R(c + 0x34);
                    sw = tx != tc ? tx > tc : c < x;
                }
            }
            if (sw) { e.W(n, x); e.W(nx, c); done = false; }
        }
    }
    for (uint32_t n = e.R(head); n; n = e.R(n + 4)) {
        const uint32_t en = e.R(n);
        if (e.R(en + 0x3d) && (!e.R(en + 0x45) || e.R(en + 0x4d))) {
            if (!e.R(en + 0x30)) e.W(en + 0x30, 2);
            if (e.R(en + 0x30) == 1) stop(e, en);
        }
    }
    const uint32_t on = (int32_t)e.H(0xd270) > 0 ? 1 : 0;
    for (uint32_t n = e.R(head); n; n = e.R(n + 4)) {
        const uint32_t en = e.R(n), id = e.R(en + 4);
        uint32_t r = rec(e, g, id);
        uint32_t ok = on;
        if ((int32_t)id >= (int32_t)e.R(g.a(0x11c25c)) && (int32_t)id <= (int32_t)e.R(g.a(0x11c260)) && !e.H(0xd26a, 0, 0, 0, 0, 0, id, on))
            ok = 0;
        if (e.R(en + 0x30) == 2) {
            if (e.R(en + 0x2c)) { fatal(e, 0x349); return; }
            if (!e.R8(r + 0x3e) || (e.R(r + 0x43) & 0xffff) || e.R(en + 0x3d)) e.H(0xd276, 0, 0, 0, 0, 0, 0, lp, en);
            else {
                if (e.R(en + 0x45)) { e.W(en + 0x1c, e.R(en + 0x4d)); e.W(en + 0x45, 0); }
                e.W(en + 0x30, 0);
            }
            continue;
        }
        uint32_t pl;
        if (e.R(g.a(0x11c3a0)) && e.R8(r + 0x51) && (pl = e.R(en + 0x28)) != 0) {
            uint32_t vec[3] = {}, best[3] = {}, bestItem = 0, cnt = 0;
            float bestD = F(0xbf800000u), sum = 0.0f;
            const uint32_t vp = (uint32_t)(uintptr_t)vec;
            do {
                const uint32_t item = e.R(pl);
                if (!item) { fatal(e, 0x36c); return; }
                e.Ed(vp);
                e.H(0x87f30, item, id);
                e.Ej(0x87f30, vp);
                const double v1 = F(vec[1]), v0 = F(vec[0]);
                const double d = v1 * v1 + v0 * v0;
                if (!bestItem || d < (double)bestD) {
                    bestD = (float)d; best[0] = vec[0]; best[1] = vec[1]; best[2] = vec[2]; bestItem = item;
                }
                sum = (float)((double)hears(e, g, id, item) + (double)sum);
                pl = e.R(pl + 4);
                ++cnt;
            } while (pl);
            const uint32_t bp = (uint32_t)(uintptr_t)best;
            e.H(0x88350, 0, 0, 0, 0, 0, 0, bp, en);
            if (e.blk) e.blk(e.ctx, 0x88350, bp, 3);
            const double avg = (double)sum / (double)(int32_t)cnt;   // 0x123710 (the FDIV-bug flag) picks an equal-result divide
            const float af = (float)avg;
            const double cur = F(e.R(en + 0x20));
            double t;
            float out;
            if (avg > cur) { t = cur + (double)kStep; out = t < (double)af ? (float)t : af; }
            else { t = cur - (double)kStep; out = t > (double)af ? (float)t : af; }
            e.W(en + 0x20, B(out));
        }
        if (e.R(en + 0x45)) {
            const uint32_t t55 = e.R(en + 0x55), t51 = e.R(en + 0x51);
            if (now > t55) { e.W(en + 0x1c, e.R(en + 0x4d)); e.W(en + 0x45, 0); }
            else if (now < t51) e.W(en + 0x1c, e.R(en + 0x49));
            else {
                const int32_t a49 = (int32_t)e.R(en + 0x49);
                const int32_t num = (int32_t)(((uint32_t)e.R(en + 0x4d) - (uint32_t)a49) * (now - t51));
                e.W(en + 0x1c, (uint32_t)(num / (int32_t)(t55 - t51) + a49));
            }
        }
        const uint32_t r2 = rec(e, g, e.R(en + 4));
        const uint32_t sel = e.R(r2 + 0x47);
        const float rg = sel == 0 ? kRange[0] : sel == 2 ? kRange[2] : sel == 3 ? kRange[3] : sel == 4 ? kRange[4] : kRange[1];
        const float r2f = (float)((double)rg * (double)rg);
        auto inRange = [&]() { const float d = F(e.R(en + 0x18)); return d < r2f || d == r2f; };
        if (!e.R(en + 0x30) && now >= e.R(en + 0x34) && e.R(en + 0x1c)) {
            if (ok && inRange()) {
                if (e.R(en + 0x2c)) { fatal(e, 0x3a1); return; }
                const uint32_t lpl = e.R(en + 0x28);
                const uint32_t one = (lpl && !e.R(lpl + 4)) ? e.R(lpl) : 0;
                const bool moved = !(F(e.R(en + 0xc)) == 0.0f && F(e.R(en + 0x10)) == 0.0f && F(e.R(en + 0x14)) == 0.0f);
                uint32_t pick = 0;
                if (e.R8(r + 0x41) || e.R8(r + 0x42)) pick = e.H(0x87c30, e.R(r + 0x60), one, en);
                const int32_t sid = (int32_t)e.R(en + 4);
                if (one && sid >= 0xb76 && sid <= 0x1230) pick = e.H(0x87bd0, en);
                bool skip = false;
                if (!(e.R8(r + 0x41) && !e.R(en + 0x28) && moved) && pick) {
                    if (!e.R8(r + 0x42)) {
                        e.W(en + 0x3d, 1);
                        e.H(0xd276, 0, 0, 0, 0, 0, 0, lp, en);
                        skip = true;
                    } else {
                        e.W(pick + 0x3d, 1);
                        e.H(0xd276, 0, 0, 0, 0, 0, 0, lp, pick);
                    }
                }
                if (skip) continue;
                const uint32_t v41 = e.R(en + 0x41);
                uint32_t ramp = 0, keep = 0;
                bool take = false;
                if (v41) { e.W(en + 0x41, 0); e.W(en + 0x24, v41); ramp = 3; take = true; }
                else if (e.R8(r + 0x3f) && !e.R(en + 0x45) && !(e.R8(en + 0x38) & 2)) { ramp = e.R8(r + 0x3f); take = true; }
                if (take) { keep = e.R(en + 0x1c); e.W(en + 0x1c, 0); }
                const uint32_t snd = e.H(0x5d7a0, en);
                e.W(en + 0x2c, snd);
                if (snd) {
                    e.W(en + 0x30, 1);
                    if (ramp) {
                        e.W(en + 0x49, e.R(en + 0x1c)); e.W(en + 0x4d, keep);
                        e.W(en + 0x51, now); e.W(en + 0x55, ramp + now); e.W(en + 0x45, 1);
                    }
                }
                r = rec(e, g, e.R(en + 4));
            }
            if (!e.R(en + 0x2c) && !e.R8(r + 0x3e) && (!e.R8(r + 0x4d) || e.R(r + 0x86) != 1))
                e.H(0xd276, 0, 0, 0, 0, 0, 0, lp, en);
        }
        if (e.R(en + 0x30) == 1) {
            const uint32_t snd = e.R(en + 0x2c);
            if (!snd) { fatal(e, 0x3f3); return; }
            const uint32_t w43 = e.R(r + 0x43) & 0xffff;
            const float d = F(e.R(en + 0x18));
            if ((w43 && now - e.R(en + 0x34) > w43) || !ok || d > r2f) stop(e, en);
            else {
                e.H(0x5d1d0, 0, 0, 0, 0, 0, 0, 0, 0, snd, en);
                if (e.R(r + 0x86) == 2) {
                    const uint32_t sid = e.R(en + 4);
                    const uint32_t r3 = rec(e, g, sid);
                    e.W(r3 + 0x7c, e.R(g.a(0x11c268)));
                    e.H(0xd258, 0, 0, 0, 0, 0, 0, kMod | 0x11c264, sid);
                    e.H(0xd2ac, 0, 0, 0, 0, 0, 0, kMod | 0x11c264, sid);
                }
            }
        }
    }
    for (uint32_t n = lst; n; n = e.R(n + 4)) {
        const uint32_t en = e.R(n);
        if (!en) continue;
        e.H(0xd258, 0, 0, 0, 0, 0, 0, kMod | 0x11bf30, en);
        e.H(0x88150); e.Ej(0x88150, en);
        stop(e, en);
        e.W(en, 0);
    }
    e.H(0xd25e, 0, 0, 0, 0, 0, 0, lp);
    const uint32_t m = e.R(g.a(0x11c394));
    const bool quiet = (m == 0 || m == 1) && (e.R(g.a(0xfada4)) || e.R(g.a(0xfadac)));
    bool loud = false;
    for (uint32_t n = e.R(head); n; n = e.R(n + 4)) {
        const uint32_t en = e.R(n);
        if (e.R8(rec(e, g, e.R(en + 4)) + 0x52) && (!e.R(en + 0x45) || e.R(en + 0x4d))) { loud = true; break; }
    }
    if (now > e.R(g.a(0x11bf3c))) {
        const uint32_t a = g.a(0xe1374), b = g.a(0xe1378);
        int32_t v = (int32_t)e.R(a);
        if (quiet) { v -= 5; e.W(a, v < 0 ? 0 : (uint32_t)v); }
        else { v += 5; e.W(a, v > 100 ? 100 : (uint32_t)v); }
        int32_t w = (int32_t)e.R(b);
        if (loud) { w -= 2; e.W(b, w < 0x46 ? 0x46 : (uint32_t)w); }
        else { w += 2; e.W(b, w > 100 ? 100 : (uint32_t)w); }
    }
    e.W(g.a(0x11bf3c), now);
}

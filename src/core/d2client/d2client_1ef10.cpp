// 1.13c D2Client+0x1EF10, written from the disassembly: the Quest Log panel draw. void(void), plain ret.
// Every import thunk and module callee goes through e.H with the registers and stack words the retail call site passes;
// GetTickCount (IAT 0xCEF5C) is e.H(0xCEF5C). Module globals are reached through g.at(rva). The frame keeps retail's
// layout where a callee sees it: the 0x48-byte cel block at S+0x10 (frame +0, cel +0x34), the wrap count at S+0xC and
// the 0x104-byte path buffer at S+0x6C.
#include <cstdint>
#include <cstring>
#include "cl_env.h"
struct D2C1EF10G { void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); };
namespace {
constexpr uint32_t kDraw = 0xd096, kStr = 0xd1ec, kFont = 0xd3a8, kWidth = 0xd35a, kText = 0xd37e, kHover = 0xd384,
    kWrap = 0xd378, kFree = 0xbf86, kExp = 0xbf8c, kQGet = 0xc58c, kQSet = 0xc904, kTick = 0xcef5c;
constexpr uint32_t kFile = 0x6fb86dcc;   // .rdata "..\Source\D2Client\UI\QuestLog.cpp"
// the quest record whose state gates act tabs 1..4
constexpr uint32_t kActQ[5] = { 0, 7, 0xf, 0x17, 0x1a };
struct P {
    const cl::Env& e; const D2C1EF10G& g; uint8_t* F;
    uint32_t A(uint32_t off) const { return (uint32_t)(uintptr_t)(F + off); }
    uint32_t& D(uint32_t off) const { return *(uint32_t*)(void*)(F + off); }
    uint32_t at(uint32_t rva) const { return g.at(g.ctx, rva); }
    uint32_t G(uint32_t rva) const { return e.R(at(rva)); }
    uint32_t G8(uint32_t rva) const { return e.R8(at(rva)); }
    void SetG(uint32_t rva, uint32_t v) const { e.W(at(rva), v); }
    void SetG8(uint32_t rva, uint32_t v) const { const uint32_t a = at(rva); e.W(a, (e.R(a) & ~0xffu) | (v & 0xff)); }
    int32_t X() const { return (int32_t)G(0x11b9a0); }
    int32_t Y() const { return (int32_t)G(0x11b9a4); }
    int32_t YH() const { return Y() + (int32_t)G(0xdbc4c); }
    // D2Gfx cel draw: (block, x, y, -1, mode, 0)
    void draw(int32_t x, int32_t y, uint32_t mode) const { e.H(kDraw, A(0x10), (uint32_t)x, (uint32_t)y, 0xffffffffu, mode); }
    void cel(uint32_t h, uint32_t fr) const { D(0x10 + 0x34) = h; D(0x10) = fr; }
    uint32_t str(uint32_t id) const { return e.H(kStr, 0, 0, 0, 0, 0, 0, id); }
    void font(uint32_t f) const { e.H(kFont, 0, 0, 0, 0, 0, 0, f); }
    void text(uint32_t s, int32_t x, int32_t y) const { e.H(kText, (uint32_t)y, 0, 0, 0, 0, 0, s, (uint32_t)x); }
    void hover(uint32_t s, int32_t x, int32_t y) const { e.H(kHover, (uint32_t)y, 0, 1, 0, 0, 0, s, (uint32_t)x); }
    // the 16-byte slot table at 0xF8100 (x at +0, y at +4)
    int32_t sx(uint32_t slot) const { return (int32_t)G(0xf8100 + slot * 16); }
    int32_t sy(uint32_t slot) const { return (int32_t)G(0xf8104 + slot * 16); }
};
constexpr uint32_t kEnt = 0x1224e9, kEntSz = 0x26a;
}  // namespace
// 1.13c D2Client+0x1EF10
extern "C" void d2c_1ef10(const cl::Env& e, const D2C1EF10G& g) {
    uint8_t F[0x170];
    memset(F, 0, sizeof F);   // only the cel block is zeroed by retail; nothing else here is read before it is written
    P p{ e, g, F };
    const uint32_t pl = p.G(0x11bbfc);
    if (!pl || e.R(pl + 0x10) == 0x11 || p.G(0xfadc8) || p.G8(0x10f2f3)) { e.H(0x1e7e0, 0); return; }
    // header: four quarters of the panel background
    uint32_t bg = p.G(0x122415);
    if (!bg) { e.H(0x1d930); e.H(0x1da90, 0, 1); bg = p.G(0x122415); }
    if (bg) {
        p.D(0x10 + 0x34) = bg;
        p.D(0x10) = 0; p.draw(p.X(), p.YH() - 0xe0, 5);
        p.D(0x10) = 1; p.draw(p.X() + 0x100, p.YH() - 0xe0, 5);
        p.D(0x10) = 2; p.draw(p.X(), p.YH() - 0x30, 5);
        p.D(0x10) = 3; p.draw(p.X() + 0x100, p.YH() - 0x30, 5);
    }
    // the act tabs
    if (const uint32_t tabs = p.G(0x122419)) {
        p.D(0x10 + 0x34) = tabs;
        int32_t* pad = (int32_t*)(void*)(F + 0x58);
        for (uint32_t i = 0; i < 5; ++i) {
            int32_t dy = 0;
            if (e.H(kExp) && p.G(0x119854)) {
                dy = 1; pad[0] = 5; pad[1] = 0x43; pad[2] = 0x81; pad[3] = 0xbf; pad[4] = 0xfd;
            } else {
                pad[0] = 5; pad[1] = 0x52; pad[2] = 0x9f; pad[3] = 0xec;
                if (i > 3) break;
            }
            uint32_t fr;
            if (i == p.G(0x123395)) fr = i * 2;
            else {
                bool on = true;
                if (i >= 1 && i <= 4) {
                    const uint32_t q = e.H(kQGet, p.G(0x11973b), kActQ[i], 0);
                    if (i < 4) on = q != 0;
                    else on = e.H(kExp) && p.G(0x119854) && q != 0;
                }
                if (!on) continue;
                fr = i * 2 + 1;
            }
            p.D(0x10) = fr;
            p.draw(pad[i] + p.X(), p.YH() + dy - 0x1c0, 5);
        }
    }
    for (uint32_t k = 0; k < 6; ++k) for (uint32_t o = 0; o < 0x39f; ++o) e.W(p.at(kEnt + 4 * o), 0);
    uint8_t& gem = F[0xb];
    gem = 0;
    if ((int32_t)p.G(0x123395) < 5) {
        p.SetG(0x1223e0, 0);
        uint32_t n = 0; bool done = false;
        for (uint32_t row = 0xf818a; !done; row += 0x10) {
            const uint32_t cnt = p.G(0x1223e0);
            const uint32_t ico = p.G8(row - 1), slot = p.G8(row);
            if (p.G8(row + 1) == p.G(0x123395) && p.G8(row - 2) && ico < 0x1b && p.G(0x122421 + 4 * ico) && (int32_t)cnt < 6) {
                const uint32_t ent = kEnt + cnt * kEntSz;
                p.SetG(ent + 1, p.G(row + 0xa));
                e.H(0x1ce00, 0, 0, 0, 0, 0, 0, 0, 0, 0, p.at(ent));
                const uint32_t ty = p.G(ent + 0x266);
                int32_t key = -1;
                if (ty <= 3) {
                    const int32_t x = p.sx(slot) + p.X(), y = p.sy(slot) - p.Y();
                    const uint32_t icon = p.G(0x122421 + 4 * ico);
                    if (ty == 0) {
                        p.SetG8(ent, 1);
                        int32_t fr = (int32_t)p.G(0x123365 + 4 * cnt);
                        if (fr >= 0x19) {
                            const uint32_t r = e.H(kQSet, p.G(0x11973b), p.G(row + 0xa), 0xc);
                            fr = 0x18;
                            e.H(0x14a10, 0, 0, 0, 0, 0, (r & ~0xffu) | 0x58, p.G(row + 0xa) & 0xffff);
                        }
                        p.cel(icon, (uint32_t)fr); p.draw(x, y, 5);
                        const uint32_t t = e.H(kTick);
                        if (!p.G(0x12337d + 4 * cnt)) p.SetG(0x12337d + 4 * cnt, t);
                        if (t - p.G(0x12337d + 4 * cnt) > 100) {
                            p.SetG(0x12337d + 4 * cnt, t);
                            p.SetG(0x123365 + 4 * cnt, p.G(0x123365 + 4 * cnt) + 1);
                            if (p.G(0x123365 + 4 * cnt) == 1 && e.H(kExp)) e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 0xe);
                        }
                    } else if (ty == 1) {
                        p.SetG8(ent, 1);
                        if (p.G(0x1223f5) == slot) {
                            p.cel(p.G(0x12241d), ico);
                            p.draw(p.sx(slot) + (int32_t)p.G(0x1234bc) + p.X(), p.sy(slot) - p.Y() + (int32_t)p.G(0xfa3fc), 5);
                        } else { p.cel(icon, 0x18); p.draw(x, y, 5); }
                    } else if (ty == 2) {
                        p.SetG8(ent, 1);
                        p.cel(icon, 0x1a); p.draw(x, y, 5);
                    } else {
                        if ((p.G(ent + 5) & 0xffff) == 0x3a0 && p.G8(ent + 0x265) == 7) gem = 1;
                        p.SetG8(ent, 1);
                        p.cel(icon, p.G(0x1223f5) == slot ? 0x19u : 0u); p.draw(x, y, 5);
                    }
                    p.SetG(0x1223e0, cnt + 1);
                    key = (int32_t)slot;
                }
                // the socket under the icon: lit for the selected quest
                p.D(0x10 + 0x34) = p.G(0x122411);
                int32_t dy;
                if (key == (int32_t)p.G(0x1223f9)) { dy = (int32_t)p.G(0xfa3f8); p.D(0x10) = 1; }
                else { dy = (int32_t)p.G(0x1234b8); p.D(0x10) = 0; }
                p.draw(p.sx(slot) + p.X() - 4, p.sy(slot) - p.Y() + dy + 5, 5);
            }
            ++n;
            if (n > 0x29 || (int32_t)p.G(0x1223e0) >= 6) done = true;
        }
        if (gem && p.G(0x1223f9) == 5) {
            p.D(0x10 + 0x34) = p.G(0x122491);
            const int32_t v = (int32_t)p.G(0x1223e8);
            p.D(0x10) = (v < 0 || v > 6) ? 0u : (uint32_t)v;
            p.draw(p.X() + 0x6e, p.YH() - 0x6e, 3);
        }
    }
    p.font(8);
    // the selected quest's title and wrapped description
    const int32_t sel = (int32_t)p.G(0x1223f9), cnt = (int32_t)p.G(0x1223e0);
    if (sel != -1 && sel < 6 && sel < cnt && cnt > 0) {
        int32_t k = 0;
        while (k < cnt && (int32_t)p.G8(kEnt + (uint32_t)k * kEntSz + 0x261) != sel) ++k;
        if (k < cnt) {
            const uint32_t ent = kEnt + (uint32_t)k * kEntSz;
            if (p.G8(ent)) {
                const uint32_t id = p.G(ent + 5) & 0xffff;
                if (id != 0xe8c) {
                    const uint32_t s = p.str(id);
                    const int32_t y = (int32_t)p.G(0xfa3f0) - p.Y();
                    const int32_t w = (int32_t)e.H(kWidth, 0, 0, 0, 0, 0, 0, s);
                    p.text(s, p.X() - w / 2 + 0xa0, y);
                }
                if (!p.G8(0x1223f3) && (p.G(ent + 7) & 0xffff)) {
                    uint32_t node = e.H(kWrap, 0x10e, 0, 0, 0, 0, 0, p.at(ent + 7), p.A(0xc));
                    for (int32_t dy = 0; node; dy += 0x14) {
                        p.text(e.R(node), p.X() + (int32_t)p.G(0xfa3f4), dy - p.Y() + (int32_t)p.G(0xfa3ec));
                        const uint32_t next = e.R(node + 4);
                        e.H(kFree, 0x8d1, 0, 0, 0, 0, 0, e.R(node), kFile);
                        e.H(kFree, 0x8d2, 0, 0, 0, 0, 0, node, kFile);
                        node = next;
                    }
                }
            }
        }
    }
    // the buy/sell button cel, loaded once
    uint32_t bs = p.G(0x11b9c4);
    if (!bs) {
        strcpy((char*)F + 0x6c, "DATA\\GLOBAL\\ui\\panel\\buysellbtn");   // wsprintfA(buf, "%s\ui\panel\buysellbtn", "DATA\GLOBAL")
        bs = e.H(0x2b420, 0, 0, 0, 0, 0, p.A(0x6c));
        p.SetG(0x11b9c4, bs);
    }
    p.D(0x10 + 0x34) = bs;
    p.D(0x10) = (p.G8(0x1223f2) ? 1u : 0u) + 0xa;
    p.draw(p.X() + 0x116, p.YH() - 0x3a, 5);
    const int32_t mx = (int32_t)p.G(0x11b828) - p.X(), my = (int32_t)p.G(0x11b824) + p.Y();
    if (mx >= 0x116 && mx <= 0x139 && my >= 0x188 && my <= 0x1a9) {
        p.font(9);
        p.hover(p.str(0x1030), p.X() + 0x128, p.YH() - 0x5f);
    }
    p.D(0x10 + 0x34) = p.G(0x12248d);
    p.D(0x10) = p.G8(0x1223f4) ? 1u : 0u;
    p.draw(p.X() + 0xe2, p.YH() - 0x3a, 5);
    if (mx > 0xe5 && mx < 0x104 && my > 0x186 && my < 0x1a8) {
        const uint32_t s = p.str(0xe88);
        p.font(9);
        p.hover(s, p.X() + 0xf1, p.YH() - 0x5f);
    }
}

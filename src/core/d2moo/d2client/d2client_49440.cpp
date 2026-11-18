// 1.13c D2Client+0x49440: build the ten-slot list panel. int(void): 1 when the panel was built, 0 when an open one was closed or a
// step failed.
// An open panel (control at 0x11976F) is closed instead: its list object is destroyed through its vtable, both pointers cleared and
// the NPC menu rebuilt (0x48B20). Otherwise a centred control is created with a title row (string 0xD24 formatted with the sum of
// two player stats) and a close row, a list object is allocated inside it, and every used slot of the ten at 0x11967D (stride 0x10:
// +0 u16 string id, +4 key, +8 used flag) adds one row: the slot's text plus four labelled "%2u" fields from a looked-up record
// (a caller-stack out-struct the lookup fills), and a second line when the record carries one. No used slot adds string 0xD25.
// Every string, control and list call is a callee; the list's own calls go through e.H keyed by the vtable value it reads.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t kT = 0xB2000000u;
constexpr uint32_t kOld = 0x119763, kCtlG = 0x11976F, kListG = 0x11977B, kSlots = 0x11967D, kSrc = 0x119854, kPlayer = 0x11BBFC;
constexpr uint32_t kStr = 0xCEACC, kStrcpy = 0xCEAB8, kStrcat = 0xCEABC, kStrncpy = 0xCEAA8, kSprintf = 0xCEAB4;
constexpr uint32_t kW2U = 0xCEAC0, kWsprintf = 0xCF114, kAddRow = 0xA36F0;
constexpr uint32_t kOnClose = kT | 0x49090, kFmt2u = kT | 0xD4CAC;
uint32_t str(const cl::Env& e, uint32_t id) { return e.H(kStr, 0, 0, 0, 0, 0, 0, id); }
void cat(const cl::Env& e, uint32_t dst, uint32_t src) { e.H(kStrcat, 0, 0, 0, 0, 0, 0, dst, src); }
void fatal(const cl::Env& e, uint32_t line) {                     // error box then exit(-1): not reachable under a vector
    const uint32_t m = e.H(0xBF92, line);
    e.H(0xBF80, kT | 0xD359F, m);
    e.H(0x3664, 0xFFFFFFFFu);
}
void addRow(const cl::Env& e, uint32_t eax, uint32_t text, uint32_t font, uint32_t fn, uint32_t show) {
    volatile uint32_t a[6] = { text, font, 0, 1, fn, show };
    e.Xs(show);
    e.H(kAddRow, text, font, 0, 1, fn, eax, e.R(G(kCtlG)));
    if (e.blk) e.blk(e.ctx, kAddRow, P(a), 6);
}
void listAdd(const cl::Env& e, uint32_t t1, uint32_t t2, uint32_t idx) {   // list vtable +0x30, thiscall, 10 stack words
    const uint32_t obj = e.R(G(kListG));
    const uint32_t fn = e.R(e.R(obj) + 0x30);
    volatile uint32_t a[10] = { t1, t2, 0, 0, 0, 0, idx, 0, 1, 0xE };
    e.Xs(0, idx);
    e.H(fn, t1, t2, 0, 0, 0, 0, obj);
    if (e.blk) e.blk(e.ctx, fn, P(a), 10);
}
}
extern "C" uint32_t d2c_49440(const cl::Env& e) {
    if (e.R(G(kOld))) { e.H(0xA3D20); e.W(G(kOld), 0); }
    if (e.R(G(kCtlG))) {
        e.H(0xA3D20);
        const uint32_t obj = e.R(G(kListG));
        if (obj) e.H(e.R(e.R(obj)), 1, 0, 0, 0, 0, 0, obj);
        e.W(G(kListG), 0);
        e.W(G(kCtlG), 0);
        e.H(0x48B20);
        return 0;
    }
    const int32_t sw = (int32_t)e.R(G(0xDBC48)), sh = (int32_t)e.R(G(0xF9E18));
    const uint32_t x = (uint32_t)((sw - 0x1EA) / 2);
    {
        const uint32_t y = (uint32_t)(sh / 2 - 0xC3);
        volatile uint32_t ca[11] = { x, y, kOnClose, 0, 0, 0, 0, 0x1EA, 0x15E, 0, 1 };
        e.Xs(0, 0);
        const uint32_t ctl = e.H(0xA3E40, x, y, kOnClose, 0, 0);
        if (e.blk) e.blk(e.ctx, 0xA3E40, P(ca), 11);
        e.W(G(kCtlG), ctl);
        if (!ctl) { fatal(e, 0x529); return 0; }
        e.W(ctl + 0x5C, 1);
    }
    {
        volatile uint16_t title[300] = {};                           // Unicode ctor: each char := 0
        const uint32_t a = e.H(0xC1C6, e.R(G(kPlayer)), 0xF, 0);
        const uint32_t b = e.H(0xC1C6, e.R(G(kPlayer)), 0xE, 0);
        const uint32_t fmt = str(e, 0xD24);
        volatile uint32_t sa[4] = { 0x12C, P(title), fmt, a + b };
        e.H(kSprintf, sa[0], sa[1], sa[2], sa[3]);
        if (e.blk) e.blk(e.ctx, kSprintf, P(sa), 4);
        addRow(e, 4, P(title), 0x15, 0, 0);
        addRow(e, 0, str(e, 0xD48), 0x13B, kOnClose, 1);
    }
    const uint32_t mem = e.H(0xBE66, 0xC, kT | 0xD04F0, 0xFFFFFFFFu, 0);
    uint32_t obj = 0;
    if (mem) {
        const uint32_t y = (uint32_t)(sh / 2 - 0xA0);
        volatile uint32_t oa[10] = { mem, x, y, 0x1EA, 0x118, 6, 0, 1, kT | 0x47860, 0 };
        e.Xs(6, 0);
        obj = e.H(0x67F20, mem, x, y, 0x1EA, 0x118, 0, mem);
        if (e.blk) e.blk(e.ctx, 0x67F20, P(oa), 10);
    }
    e.W(G(kListG), obj);
    if (!obj) { e.H(0x49090); return 0; }
    e.H(0xA3400, obj, 0, 0, 0, 0, 0, 0, 0, 0, e.R(G(kCtlG)));
    uint32_t count = 0;
    for (uint32_t i = 0; i < 10; ++i) {
        const uint32_t slot = G(kSlots) + 16 * i;
        if (!e.R(slot + 8)) continue;
        const uint32_t id = e.R(slot) & 0xFFFF;
        const uint32_t k = e.H(0xCD8A, id, 0, 0, 0, 0, 0, e.R(G(kSrc)), 0);
        volatile uint32_t out[15] = {};                              // filled by the lookup
        const uint32_t mode = e.R8(G(0x11C390));
        if (!e.H(0xCD7E, e.R(slot + 4), k, mode, P(out), 0, 0, e.R(G(kSrc)), e.R(G(kPlayer)))) return 0;
        const uint32_t field[4][2] = { { 0xD28, out[1] }, { 0xD26, out[2] }, { 0xD27, out[7] }, { 0xD29, out[5] } };
        const uint32_t v5 = out[12];
        volatile uint16_t b1[150] = {}, b2[150] = {};
        e.H(kStrcpy, 0, 0, 0, 0, 0, 0, P(b1), str(e, id == 0x421 ? 0x2B0D : id));
        cat(e, P(b1), str(e, 0xF9B));
        cat(e, P(b1), str(e, 0xF9C));
        cat(e, P(b1), str(e, 0xF9B));
        volatile uint16_t s20[20] = {}, s10[10] = {};
        volatile char cb[12] = {};
        for (uint32_t f = 0; f < 4; ++f) {
            e.H(kStrncpy, 0x14, 0, 0, 0, 0, 0, P(s20), str(e, field[f][0]));
            cat(e, P(b1), P(s20));
            cat(e, P(b1), str(e, 0xF9D));
            cat(e, P(b1), str(e, 0xF9B));
            e.H(kWsprintf, P(cb), kFmt2u, field[f][1]);
            e.H(kW2U, 9, 0, 0, 0, 0, 0, P(s10), P(cb));
            cat(e, P(b1), P(s10));
            const uint32_t dst = f < 3 ? P(b1) : P(b2);
            cat(e, dst, str(e, 0xF9B));
            cat(e, dst, str(e, 0xF9B));
        }
        if (e.H(0xCE26, 0, 0, 0, 0, 0, 0, v5)) {
            volatile uint16_t b3[150] = {};
            volatile uint16_t fmt[6] = { 'a', '0', ':', '%', '0', 0 };
            const uint32_t c2 = e.H(0xCE26, 0, 0, 0, 0, 0, 0, v5);
            e.H(0x90CB0, P(b3), P(fmt), c2, 0);
            if (e.blk) e.blk(e.ctx, 0x90CB0, P(fmt), 3);
            cat(e, P(b2), P(b3));
        }
        listAdd(e, P(b1), P(b2), i);
        ++count;
    }
    if (!count) listAdd(e, str(e, 0xD25), 0, 10);
    const uint32_t ctl = e.R(G(kCtlG));
    if (!ctl) { fatal(e, 0x4F2); return 0; }
    if ((int32_t)e.R(ctl + 0x50) <= -1) fatal(e, 0x4F8);
    e.W(ctl + 0x44, 0xFFFFFFFFu);
    return 1;
}

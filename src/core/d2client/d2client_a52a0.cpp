// 1.13c D2Client+0xA52A0 and +0xA4B00 in retail's register form, every callee through FnCallees.
#include "d2client_batch_a3040.h"
#include "d2client_batch_a4b60.h"
namespace d2client_batch_a3040 {
// 1.13c D2Client+0xA4B00: eax = obj (null is the fatal 0xA4A), [obj+0x68] = v
void sub_A4B00(FnCallees& s, uint32_t obj, uint32_t v) {
    if (!obj) s.fatal(0xa4a);
    fnAt<uint32_t>(fnPtr(obj), 0x68) = v;
}
}
namespace d2client_batch_a4b60 {
// 1.13c D2Client+0xA52A0: walk both 0x80-slot chains of the table at 0x10A608 (next at +0xE4);
// 0xA4A90(EAX = unit, ECX = b, EDI = target) non-zero gives 1
uint32_t sub_A52A0(FnCallees& s, uint32_t target, uint32_t b) {
    const uint32_t* tab = s.g(0x10a608);
    for (int t = 0; t < 2; t++)
        for (int i = 0; i < 0x80; i++)
            for (uint32_t u = tab[t * 0x80 + i]; u; u = fnAt<uint32_t>(fnPtr(u), 0xe4)) {
                const uint32_t r = s.h(0xa4a90, u, b);
                s.edi(0xa4a90, target);
                if (r) return 1;
            }
    return 0;
}

// the unit position pair as retail inlines it: types 2, 4, 5 read the path record (+0xC x, +0x10 y); every other type asks
// 0xC3BE (x) / 0xC2EC (y) with the record, 0 when the record is null
static int32_t unitX(FnCallees& s, uint32_t u) {
    const uint32_t t = fnAt<uint32_t>(fnPtr(u), 0), p = fnAt<uint32_t>(fnPtr(u), 0x2c);
    if (t == 2 || (t > 3 && t < 6)) return fnAt<int32_t>(fnPtr(p), 0xc);
    return p ? (int32_t)s.h(0xc3be, 0, 0, 0, 0, 0, p) : 0;
}
static int32_t unitY(FnCallees& s, uint32_t u) {
    const uint32_t t = fnAt<uint32_t>(fnPtr(u), 0), p = fnAt<uint32_t>(fnPtr(u), 0x2c);
    if (t == 2 || (t > 3 && t < 6)) return fnAt<int32_t>(fnPtr(p), 0x10);
    return p ? (int32_t)s.h(0xc2ec, 0, 0, 0, 0, 0, p) : 0;
}
// 0xC2F8(&x, &y) rewrites the pair in place; the box test is retail's four signed compares
static bool inBox(int32_t px, int32_t py, int32_t l, int32_t t, int32_t w, int32_t h) {
    return !(px < l || py < t || px > w + l || py > h + t);
}

// 1.13c D2Client+0xA5360: eax = callback (null is the fatal 0x977); 0xA4DA0(table 0x10AA08) with EBX = callback
void sub_A5360(FnCallees& s, uint32_t cb) {
    if (!cb) s.fatal(0x977);
    s.h(0xa4da0, 0, 0, 0, cb, 0, s.mod(0x10aa08));
}
// 1.13c D2Client+0xA5540: esi = unit; the current unit (0x11BBFC) in mode 1 or 5 is steered at the unit position
void sub_A5540(FnCallees& s, uint32_t unit) {
    const uint32_t c = *s.g(0x11bbfc);
    if (!c) return;
    const uint32_t k = fnAt<uint32_t>(fnPtr(c), 0x10);
    if (k != 1 && k != 5) return;
    const uint32_t a = fnAt<uint32_t>(fnPtr(c), 0x2c);
    if (!a) s.fatal(0x802);
    const int32_t y = unitY(s, unit);
    const int32_t x = unitX(s, unit);
    s.h(0xc478, 0, 0, 0, 0, 0, a, (uint32_t)x, (uint32_t)y, 0);
}
// 1.13c D2Client+0xA5600: eax = unit with a path (null is the fatal 0x7f3); 0xC43C(unit, 1), then 0xC478 at the current unit
void sub_A5600(FnCallees& s, uint32_t unit) {
    const uint32_t a = fnAt<uint32_t>(fnPtr(unit), 0x2c);
    const uint32_t c = *s.g(0x11bbfc);
    if (!a) s.fatal(0x7f3);
    s.h(0xc43c, 0, 0, 0, 0, 0, unit, 1);
    const int32_t y = unitY(s, c);
    const int32_t x = unitX(s, c);
    s.h(0xc478, 0, 0, 0, 0, 0, a, (uint32_t)x, (uint32_t)y, 0);
}
// 1.13c D2Client+0xA5720: eax = unit, ebx = x, edi = rect, stack y; the unit position goes through 0xC2F8 and is tested
// against the rect (+0x1A0 left, +0x1A4 top, +0x1A8 width, +0x1AC height)
uint32_t sub_A5720(FnCallees& s, uint32_t unit, int32_t x, uint32_t rect, int32_t y) {
    int32_t px = unitX(s, unit);
    int32_t py = unitY(s, unit);
    s.h(0xc2f8, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&px, (uint32_t)(uintptr_t)&py);
    const int32_t l = fnAt<int32_t>(fnPtr(rect), 0x1a0) + px, t = fnAt<int32_t>(fnPtr(rect), 0x1a4) + py;
    return inBox(x, y, l, t, fnAt<int32_t>(fnPtr(rect), 0x1a8), fnAt<int32_t>(fnPtr(rect), 0x1ac));
}
// 1.13c D2Client+0xA57E0: eax = unit (not type 5 is the fatal 0x58b), ebx = x, stack y; the box 0xC1E4(0xC1F0(unit), unit)
// is offset by the unit position after 0xC2F8
uint32_t sub_A57E0(FnCallees& s, uint32_t unit, int32_t x, int32_t y) {
    if (fnAt<uint32_t>(fnPtr(unit), 0) != 5) s.fatal(0x58b);
    const uint32_t e = s.h(0xc1f0, 0, 0, 0, 0, 0, unit);
    const uint32_t r = s.h(0xc1e4, 0, 0, 0, 0, 0, e, unit);
    if (!r) return 0;
    int32_t px = unitX(s, unit);
    int32_t py = unitY(s, unit);
    s.h(0xc2f8, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&px, (uint32_t)(uintptr_t)&py);
    const int32_t l = fnAt<int32_t>(fnPtr(r), 4) + px, t = fnAt<int32_t>(fnPtr(r), 8) + py;
    return inBox(x, y, l, t, fnAt<int32_t>(fnPtr(r), 0xc), fnAt<int32_t>(fnPtr(r), 0x10));
}
// 1.13c D2Client+0xA59E0: esi = unit, bl = old state, stack byte = new state; a player unit whose state changed asks 0xC1F0 for its
// record, which goes to 0xC6CA (new is 2) or 0xC694 (old is 2); 0xC502(unit) is the tail call
void sub_A59E0(FnCallees& s, uint32_t unit, uint8_t b, uint8_t c) {
    if (!unit || fnAt<uint32_t>(fnPtr(unit), 0) != 1 || b == c) return;
    const uint32_t r = s.h(0xc1f0, 0, 0, 0, 0, 0, unit);
    if (!r) return;
    if (c == 2) s.h(0xc6ca, 0, 0, 0, 0, 0, r);
    else if (b == 2) s.h(0xc694, 0, 0, 0, 0, 0, r);
    s.h(0xc502, 0, 0, 0, 0, 0, unit);
}
}

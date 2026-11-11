// 1.13c D2Client+0x7DFB0: chat/system message handler. stdcall(msg), ret 4; ECX unused.
// msg: +1 kind (0..0x12, larger = nothing), +2 colour/channel byte, +3 dword id, +7 sub-kind, +8 name (cut to 15 chars),
// +0x18 second string. Three Unicode locals are built (out[0x132], name[0x10], third[0x10]; a fourth[0x10] in kinds 2/3);
// the name is widened into `name`, the text composed into `out` from D2Lang strings (0xD1EC = D2Lang 10003, ECX = id) and
// handed to 0x7D850(out, msg[2]) (kind 7: 0x7CFC0/0x7D610). Every D2Lang Unicode import goes through h() keyed by its IAT
// slot with the registers the call site loads. The 0x104 length checks fall into the fatal path (0xBF92/0xBF80/0x3664)
// and then, as retail's code falls through when exit returns, into kind 5's tail; the port keeps that layout.
#include <cstdint>
#include "cl_env.h"
namespace {
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
constexpr uint32_t kStrlen = 0xCEAB0, kSprintf = 0xCEAB4, kStrcpy = 0xCEAB8, kStrcat = 0xCEABC, kWin2U = 0xCEAC0, kTick = 0xCEF5C;
struct Ctx {
    const cl::Env& e; uint32_t m;
    volatile uint16_t out[0x132], name[0x10], third[0x10], fourth[0x10];
    uint32_t str(uint32_t id) const { return e.H(0xD1EC, 0, 0, 0, 0, 0, 0, id & 0xFFFFu); }
    uint32_t len(uint32_t p) const { return e.H(kStrlen, 0, 0, 0, 0, 0, 0, p); }
    void w2u(volatile uint16_t* d, uint32_t s) const { e.H(kWin2U, 0x10, 0, 0, 0, 0, 0, P(d), s); }
    void cpy(uint32_t s) const { e.H(kStrcpy, 0, 0, 0, 0, 0, 0, P(out), s); }
    void cat(uint32_t s) const { e.H(kStrcat, 0, 0, 0, 0, 0, 0, P(out), s); }
    void spr(uint32_t fmt, uint32_t a3 = 0, uint32_t a4 = 0) const { e.H(kSprintf, 0x132, P(out), fmt, a3, a4); }
    uint32_t b(uint32_t o) const { return e.R8(m + o); }
    bool empty(uint32_t o) const { return b(o) == 0; }
};
}
extern "C" void d2c_7dfb0(uint32_t msg, const cl::Env& e) {
    Ctx c{ e, msg, {}, {}, {}, {} };
    { uint32_t n = 0; while (c.b(8 + n)) ++n;
      if (n >= 0xF) { const uint32_t a = msg + 0x14; e.W(a, e.R(a) & 0x00FFFFFFu); } }
    const uint32_t kind = c.b(1);
    if (kind > 0x12) return;
    const uint32_t nameSrc = msg + 8;
    uint32_t tailId = 0;                                   // e51d: strcat(out, str(id)) then 0x7D850
    auto lenCheck = [&](uint32_t id, uint32_t line) -> bool {   // false: the fatal path ran (falls into kind 5's tail)
        if ((int32_t)c.len(c.str(id)) < 0x104) return true;
        const uint32_t f = e.H(0xBF92);
        e.H(0xBF80, G(0xD359F), f, line);
        e.H(0x3664, 0xFFFFFFFFu);
        return false;
    };
    auto nameTail = [&](uint32_t id) { c.w2u(c.name, nameSrc); c.cpy(P(c.name)); tailId = id; };
    switch (kind) {
    case 0x0: if (lenCheck(0xE37, 0x113A)) nameTail(0xE37); else nameTail(0xE3E); break;
    case 0x1: if (lenCheck(0xE38, 0x1140)) nameTail(0xE38); else nameTail(0xE3E); break;
    case 0x4: if (lenCheck(0xE3D, 0x1166)) nameTail(0xE3D); else nameTail(0xE3E); break;
    case 0x5: lenCheck(0xE3E, 0x1172); nameTail(0xE3E); break;
    case 0xD: if (lenCheck(0xE44, 0x116C)) nameTail(0xE44); else nameTail(0xE3E); break;
    case 0x8: nameTail(0xFCC); break;
    case 0xA:
        if (c.empty(8)) { c.cpy(c.str(0x1027)); break; }
        nameTail(0x1026); break;
    case 0x9: c.cpy(c.str(0xFCF)); break;
    case 0x10: c.cpy(c.str(0x2A38)); break;
    case 0x2: case 0x3: {
        if (kind == 2 && e.R(G(0x11BBFC))) {
            const uint32_t s = e.H(0x9D850, 0, 0, 0, 0, 0, 0, 0, 0, 0, e.R(G(0x11BBFC)));   // ESI = player: its name
            uint32_t i = 0; while (c.b(8 + i) == e.R8(s + i) && c.b(8 + i)) ++i;
            if (c.b(8 + i) == e.R8(s + i)) break;      // the sender is the local player: 0x7D850 with an empty text
        }
        const uint32_t id = kind == 2 ? 0xE39u : 0xE3Au;
        if (!lenCheck(id, kind == 2 ? 0x114Au : 0x1158u)) { nameTail(0xE3E); break; }
        c.w2u(c.name, nameSrc);
        if (!c.empty(0x18)) {
            c.w2u(c.fourth, msg + 0x18);
            const uint32_t fmt = c.str(kind == 2 ? 0xE3Bu : 0xE3Cu);
            c.spr(fmt, P(c.name), P(c.fourth));
        } else c.spr(c.str(id), P(c.name));
        break;
    }
    case 0x6: {
        c.w2u(c.name, nameSrc);
        const uint32_t sub = c.b(7);
        uint32_t id = 0, s = 0; bool ok = false;
        if (sub == 2) {
            const uint32_t r = e.H(0xC226, e.R(msg + 3));
            if (r) { id = 0xE3F; s = e.R(msg + 3) == 0xB ? c.str(0xC98) : r + 0x40; ok = true; }
        } else if (sub == 1) {
            if (!c.empty(0x18)) {
                const uint32_t r = e.H(0xC604, e.R(msg + 0x18) & 0xFFFFu);
                if (r) { id = 0xE40; s = c.str(e.R(r + 2) & 0xFFFFu); ok = true; }
            } else {
                const uint32_t r = e.H(0x1230, 0, 0, 0, 0, 0, e.R(msg + 3));
                if (r) { id = 0xE3F; s = c.str(e.R(r + 6) & 0xFFFFu); ok = true; }
            }
        } else if (sub == 0) {
            if (!c.empty(0x18)) { c.w2u(c.third, msg + 0x18); id = 0xE40; s = P(c.third); ok = true; }
        }
        if (!ok) { c.cpy(P(c.name)); tailId = 0xE41; break; }
        const int32_t l1 = (int32_t)c.len(c.str(id));
        const int32_t l2 = (int32_t)c.len(s);
        if (l1 + l2 + 0x10 >= 0x132) return;
        c.cpy(P(c.name));
        c.cat(c.str(id));
        c.cat(s);
        break;
    }
    case 0x7:
        if (!e.H(0x7CFC0, msg, 0, 0, 0, 0, P(c.out))) return;
        e.H(0x7D610, P(c.out), c.b(2));
        return;
    case 0xB: c.w2u(c.name, nameSrc); c.spr(c.str(0x69B), P(c.name)); break;
    case 0xF: c.spr(c.str(0xE43), e.R(msg + 3)); break;
    case 0x11: c.spr(c.str(0x2AFC), e.R(msg + 3)); break;
    case 0x12:
        c.spr(c.str(0x2AFD), P(&c));                       // 3 pushed args; the next stack word is the saved EBP (a caller stack address)
        if (e.R(G(0x11C3B8))) { const uint32_t r = e.H(0xA5480, 5, 0, 1); e.H(0xC556, e.R(G(0x11C3B8)), r); }
        e.W(G(0x10B9CC), e.H(kTick));
        e.W(G(0x10B9D0), 0xFA0); e.W(G(0x10B9D4), 0x2710); e.W(G(0x10B9D8), 0xFA0);
        e.W(G(0x11BEFC), 6);
        e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 0x1220);
        break;
    default: return;                                       // 0xC, 0xE
    }
    if (tailId) c.cat(c.str(tailId));
    e.H(0x7D850, P(c.out), c.b(2));
}

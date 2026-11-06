// src/core/d2game/d2game_batch_71b00.cpp -- 1.13c D2Game.dll 0x71B00, from the disassembly (the decompile loses the register arguments
// of the callees and the packet bytes).
//
// Retail convention: ECX = game, EAX = unit (the acting unit), stack word 0 = rec (ret 4). rec: +0 type, +0x64 id, +0x68 flag word,
// +0x6C flag byte. The function finds the entry `it` with that id in the game's 128-bucket table (+0x1120, next at +0xE4, id at +0xC);
// a flagged rec or a missing entry sends the two-byte error packet {0x77, 0x0C} to rec's client. Otherwise it resolves the helper unit
// (0x107E0) and the unit's two owner words, asks 0xD76A0 (restore) and either sends four six-byte {0x79, 0|1, 0, 0, 0, 0} packets
// (rec twice, it twice) when either side's block has +0x50 == 4 and calls 0x70B70, or (D76A0 returned 0) tears down through
// 0xDFC10 / 0xE20A0 / 0x719A0. Every callee is a Env seam keyed by its D2Game RVA; pointer fields are read through Env::ptr.
// A callee's register arguments are named as retail passes them. The game == 0 path calls Fog 10265/10024 and exits: Env::fatal.
#include <cstdint>
#include <cstring>
namespace d2game_batch71b00 {
using U = uint32_t; using I = int32_t; using P = void*;
static U r(P p, U o) { U v; std::memcpy(&v, (uint8_t*)p + o, 4); return v; }
static P pw(U v) { return (P)(uintptr_t)v; }
struct Env {
    P (*ptr)(P, U);                                  // the 32-bit pointer field at +o
    U (*pd)(P unit);                                 // 0xA4C0: stack (unit), ret 4: the unit's data block (+0x50, +0x9C)
    U (*helper)(P game, P unit);                     // 0x107E0: EAX = game, stack (unit, 0), ret 8: the helper unit or 0
    U (*own454)(P owner);                            // 0xA454: stack (owner), ret 4
    U (*own3b8)(P owner);                            // 0xA3B8: stack (owner), ret 4
    void (*a4a2)(U unit, U n);                       // 0xA4A2: stack (unit, 4), ret 8
    void (*a92e)(U unit, U n);                       // 0xA92E: stack (unit, 1), ret 8
    U (*d76a0)(P game, P it, U id, U w18, U w10);    // 0xD76A0: EDX = 0xCAA, stack (game, it, id, w18, w10, 0, 1, 0), ret 0x20
    U (*a9ee)(P unit);                               // 0xA9EE: stack (unit), ret 4
    U (*a490)(U v);                                  // 0xA490: stack (v), ret 4
    void (*f5080)(P game, P it, U helper, P unit);   // 0xD5080: EAX = unit, stack (game, it, helper, 1), ret 0x10
    void (*ffc10)(P game, U helper);                 // 0xDFC10: EDI = game, EBX = 0x6FC2E7E0, stack (helper), ret 4
    void (*d20a0)(P game, U helper);                 // 0xE20A0: ECX = game, EDX = helper, plain ret
    void (*f919a0)(P game, P rec, P it, U pdRec, U pdIt); // 0x719A0: EAX = game, ESI = rec, stack (it, pdRec, pdIt, 9), ret 0x10
    void (*f8ffc0)(P rec, P game, U unitId, U helperId);  // 0x6FFC0: EAX = rec, stack (game, unitId, helperId), ret 0xC
    void (*f90b70)(P rec, U pdRec, U pdIt);          // 0x70B70: EAX = rec, stack (pdRec, pdIt), ret 8
    void (*send)(U client, const uint8_t* buf, U n); // 0x8A3E0: EAX = client, stack (buf, n), ret 8
    void (*fatal)();                                 // Fog 10265 / 10024, then exit(-1)
};

// 1.13c D2Game+0x71B00
inline void sub_71B00(Env& e, P game, P unit, P rec) {
    auto client = [&](P u) -> U {                    // type 0: the data block's +0x9C; else 0
        if (r(u, 0) == 0) return r(pw(e.pd(u)), 0x9C);
        return 0;
    };
    auto fail = [&]() { const uint8_t pkt[2] = {0x77, 0x0C}; e.send(client(rec), pkt, 2); };
    const U id = r(rec, 0x64);
    if ((uint8_t)r(rec, 0x6C) != 0 && r(rec, 0x68) != 0) { fail(); return; }
    if (!game) { e.fatal(); return; }
    P it = pw(r(game, 0x1120 + (id & 0x7F) * 4));
    for (;;) {
        if (!it) { fail(); return; }
        if (r(it, 0xC) == id) break;
        it = pw(r(it, 0xE4));
    }
    P helper = pw(e.helper(game, unit));
    const I t = I(r(unit, 0));
    U w18, w10;
    if (t == 2 || (t > 3 && t <= 5)) { P o = e.ptr(unit, 0x2C); w18 = r(o, 0xC); w10 = r(o, 0x10); }
    else {
        P o = e.ptr(unit, 0x2C);
        w18 = o ? e.own454(o) : 0;
        w10 = o ? e.own3b8(o) : 0;
    }
    e.a4a2((U)(uintptr_t)helper, 4);
    e.a92e((U)(uintptr_t)helper, 1);
    const U hid = helper ? r(helper, 0xC) : 0xFFFFFFFFu;
    if (e.d76a0(game, it, hid, w18, w10) == 0) {
        e.ffc10(game, (U)(uintptr_t)helper);
        e.d20a0(game, (U)(uintptr_t)helper);
        const U pr = e.pd(rec), pi = e.pd(it);
        e.f919a0(game, rec, it, pr, pi);
        return;
    }
    if (r(unit, 0) == 4 && e.a9ee(unit) != 0) {
        const U v = r(unit, 0x60);
        if (v && e.a490(v) != 0) e.f5080(game, it, (U)(uintptr_t)helper, unit);
    }
    e.f8ffc0(rec, game, r(unit, 0xC), hid);
    const U pr = e.pd(rec), pi = e.pd(it);
    if (r(pw(pr), 0x50) == 4 || r(pw(pi), 0x50) == 4) {
        const uint8_t p0[6] = {0x79, 0, 0, 0, 0, 0}, p1[6] = {0x79, 1, 0, 0, 0, 0};
        e.send(client(rec), p0, 6);
        e.send(client(rec), p1, 6);
        e.send(client(it), p0, 6);
        e.send(client(it), p1, 6);
        e.f90b70(rec, pr, pi);
    }
}
}

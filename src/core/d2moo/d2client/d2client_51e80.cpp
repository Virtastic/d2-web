// 1.13c D2Client+0x51E80 -- the hover resolver: no inputs. g = a pinned globals block (+0 busy [0x11C2F8], +4 type-4 gate [0x11C2F0], +8 playerSelf
// [0x11BBFC], +0xC last room [0x12347C] (read, written), +0x10 room-dirty [0x123480] (written; 0xD1470 also gets its address in EDI), +0x20 the
// unit-type priority table [0xF2190], 8 dwords). Picks the unit under the cursor and hands it to 0x51860.
//  1. busy: return. 0x51D10(EAX=&py, ECX=&px) non-zero: return. P = 0x51A80() (the already-targeted unit). If P passes the cursor test 0xA68E0(P,px,py,1)
//     and is not a type-4 unit under the gate, it wins: 0x51860(P), return.
//  2. head = self, room = 0xC1F0(head); either missing: return. A changed room sets the dirty word; the room is stored.
//  3. cx = 0x1630(head), cy = 0x1660(head) converted in place by 0xC2F8(&cx,&cy); 0xC3DC(room,&rooms,&count) lists the neighbouring rooms.
//  4. Per room (u = [room+0x74]), per unit along [u+0xE8]: skip type 3; a unit equal to the last one ends the room (a self-loop guard); a count past
//     0x7530 is fatal (0xBF92(0x218), 0xBF80, 0x3664; never vectored); skip head; skip when 0xD1470 says so, when a type-4 unit meets the gate or
//     when the cursor test fails; P that is not 0xa5440-blocked wins outright (best = P, next room). Otherwise the first candidate is taken and
//     later ones compete on the table priority (type 0 that is 0xa5440 raises to 1, type 1 that is 0xC538 drops to -8 and a 0xC1FC(u,0x8F) unit is
//     skipped, a best of type 1 that is 0xC538 drops to -8): lower best priority loses to higher. A type-2 candidate against a non-2 best needs
//     0xCD00(u) and 0x51230(u,best) to agree; a non-2 candidate against a type-2 best with 0xCD00(best) is vetoed by 0x512E0(best,u) or else wins
//     outright; everything else: equal priority falls to the 0x510B0 distance (smaller wins).
//  5. 0x51860(best) with best possibly 0.
#include "cl_env.h"
extern "C" void d2c_51e80(const cl::Env& e, uint32_t g) {
    if (e.R(g)) return;
    volatile uint32_t loc[2] = { 0, 0 };   // [0] = py (EAX), [1] = px (ECX)
    if (e.H(0x51d10, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)&loc[0], (uint32_t)(uintptr_t)&loc[1])) return;
    uint32_t py = loc[0], px = loc[1];
    uint32_t P = e.H(0x51a80);
    if (P && e.H(0xa68e0, P, px, py, 1) && !(e.R(P) == 4 && e.R(g + 4))) { e.H(0x51860, 0, 0, 0, 0, 0, P); return; }
    uint32_t head = e.R(g + 8);
    if (!head) return;
    uint32_t room = e.H(0xc1f0, head);
    if (!room) return;
    if (e.R(g + 0xc) != room) e.W(g + 0x10, 1);
    e.W(g + 0xc, room);
    volatile uint32_t cc[2];
    cc[0] = e.H(0x1630, 0, 0, 0, 0, 0, 0, head);
    cc[1] = e.H(0x1660, 0, 0, 0, 0, 0, 0, head);
    e.H(0xc2f8, (uint32_t)(uintptr_t)&cc[0], (uint32_t)(uintptr_t)&cc[1]);
    uint32_t cx = cc[0], cy = cc[1];
    volatile uint32_t rl[2] = { 0, 0 };   // [0] = rooms array, [1] = count
    e.H(0xc3dc, room, (uint32_t)(uintptr_t)&rl[0], (uint32_t)(uintptr_t)&rl[1]);
    uint32_t arr = rl[0], n = rl[1], best = 0;
    for (uint32_t i = 0; i < n; i++) {
        uint32_t u = e.R(e.R(arr + 4 * i) + 0x74), last = 0;
        int32_t cnt = 0;
        while (u) {
            uint32_t t = e.R(u);
            bool adv = false;
            if (t == 3) adv = true;
            else {
                if (last == u) break;
                int32_t old = cnt++;
                last = u;
                if (old > 0x7530) {
                    uint32_t m = e.H(0xbf92, 0x218);
                    e.H(0xbf80, 0xd359f, m);
                    e.H(0x3664, 0xffffffffu);
                    return;
                }
                if (u == head) adv = true;
                else if (e.H(0x51470, head, cy, px, py, 0, 0, cx, 0, 0, u)) adv = true;
                else if (t == 4 && e.R(g + 4)) adv = true;
                else if (!e.H(0xa68e0, u, px, py, 1)) adv = true;
                else if (u == P && !e.H(0xa5440, 0, 0, 0, 0, 0, u)) { best = u; break; }
                else {
                    bool take = false;
                    if (!best) take = true;
                    else {
                        uint32_t tb = e.R(best);
                        int32_t pu = (int32_t)e.R(g + 0x20 + 4 * t), pb = (int32_t)e.R(g + 0x20 + 4 * tb);
                        if (t == 0) { if (e.H(0xa5440, 0, 0, 0, 0, 0, u)) pu = 1; }
                        else if (t == 1) {
                            if (e.H(0xc538, u)) pu = -8;
                            if (e.H(0xc1fc, u, 0x8f)) adv = true;
                        }
                        if (!adv) {
                            if (tb == 1 && e.H(0xc538, best)) pb = -8;
                            if (t == 2 && tb != 2) {
                                if (e.H(0xcd00, u) && !e.H(0x51230, best, 0, 0, 0, 0, 0, 0, 0, 0, u)) adv = true;
                                else take = pb < pu;
                            } else if (t != 2 && tb == 2) {
                                if (e.H(0xcd00, best)) { if (!e.H(0x512e0, u, 0, 0, 0, 0, 0, 0, 0, 0, best)) adv = true; else take = true; }
                                else take = pb < pu;
                            } else if (pb != pu) take = pb < pu;
                            else {
                                int32_t d1 = (int32_t)e.H(0x510b0, 0, 0, 0, 0, 0, head, 0, 0, u);
                                int32_t d2 = (int32_t)e.H(0x510b0, 0, 0, 0, 0, 0, head, 0, 0, best);
                                take = d1 < d2;
                            }
                        }
                    }
                    if (take && !adv) best = u;
                }
            }
            u = e.R(u + 0xe8);
        }
    }
    e.H(0x51860, 0, 0, 0, 0, 0, best);
}

// D2Client+0x6D1C0 -- gate [11C04C] := 1, walk (eax 0), stamp the tick; the second walk (eax 1) runs when the gate is set (always, here) or 600 ms have passed; stamp; gate := 0.
#include "d2client_6d1c0.h"

// 1.13c D2Client+0x6D1C0
void d2c_6d1c0(const D2Client6d1c0Env& e) {
    e.setGate(1);
    d2c_6d010(e.walk, 0);
    e.setLast(e.tick());
    if (e.getGate() == 0 && e.tick() < e.getLast() + 0x258) { e.setGate(0); return; }   // retail: jne skips the clock read when the gate is set
    d2c_6d010(e.walk, 1);
    e.setLast(e.tick());
    e.setGate(0);
}

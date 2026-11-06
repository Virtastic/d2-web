// 1.13c D2Client+0x17F70: monster-AI tick, EAX = unit (register convention; Ghidra's constant propagation hid the class switch, see the disassembly).
// unit must be type 1 with a data block (unit+0x14); the delay dword at data+0x30 counts down while > 0. Then 0x16D80(unit) picks a target (distance
// through EDI, preset 0x7FFFFFFF); no target and no player (D2Client+0x11BBFC): delay := 0x19, idle 0x17270. A target of 0 falls back to the player.
// distance > 30: 0x16FE0(EAX=unit, ECX=distance-15), idle. 0x1760(EAX=class, EDX=0x16) nonzero: 0x17710(EAX=unit). Else class dispatch (below).
#include "cl_env.h"
extern "C" void d2c_17f70(uint32_t unit, const cl::Env& e) {
    if (!unit || e.R(unit) != 1) return;
    const uint32_t data = e.R(unit + 0x14);
    if (!data) return;
    const uint32_t delay = data + 0x30;
    const int32_t t = (int32_t)e.R(delay);
    if (t > 0) { e.W(delay, (uint32_t)(t - 1)); return; }
    int32_t dist = 0x7fffffff;
    e.Ed((uint32_t)(uintptr_t)&dist);
    uint32_t target = e.H(0x16d80, unit);
    if (!target && !e.self) {
        if (e.R(unit) == 1 && e.R(unit + 0x14)) e.W(delay, 0x19);
        e.H(0x17270);
        return;
    }
    if (!target) target = e.self;
    if (dist > 30) { e.H(0x16fe0, 0, 0, 0, 0, 0, unit, (uint32_t)(dist - 15)); e.H(0x17270); return; }
    const uint32_t cls = e.R(unit + 4);
    if (e.H(0x1760, 0, 0, 0, 0, 0, cls, 0, 0x16)) { e.H(0x17710, 0, 0, 0, 0, 0, unit); return; }
    auto idle = [&] { e.H(0x17270); };
    if ((int32_t)cls > 0x11a) {
        if ((int32_t)cls <= 0x140) {
            if (cls == 0x140) { idle(); return; }
            if (cls == 0x11b) { e.H(0x179e0, (uint32_t)dist, 0, 0, 0, 0, target, unit); return; }
            if (cls == 0x13e) { e.H(0x17950, (uint32_t)dist, 0, 0, 0, 0, target, unit); return; }
            if (cls == 0x13f) { e.H(0x17c00, 0, 0, 0, 0, 0, unit); return; }
            idle(); return;
        }
        if (cls == 0x153) { e.H(0x179e0, (uint32_t)dist, 0, 0, 0, 0, target, unit); return; }
        if (cls == 0x22c) { e.H(0x17800, 0, 0, 0, 0, 0, unit); return; }
        if (cls == 0x23e) { e.H(0x17010, 0, 0, 0, 0, 0, unit); return; }
        idle(); return;
    }
    if ((int32_t)cls >= 0x116) return;
    switch (cls) {
    case 0x95: case 0x10c: e.H(0x17e80, 0, 0, 0, 0, 0, unit); return;
    case 0x97: case 0x10d: e.H(0x179e0, (uint32_t)dist, 0, 0, 0, 0, target, unit); return;
    case 0x9d: case 0x9e: e.H(0x17c00, 0, 0, 0, 0, 0, unit); return;
    case 0x9f: e.H(0x17e10, target, (uint32_t)dist, 0, 0, 0, unit); return;
    case 0xe3: e.H(0x17b60, target, 0, 0, 0, 0, unit); return;
    default: idle(); return;
    }
}

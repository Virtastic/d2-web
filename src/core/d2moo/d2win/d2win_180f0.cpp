// 1.13c D2Win+0x180F0 -- stdcall, one argument (the mouse message block: +0 the control out slot, +4 the message id):
// the mouse button dispatcher of the control list. 0x17800 sees the message first. A captured control (global at image
// offset 0xC9E24, type 0xB asserted) whose flags (+8) have both bit 2 and bit 0 (Fog's bit mask table entries 2 and 0)
// asks 0x10E10; the capture is then released (0x17FC0, ECX = control) and cleared, and a non-zero answer ends the call.
// Otherwise the list (global 0x214B8, or 0x214A0 when that is empty) is walked along +0x3C:
//   button down (0x201, 0x204): the pressed flag 0x214C4 := 1; a control is hit when its own test (+0x38, ECX = control)
//     answers non-zero, or without one when the cursor (0x21488, 0x2148C) lies in x <= cx < x + w, y - h <= cy < y
//     (+0xC, +0x10, +0x14, +0x18, signed); a hit control goes into the message and its handler (+0x24, one stack word =
//     message) runs with the depth counter 0x214A8 raised; a non-zero answer stops the walk;
//   button up (0x202, 0x205): the pressed flag := 0; every control goes into the message and its handler (+0x28) runs.
// Every path but the early one ends in the Storm import 0x7624 (the message), whose answer is EAX at return.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2win_c9e24;
__attribute__((weak)) uint32_t g_d2win_214a0;
__attribute__((weak)) uint32_t g_d2win_214a8;
__attribute__((weak)) uint32_t g_d2win_214b8;
__attribute__((weak)) uint32_t g_d2win_214c4;
__attribute__((weak)) int32_t g_d2win_21488;
__attribute__((weak)) int32_t g_d2win_2148c;
namespace d2win {
uint32_t f180f0(GE& e, uint32_t msg)
{
    e.h(0x17800, 0, 0, 0, 0, 0, msg);
    if (g_d2win_c9e24) {
        // retail: a captured control whose type is not 0xB is a fatal assert (line 0x62) and the process exits
        const uint32_t flags = GE::rd32(g_d2win_c9e24 + 8);
        if ((flags & 4) && (flags & 1)) {
            const uint32_t handled = e.h(0x10E10);
            uint32_t ret = handled;
            if (g_d2win_c9e24) {
                ret = e.h(0x17FC0, 0, g_d2win_c9e24);
                g_d2win_c9e24 = 0;
            }
            if (handled) return ret;
        }
    }
    uint32_t c = g_d2win_214b8 ? g_d2win_214b8 : g_d2win_214a0;
    const uint32_t id = GE::rd32(msg + 4);
    if (id == 0x201 || id == 0x204) {
        g_d2win_214c4 = 1;
        for (; c; c = GE::rd32(c + 0x3C)) {
            const uint32_t test = GE::rd32(c + 0x38);
            if (test) {
                if (!e.h(HookEnv::handlerRva(test), 0, c)) continue;
            } else {
                const int32_t x = (int32_t)GE::rd32(c + 0xC), y = (int32_t)GE::rd32(c + 0x10);
                if (g_d2win_21488 < x) continue;
                if (g_d2win_2148c < (int32_t)((uint32_t)y - GE::rd32(c + 0x18))) continue;
                if (g_d2win_21488 >= (int32_t)(GE::rd32(c + 0x14) + (uint32_t)x)) continue;
                if (!(g_d2win_2148c < y)) continue;
            }
            GE::wr32(msg, c);
            const uint32_t fn = GE::rd32(c + 0x24);
            if (!fn) continue;
            g_d2win_214a8++;
            const uint32_t r = e.h(HookEnv::handlerRva(fn), 0, 0, 0, 0, 0, msg);
            g_d2win_214a8--;
            if (r) break;
        }
    } else if (id == 0x202 || id == 0x205) {
        g_d2win_214c4 = 0;
        for (; c; c = GE::rd32(c + 0x3C)) {
            GE::wr32(msg, c);
            const uint32_t fn = GE::rd32(c + 0x28);
            if (fn) {
                g_d2win_214a8++;
                e.h(HookEnv::handlerRva(fn), 0, 0, 0, 0, 0, msg);
                g_d2win_214a8--;
            }
        }
    }
    return e.h(0x7624, 0, 0, 0, 0, 0, msg);
}
}

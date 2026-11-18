// 1.13c D2Client+0x9B300 -- left-button press in screen state 0xE: stores the position, then tries the hit tests in order.
// A hit on the first test (callee 0x26320 == 0 and 0x8BD90) sets the 0x11BEDC flag; a hit on 0x8B650 sets the press flags;
// 0x8B600 clears them (and arms the drag flags when no cursor item is held); otherwise a press inside the lists
// (0x8C580 / 0x8C440, handled by 0x98DC0) or the slot tests (0x8D3D0 / 0x8C410) clears the drag state. Every handled path marks
// the message and calls the handled routine; EAX is the value the last call left, modelled per exit.
#include "cl_mouse.h"
extern "C" uint32_t d2c_9b300(uint32_t eax, uint32_t msg, const cl::Env& e) {
    if (clc::G(e, 0x11BC34) != 0xE) return eax;
    const uint32_t x = clm::X(e, msg), y = clm::Y(e, msg);
    clm::pos(e, x, y);
    auto stop = [&]() {   // 4B382: the handled message, the 0x88A70 call, then the handled routine
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1C, 0);
        e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
        return e.H(0xBE84, msg);
    };
    eax = e.H(0x26320, 0, 0, 0, 0, 0, msg);
    if (eax == 0 && (eax = e.H(0x8BD90, 0, 0, 0, 0, 0, 0, x, y)) != 0) {
        clc::S(e, 0x11BEDC, 1);
        return stop();
    }
    eax = e.H(0x8B650, 0, 0, 0, 0, 0, x, 0, y);
    if (eax) {
        clc::S(e, 0x11BE90, 1);
        clc::S(e, 0x11BE8C, 1);
        clc::S(e, 0x11BE98, 0);
        clc::S(e, 0x11BE94, 0);
        clc::S(e, 0x11BEEC, 1);
        return stop();
    }
    eax = e.H(0x8B600, 0, 0, 0, 0, 0, x, 0, y);
    if (eax) {
        clc::S(e, 0x11BE90, 0);
        clc::S(e, 0x11BE8C, 0);
        if (clc::G(e, 0x11B834) == 0) {
            clc::S(e, 0x11BE98, 1);
            clc::S(e, 0x11BE94, 1);
            clc::S(e, 0x11BEEC, 1);
            e.W(msg + 0x18, 1);
            e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
        }
        e.W(msg + 0x1C, 0);
        return e.H(0xBE84, msg);
    }
    eax = e.H(0x26320, 0, 0, 0, 0, 0, msg);
    bool tail = true;   // 4B449: the message not handled leaves the third argument call to 0x8F880
    if (eax == 1) {
        clc::S(e, 0x11BE64, 0);
        clc::S(e, 0x11BEEC, 0);
        clc::S(e, 0x11BE8C, 0);
        clc::S(e, 0x11BE94, 0);
        tail = false;
    } else {
        bool list = (eax = e.H(0x8C580, 0, 0, 0, 0, 0, x, y)) != 0;
        if (!list) list = (eax = e.H(0x8C440, 0, 0, 0, 0, 0, x, y)) != 0;
        if (list) {
            e.H(0x98DC0, msg);
            e.W(msg + 0x18, 1);
            e.W(msg + 0x1C, 0);
            eax = e.H(0xBE84, msg);
        } else {
            eax = e.H(0x8D3D0, 0, 0, 0, 0, 0, y);
            if (eax) {
                clc::S(e, 0x11BE7C, 1);
                clc::S(e, 0x11BE6C, 0);
                clc::S(e, 0x11BE64, 0);
                clc::S(e, 0x11BEEC, 0);
                clc::S(e, 0x11BE8C, 0);
                clc::S(e, 0x11BE94, 0);
                e.W(msg + 0x18, 1);
                e.W(msg + 0x1C, 0);
                e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
                eax = e.H(0xBE84, msg);
            } else {
                eax = e.H(0x8C410, 0, 0, 0, 0, 0, x, y);
                if (eax) {
                    clc::S(e, 0x11BE6C, 0);
                    clc::S(e, 0x11BE64, 0);
                    clc::S(e, 0x11BEEC, 0);
                    clc::S(e, 0x11BE8C, 0);
                    clc::S(e, 0x11BE90, 0);
                    clc::S(e, 0x11BE94, 0);
                    clc::S(e, 0x11BE98, 0);
                    e.W(msg + 0x18, 1);
                    e.W(msg + 0x1C, 0);
                    return e.H(0xBE84, msg);
                }
            }
        }
    }
    if (tail && e.R(msg + 0x18) == 0) eax = e.H(0x8F880, y);
    clc::S(e, 0x11BE6C, 0);
    clc::S(e, 0x11BE90, 0);
    clc::S(e, 0x11BE98, 0);
    return eax;
}

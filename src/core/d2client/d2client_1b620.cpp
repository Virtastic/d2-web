// 1.13c D2Client+0x1B620: EAX = request struct (+4 -> unit). Finds the type-7 entry for the unit's id in the list at D2Client+0x11C4D4 (link +0x30), resolves its object id through the 128-bucket table at +0x10A808 (chain link +0xE4), and when the first list entry carrying that id has kind 2 (field +0x08 of the entry+0x24 block) calls 0x511F0(this, 0); every exit but the unvectored error path runs 0x1AAA0 and returns 0
// hd = host address of the module word at RVA 0x11C4D4; tb = host address of the bucket table at RVA 0x10A808
#include "cl17.h"
extern "C" uint32_t d2c_1aaa0(uint32_t eax, uint32_t edi, const cl::Env& e);
extern "C" uint32_t d2c_1b620(uint32_t edi, uint32_t hd, uint32_t tb, const cl::Env& e) {
    e.H(0x185e0, 0, 0, 0, 0, 0, 0, edi);
    uint32_t th = e.R(edi + 4);
    if (th) {
        uint32_t id = e.R(th + 0xc);
        uint32_t head = e.R(hd), n = head;
        if (n) {
            while (!(e.R(n + 4) == 7 && e.R(n + 0xc) == id && e.R(n + 0x20) == 0)) {
                n = e.R(n + 0x30);
                if (!n) { d2c_1aaa0(0, edi, e); return 0; }
            }
            uint32_t k = e.R(n + 8);
            if (k != 0xffffffffu) {
                uint32_t p = e.R(tb + (k & 0x7f) * 4);
                if (p) {
                    while (e.R(p + 0xc) != k) {
                        p = e.R(p + 0xe4);
                        if (!p) { d2c_1aaa0(0, edi, e); return 0; }
                    }
                    // e.R(p) != 1 -> the log-and-exit path, never vectored
                    uint32_t q = head;
                    while (e.R(q + 8) != k) q = e.R(q + 0x30);
                    if (e.R(q + 0x24 + 8) == 2) e.H(0x511f0, 0, 0, 0, 0, 0, 0, th);
                }
            }
        }
    }
    d2c_1aaa0(0, edi, e);
    return 0;
}

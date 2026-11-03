// 1.13c D2Common+0x2E0C0: init a tile record from an act (regs: EAX = act, ESI = out; void). The 12 out dwords are zeroed,
// then the act's mode byte (+0x480) picks the 0x9306 query: mode 0 gives (0, 0), mode 1 gives (0, 1), mode 2 gives
// (0x1D, 0xC), any other mode returns. A nonzero query result sets out[6] to its first dword, out[5] to 0x4000 (+4 with
// flag 1, +0x800 with flag 4, both from 0x92FA), and out bytes 0x28..0x2A to 0xFF. A zero result is retail's fatal 0x44C;
// the null act and out are 0x437 / 0x438.
#include <cstdint>

struct D2Common2E0C0Env {
    int32_t (*fn9306)(uint32_t p, uint32_t a, uint32_t b, uint32_t c, uint32_t* buf, uint32_t size);   // stdcall, 6 words
    uint32_t (*fn92FA)(uint32_t id);                                            // stdcall, 1 word: the flag word
    void (*onFatal)(uint32_t code);                                             // null: the fatal path returns
};

extern "C" void D2Common_2E0C0_InitTile(uint32_t act, uint32_t out, const D2Common2E0C0Env* e)
{
    auto W = [](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); };
    auto B = [](uint32_t a) -> uint8_t& { return *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); };
    if (!act) { if (e->onFatal) e->onFatal(0x437); return; }
    if (!out) { if (e->onFatal) e->onFatal(0x438); return; }
    for (uint32_t i = 0; i < 12; ++i) W(out + 4 * i) = 0;
    const uint8_t mode = B(act + 0x480);
    uint32_t a = 0, c = 0;
    if (mode != 0) {
        if (mode == 1) c = 1;
        else if (mode == 2) { a = 0x1D; c = 0xC; }
        else return;
    }
    uint32_t buf[40] = {};
    if (e->fn9306(act + 0x0C, 0, a, c, buf, 0x28)) {
        W(out + 0x18) = buf[0];
        W(out + 0x1C) = 0;
        W(out + 0x24) = 0;
        B(out + 0x28) = 0xFF;
        B(out + 0x29) = 0xFF;
        B(out + 0x2A) = 0xFF;
        W(out + 0x14) = 0x4000;
        const uint32_t flags = e->fn92FA(buf[0]);
        if (flags & 1) W(out + 0x14) |= 4;
        if (flags & 4) W(out + 0x14) |= 0x800;
        return;
    }
    if (e->onFatal) e->onFatal(0x44C);
}

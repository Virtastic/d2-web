// 1.13c D2Client+0x47500 seam types: shared by the port, the env adapter and the FN probe.
#pragma once
#include <cstdint>
namespace d2client_47500 {
struct Env {
    void* c;
    const char16_t* (*str)(void*, uint32_t id);
    uint32_t (*menu)(void*, const uint32_t w[11]);                                   // 0x6FB53E40: stack words 0..10
    void (*line)(void*, uint32_t colour, uint32_t menu, const uint32_t w[6]);        // 0x6FB536F0: EAX colour, ECX menu, stack words 0..5
    void (*name)(void*, uint32_t unit, char16_t* buf, uint32_t n);                   // 0x6FB414F0: (unit, buf, 0xBF)
    void (*cost)(void*, uint32_t* out, uint32_t unit, uint32_t carried, char16_t* buf, uint32_t n);   // 0x6FAF6230: EAX = out, ECX = unit, (carried, buf, 0x28)
    void (*fatal)(void*, uint32_t code);                                             // log + _exit(-1)
};
struct State {      // the module globals the function reads and writes
    uint32_t box;         // 0x6FBC9757
    uint32_t g9663;       // 0x6FBC9663 (dword store)
    uint32_t mode;        // 0x6FBC9729
    uint32_t a824, a828;  // 0x6FBCB824 / 0x6FBCB828 (menu anchor)
    uint32_t id;          // 0x6FBD22FD
    char16_t* gold;       // 0x6FBD230D (wide text, 0x28 bytes)
    const uint32_t* hash; // 0x6FBBAE08[128]: unit hash heads
};
void sub_47500(const Env& e, State& s);
}

#pragma once
#include <cstdint>
namespace d2client_ad950 {
struct Env {
 void (*fnAC140)(uint32_t eax, uint8_t* esi);                           // D2Client+0xAC140: eax = the caller's ecx, esi = the 12-byte local it fills
 void (*fnC40C)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);        // D2Client+0xC40C import thunk (4 stack words)
 void (*fn2A1B0)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);       // D2Client+0x2A1B0 (4 stack words)
 uint32_t player;                                                       // D2Client+0x11BBFC
};
void sub_AD950(const Env& e, uint32_t ecx);
}

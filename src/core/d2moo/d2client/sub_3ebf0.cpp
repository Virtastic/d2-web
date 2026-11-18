// D2Client+0x3EBF0: reset the 0x3C4-byte block at 0x6fbc9968 to 0, the 0xF0-byte block at 0x6fbc9c38 to 0xFF,
// then call sub_3E7F0 (0x3E7F0) for both panel layouts.
#include <cstdint>
namespace d2client_3ebf0 {
struct Env {
 uint8_t* blk9968;   // DAT_6fbc9968, 0xF1*4+1 bytes
 uint32_t* blk9c38;  // DAT_6fbc9c38, 0x3C dwords
 uint32_t* g9d28;    // DAT_6fbc9d28
 uint32_t (*cc420)();  // DAT_6fbcc420
 uint32_t player;    // DAT_6fbcbbfc
 int32_t width;      // DAT_6fb8bc48
 void (*f3e7f0)(uint32_t, int32_t, int, int, int);
};
// 1.13c D2Client+0x3EBF0
void Fn_3EBF0(const Env& e) {
 for (int i = 0; i < 0xF1 * 4 + 1; ++i) e.blk9968[i] = 0;
 for (int i = 0; i < 0x3C; ++i) e.blk9c38[i] = 0xFFFFFFFFu;
 *e.g9d28 = 0;
 if (e.cc420()) { e.f3e7f0(e.player, 0x50, 1, 0, 0); }
 else { e.f3e7f0(e.player, e.width - 0x80, 0, 0, 0); }
 if (e.cc420()) { e.f3e7f0(e.player, 0x50, 1, 1, 0); }
 else { e.f3e7f0(e.player, e.width - 0x80, 0, 1, 0); }
}
}

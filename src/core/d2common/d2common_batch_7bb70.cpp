// D2Common 1.13c batch. Room status-list relink; retail layout offsets, 32-bit link slots.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x7BB70: room in EAX, status in BL (unaff_BL). If the status differs from +0x44,
// unlink the room from its current list node (+0x1c prev / +0xe8 next, only when both set), then if
// status < 4 insert it after list head (*(*(+0x58)+0x1b4) + 0xa0 + status*0xec) and store +0x44.
// Oddity: when status >= 4 the room is left unlinked and +0x44 is not updated.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7bb70(intptr_t pRoom, uint8_t nStatus) {
 auto F = [](intptr_t p, int off) -> uint32_t& { return *reinterpret_cast<uint32_t*>(p + off); };
 if (*reinterpret_cast<uint8_t*>(pRoom + 0x44) == nStatus) return;
 if (F(pRoom, 0xe8) != 0 && F(pRoom, 0x1c) != 0) {
  F(F(pRoom, 0xe8), 0x1c) = F(pRoom, 0x1c);
  F(F(pRoom, 0x1c), 0xe8) = F(pRoom, 0xe8);
  F(pRoom, 0xe8) = 0;
  F(pRoom, 0x1c) = 0;
 }
 if (nStatus < 4) {
  const uint32_t head = F(F(pRoom, 0x58), 0x1b4) + 0xa0 + uint32_t(nStatus) * 0xec;
  F(pRoom, 0x1c) = head;
  F(pRoom, 0xe8) = F(head, 0xe8);
  F(F(head, 0xe8), 0x1c) = uint32_t(pRoom);
  F(head, 0xe8) = uint32_t(pRoom);
  *reinterpret_cast<uint8_t*>(pRoom + 0x44) = nStatus;
 }
}
}

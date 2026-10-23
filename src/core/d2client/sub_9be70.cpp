// D2Client sub_9BE70: mouse-message hit test against a panel rect, then FUN_6fb48dc0 (0x98DC0).
#include <cstdint>
#define W __attribute__((weak))
extern "C" {
W extern int32_t DAT_6fbbb2a4, DAT_6fbbb430, DAT_6fbbb2f8, DAT_6fbbb2fc, DAT_6fbbb300, DAT_6fbbb304;
W void Ordinal_511(void* msg);
W void FUN_6fb48dc0(uint8_t* msg);

// 1.13c D2Client+0x9BE70
// `msg` is the hidden ESI argument (+0xc x, +0xe y, +0x18 handled, +0x1c result).
int32_t d2client_sub_9be70(uint8_t* msg) {
  int32_t y = *(uint16_t*)(msg + 0xe), x = *(uint16_t*)(msg + 0xc);
  DAT_6fbbb2a4 = x; DAT_6fbbb430 = y;
  *(uint32_t*)(msg + 0x18) = 1; *(uint32_t*)(msg + 0x1c) = 0;
  Ordinal_511(nullptr);
  if (DAT_6fbbb2f8 <= x && x < DAT_6fbbb2fc && DAT_6fbbb300 <= y && y <= DAT_6fbbb304) {
    FUN_6fb48dc0(msg);
    return 1;
  }
  return 0;
}
}

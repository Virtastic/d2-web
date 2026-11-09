// D2Client sub_9BE70: mouse-message hit test against a panel rect, then FUN_6fb48dc0 (0x98DC0).
#include <cstdint>
#define W __attribute__((weak))
extern "C" {
W extern int32_t DAT_6fbbb2a4, DAT_6fbbb430, DAT_6fbbb2f8, DAT_6fbbb2fc, DAT_6fbbb300, DAT_6fbbb304;
W void Ordinal_511(void* msg);
W void FUN_6fb48dc0(uint8_t* msg);

// The retail cells and callees as the replay probes see them: x/y are 0x10B2A4 / 0x10B430, the rect is 0x10B2F8..0x10B304.
struct D2c9be70Env {
  int32_t *x, *y, *left, *right, *top, *bottom;
  void (*ord511)(uint8_t* msg);     // thunk 0xBE84, stdcall: the message is its one stack word (the push esi at 0x9BE7A)
  void (*click98dc0)(uint8_t* msg); // 0x98DC0, ESI = msg and the same pointer on the stack (ret 4)
};

// 1.13c D2Client+0x9BE70
// `msg` is the hidden ESI argument (+0xc x, +0xe y, +0x18 handled, +0x1c result).
int32_t d2_client_9be70(uint8_t* msg, const D2c9be70Env* e) {
  int32_t y = *(uint16_t*)(msg + 0xe), x = *(uint16_t*)(msg + 0xc);
  *e->x = x; *e->y = y;
  *(uint32_t*)(msg + 0x18) = 1; *(uint32_t*)(msg + 0x1c) = 0;
  e->ord511(msg);
  if (*e->left <= x && x < *e->right && *e->top <= y && y <= *e->bottom) {
    e->click98dc0(msg);
    return 1;
  }
  return 0;
}

static void o511(uint8_t* m) { Ordinal_511(m); }
static void c98dc0(uint8_t* m) { FUN_6fb48dc0(m); }

int32_t d2client_sub_9be70(uint8_t* msg) {
  const D2c9be70Env e = { &DAT_6fbbb2a4, &DAT_6fbbb430, &DAT_6fbbb2f8, &DAT_6fbbb2fc, &DAT_6fbbb300, &DAT_6fbbb304, o511, c98dc0 };
  return d2_client_9be70(msg, &e);
}
}

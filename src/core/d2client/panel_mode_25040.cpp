// 1.13c D2Client+0x25040 -- mode switch on a panel object (ESI=this, EDI=new mode, stack arg=param_1).
// Pure state part; side-effect calls are reported as event bits (caller performs them).
#include <stdint.h>
#include <stddef.h>
struct D2PanelMode { int32_t cur; uint32_t v11; uint32_t flags; };            // this[4], this[0x11], this[0x31]
struct D2PanelTbl { uint32_t e4, dc; uint8_t b118[8]; uint8_t b129[8]; uint8_t b12b; }; // **this[5] fields used
enum { EV_FLUSH = 1, EV_PANELEND = 2, EV_ENTER = 4, EV_FATAL = 8 }; // 11090+1a220 / 1660,1630,10331,10032 / 10408
uint32_t d2client_panel_mode_25040(D2PanelMode* s, const D2PanelTbl* t, int32_t mode, int32_t p1) {
  uint32_t ev = 0; uint32_t b;
  if (mode > 7) return EV_FATAL;
  if (mode == 0 && s->cur == 2 && t->e4 != 0x100) {
    s->v11 = t->b129[0]; ev = EV_FLUSH | EV_PANELEND; s->flags &= ~2u; goto tail;
  }
  if (mode != 0 && mode == 2 && s->cur == 0 && t->dc != 0x100) {
    ev = EV_FLUSH; s->flags &= ~2u; b = t->b12b; goto set;
  }
  s->flags = p1 ? s->flags | 2 : s->flags & ~2u;
  if (mode != s->cur) {
    if (mode == 4 || mode == 2 || mode == 5) { if (s->cur != 5 && s->cur != 2 && s->cur != 4) ev |= EV_ENTER; }
    else if (mode == 0 && s->cur != 0) ev |= EV_PANELEND;
    ev |= EV_FLUSH;
  }
  b = t->b129[mode];
set: s->v11 = b;
tail:
  s->flags = t->b118[s->cur] ? s->flags & ~0x20u : s->flags | 0x20;
  return ev;
}

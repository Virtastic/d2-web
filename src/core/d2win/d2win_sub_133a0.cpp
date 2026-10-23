// 1.13c D2Win+0x133A0 (size 482): draw the popup text (0x6F9A9E58) in a filled box, then restore the font.
// Seams: D2Gfx #10080/#10014/#10049, D2Win 0x11FB0 (max width) 0x11E80 (height) 0x12940 (draw) 0x12FE0 (set font).
#include <cstdint>
struct D2Win133A0Env {
  const uint16_t* text;        // 0x6F9A9E58
  int32_t curFont;             // 0x6F8FFD34
  int32_t fontScale;           // 0x6F8FFD40
  uint32_t posX;               // 0x6F9AA658
  int32_t textColor;           // 0x6F9AA660
  int32_t posY;                // 0x6F9AA65C
  int32_t centered;            // 0x6F9AA664
  int32_t fillPct;             // 0x6F9AA668 (0..128)
  uint8_t* fillCache;          // 0x6F9A9D10
  const uint8_t* fontHeights;  // per-font byte at [table+font*0x14][+10] already resolved by caller
  int32_t (*maxWidth)();                                        // 0x11FB0
  int32_t (*height)();                                          // 0x11E80
  void (*getDims)(int32_t*, int32_t*);                          // #10080
  void (*fill)(int32_t, int32_t, int32_t, int32_t, uint32_t, int32_t mode); // #10014
  uint8_t (*mkTrans)();                                         // #10049(0x6F922338,0x100,0x50,0,0)
  void (*draw)(const uint16_t*, int32_t y, int32_t w, int32_t col, int32_t centered); // 0x12940
  void (*setFont)(int32_t);                                     // 0x12FE0
};
// 1.13c D2Win+0x133A0
void d2win_sub_133a0(D2Win133A0Env& e) {
  if (!e.text || !e.text[0]) return;
  int32_t f = e.curFont, w, h, dx, dy, y0;
  uint32_t px;
  for (;;) {
    w = e.maxWidth() + 8;
    h = e.height();
    px = e.posX;
    if (e.centered == 1) px -= w >> 1;
    y0 = e.posY + 2;
    e.getDims(&dx, &dy);
    if (h <= dy - 10) break;
    if (f == 1) { e.setFont(0); f = 0; }
    else { if (e.curFont) break; e.setFont(6); f = 6; }
  }
  int32_t x = dx - w;
  int32_t lim = (int32_t)(((int32_t)px < 1 ? 0u : ~0u) & px);
  if (lim < x) x = lim;
  int32_t b = h - 5; if (b < y0) b = y0;
  int32_t yb = dy - 5; if (b < yb) yb = b;
  uint32_t mode;
  if (!e.fillPct) { mode = 2; e.fill(x, yb - h, x + w, yb, 0, 0); }
  else {
    if (!*e.fillCache) *e.fillCache = e.mkTrans();
    mode = 1;
    int32_t fw = (e.fillPct * w) >> 7, ty = yb - h;
    e.fill(x, ty, x + fw, yb, *e.fillCache, 0);
    if (w - fw) e.fill(x + fw, ty, x + w, yb, 0, 2); // ponytail: decomp used unaffected EBP for x; x+fw assumed
  }
  e.draw(e.text, yb - (int32_t)(*e.fontHeights * e.fontScale) / 10, w, e.textColor, 1);
  if (mode != (uint32_t)x) e.setFont(x);
}

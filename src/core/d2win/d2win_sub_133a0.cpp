// 1.13c D2Win+0x133A0 (size 482): draw the popup text (0x6F9A9E58) in a filled box, then restore the font.
// Seams: D2Gfx #10080/#10014/#10049, D2Win 0x11FB0 (max width) 0x11E80 (height) 0x12940 (draw) 0x12FE0 (set font).
// Arguments checked against the retail disassembly (not the decompile): the draw takes x in EAX (the clamped box
// x); the remainder strip's first corner is the slot at [esp+0x20], which holds posY+2, not x; the fill modes are
// 1 (tiled, cache palette) and 2 (plain) as pushed; the closing restore sets the font to the entry font when the
// loop left it changed (the entry value sits at [esp+0x28]).
#include <cstdint>
#include <emscripten.h>
struct D2Win133A0Env {
  const uint16_t* text;        // 0x6F9A9E58
  int32_t* curFont;            // 0x6F8FFD34, live: SetUnicodeTextFont writes it
  int32_t fontScale;           // 0x6F8FFD40
  uint32_t posX;               // 0x6F9AA658
  int32_t textColor;           // 0x6F9AA660
  int32_t posY;                // 0x6F9AA65C
  int32_t centered;            // 0x6F9AA664
  int32_t fillPct;             // 0x6F9AA668 (0..128)
  uint8_t* fillCache;          // 0x6F9A9D10
  int32_t (*fontHeight)(int32_t font);                          // byte [DAT_6f901518 + font*0x14 + 10]
  int32_t (*maxWidth)();                                        // 0x11FB0 (text in ECX)
  int32_t (*height)();                                          // 0x11E80
  void (*getDims)(int32_t*, int32_t*);                          // #10080 (width, height)
  void (*fill)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t pal, int32_t mode); // #10014
  uint8_t (*mkTrans)();                                         // #10049(0x6F922338,0x100,0x50,0,0)
  void (*draw)(int32_t x, const uint16_t* text, int32_t y, int32_t w, int32_t col, int32_t centered); // 0x12940
  void (*setFont)(int32_t);                                     // 0x12FE0
};
// 1.13c D2Win+0x133A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_133a0(D2Win133A0Env& e) {
  if (!e.text[0]) return;
  const int32_t entryFont = *e.curFont;
  int32_t f = entryFont, w, h, dx, dy, y0;
  uint32_t px;
  for (;;) {
    w = e.maxWidth() + 8;
    h = e.height();
    px = e.posX;
    if (e.centered == 1) px -= (uint32_t)(w >> 1);
    y0 = e.posY + 2;
    e.getDims(&dx, &dy);
    if (h <= dy - 10) break;
    if (f == 1) { e.setFont(0); f = 0; }
    else { if (*e.curFont != 0) break; e.setFont(6); f = 6; }
  }
  int32_t x = dx - w;
  int32_t lim = (int32_t)(((int32_t)px < 1 ? 0u : ~0u) & px);
  if (lim < x) x = lim;
  int32_t b = h - 5; if (b < y0) b = y0;
  int32_t yb = dy - 5; if (b < yb) yb = b;
  if (!e.fillPct) {
    e.fill(x, yb - h, x + w, yb, 0, 2);
  } else {
    if (!*e.fillCache) *e.fillCache = e.mkTrans();
    int32_t fw = (e.fillPct * w) >> 7, ty = yb - h;
    e.fill(x, ty, x + fw, yb, *e.fillCache, 1);
    if (w - fw) e.fill(y0, ty, y0 + (w - fw), yb, 0, 2);   // retail first corner is [esp+0x20] = posY+2
  }
  const int32_t lineH = e.fontHeight(*e.curFont) * e.fontScale;
  e.draw(x, e.text, yb - lineH / 10, w, e.textColor, 1);
  if (f != entryFont) e.setFont(entryFont);
}

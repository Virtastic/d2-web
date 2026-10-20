// D2Client 0xBC8F0..0xBE860 batch: stat-panel / HUD text helpers, env-seam implementations from the decompile.
// Imported ordinals (ord) and unrecovered D2Client callees (fn, by 1.13c RVA) are Env function pointers; retail memory via rd/wr.
// Register/stack arg order of the odd-convention functions is positional (see each marker).
#include <cstdint>
namespace d2client_batch_bc8f0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); uint32_t (*rd16)(P); void (*wr32)(P, uint32_t); void (*wr16)(P, uint32_t); void (*wr8)(P, uint32_t);
 uint32_t (*ord)(uint32_t id, P a, P b, P c, P d, P e, P f);   // Ordinal_<id> imports (D2Gfx/D2Win/D2Common/D2Lang)
 uint32_t (*fn)(P rva, P a, P b, P c, P d);                    // unrecovered D2Client callees by RVA
 int32_t (*uStrlen)(P); P (*uStrstr)(P, P); void (*uStrcpy)(P, P); void (*uStrncpy)(P, P, int32_t);
 void (*uSprintf)(int32_t, P, P); void (*win2Unicode)(P, P, int32_t); int32_t (*uWidth)(P, int32_t);
 int32_t (*sendMessageA)(uint32_t hwnd, uint32_t msg, uint32_t wp, uint32_t lp);
 int32_t (*getKeyState)(int32_t); void (*sprintfA)(P, P, uint32_t); int32_t (*snprintfPath)(P, P, P);
 void (*fatal)(uint32_t code);   // _exit(-1) path (Ordinal_10265 + 10024)
 P sgptDataTables, toUpperTable;
};
// D2Client globals (VA)
enum : P {
 gOrgX = 0x6fbcb9a0, gOrgY = 0x6fbcb9a4, gScrW = 0x6fb8bc48, gScrH = 0x6fb8bc4c, gColor = 0x6fbcbff8,
 gCls = 0x6fba2188, gMode = 0x6fbcbbc8, gTbl = 0x6fb835a4, gFlag9854 = 0x6fbc9854, gCharSel = 0x6fbcc390,
 gSub394 = 0x6fbcc394, gVal9858 = 0x6fbc9858, gPlayer = 0x6fbcbbfc, gBtnDown = 0x6fbcbbc4,
 gCursX = 0x6fbcb824, gCursY = 0x6fbcb828, gState864 = 0x6fbcb864, gMsg980 = 0x6fbcc980, gMsgY = 0x6fbcca24,
 gStr30 = 0x6fbcca30, gStr30b = 0x6fbccc30, gRows = 0x6fb8d6e0, gBtns = 0x6fb8d814,
};
template <class... A> static uint32_t O(const Env& e, uint32_t id, A... a) { P v[6] = {(P)a...}; return e.ord(id, v[0], v[1], v[2], v[3], v[4], v[5]); }
template <class... A> static uint32_t F(const Env& e, P rva, A... a) { P v[4] = {(P)a...}; return e.fn(rva, v[0], v[1], v[2], v[3]); }
static int32_t i32(const Env& e, P a) { return (int32_t)e.rd32(a); }
static int32_t s16(const Env& e, P a) { return (int16_t)e.rd32(a); }
static P cls(const Env& e) {
 int32_t c = i32(e, gCls); P t = e.rd32(e.sgptDataTables);
 return (c < 0 || i32(e, t + 0xa80) <= c) ? 0 : e.rd32(t + 0xa78) + (P)c * 0x1a8;
}
// 1.13c D2Client+0xBC8F0  (x = EAX, y = stack arg 8, a = stack arg 4, b = return-slot arg)
void sub_BC8F0(const Env& e, int32_t x, int32_t y, int32_t a, int32_t b) {
 bool l = false, r = false;
 e.wr32(gColor, O(e, 10190, 0, 0, 0, 0, 0, 0));
 int32_t ox = i32(e, gOrgX), oy = i32(e, gOrgY), sh = i32(e, gScrH);
 if (ox + 0xa2 <= x) {
  if (ox + 0x140 < x || y < oy - 0x14e + sh || oy - 0x13b + sh < y) {
   l = false;
   if (x <= ox + 0x140 && oy - 0x134 + sh <= y && y <= oy - 0x121 + sh) r = true;
  } else l = true;
 }
 int32_t idx = 0, v = a;
 if (a == 0) l = false;
 if (b == 0) r = false;
 if (!l) { if (!r) return; idx = 1; v = b; }
 if (v < 5) v = 5; else if (v > 0x5f) v = 0x5f;
 P t = gTbl + (P)idx * 0x18;
 int32_t y0 = i32(e, t + 4) + oy - 0x1e0 + sh;
 uint32_t buf[3] = {e.rd32(gMode), 2, 0};   // ponytail: only the head of the 0x80-wchar buffer matters to the seam
 (void)buf;
 O(e, 10014, i32(e, t) + ox, y0, i32(e, t) + ox + 0x9b, 0, 0, 0);
 O(e, 10003, i32(e, t + 0xc) + oy - 0x1e0 + sh, 0, 0, 0, 0, 0);
 O(e, 10150, 0, 0, 0, 0, 0, 0);
 if (cls(e)) {
  uint32_t s = O(e, 10003, v, 0, 0, 0, 0, 0);
  uint32_t fmt = O(e, 10003, s, 0, 0, 0, 0, 0);
  e.uSprintf(0x80, (P)y0 + 0x1e, fmt);   // dest is the local wchar buffer owned by the seam
  O(e, 10150, i32(e, t + 0x14) + oy - 0x1e0 + sh, 0, 0, 0, 0);
  return;
 }
 O(e, 10265, 0xec, 0, 0, 0, 0, 0); e.fatal(0xec);
}
// 1.13c D2Client+0xBCB30  (esi = mouse event record)
uint32_t sub_BCB30(const Env& e, P ev) {
 if (O(e, 10031, 0, 0, 0, 0, 0, 0) == 2 && F(e, 0x26320, 0, 0, 0, 0) == 0 &&
     (int32_t)e.rd16(ev + 0xc) <= i32(e, gScrW) / 2 && (int32_t)e.rd16(ev + 0xe) < i32(e, gScrH) - 0x2f) {
  if (i32(e, gState864) == 6) F(e, 0xBEB90, 0, 0, 0, 0);
  e.wr32(ev + 0x18, 1);
  O(e, 511, 0, 0, 0, 0, 0, 0);
  return 1;
 }
 return 0;
}
// 1.13c D2Client+0xBCB90  (param_1 = unit, ebx = value)
int32_t sub_BCB90(const Env& e, P u, int32_t ebx) {
 if (!u) return 0;
 uint32_t c = e.rd32(gCharSel) & 0xff;
 if (c > 2) { O(e, 10265, 0x18e, 0, 0, 0, 0, 0); e.fatal(0x18e); return 0; }
 P r = cls(e);
 if (!r) { O(e, 10265, 0x193, 0, 0, 0, 0, 0); e.fatal(0x193); return 0; }
 uint32_t s394 = e.rd32(gSub394), a = (s394 < 2 || 3 < s394) ? 1 : e.rd32(gVal9858);
 int32_t cap = s16(e, r + 0xaa + c * 2);
 O(e, 11089, (P)i32(e, gCls), a, c, (P)cap, 2, 0);
 if ((e.rd32(r + 0x4c) & 0xff) != 1 && e.rd32(gFlag9854) == 0 && c != 0) ebx = ebx * 10 / 12;
 int32_t v = (int32_t)F(e, 0xBC720, 0, 0, 0, 0);
 if (ebx < 0) { v -= ebx; ebx = 0; }
 int32_t pct = 0;
 if (v < 0) { ebx -= v; v = 0; if (ebx != 0) pct = v * 100 / (ebx + v); }
 else if (v != 0 || ebx != 0) pct = (v == 0 && ebx == 0) ? 0 : v * 100 / (ebx + v);
 int32_t k = (int32_t)O(e, 10973, 2, 0xc, 0, 0, 0, 0);
 int32_t o = k * pct * 2 / (cap + k);
 if (o > 4) return o > 0x5f ? 0x5f : o;
 return 5;
}
// 1.13c D2Client+0xBDCA0
void sub_BDCA0(const Env& e, P ev) {
 int32_t x = e.rd16(ev + 0xc), y = e.rd16(ev + 0xe);
 int32_t ox = i32(e, gOrgX), oy = i32(e, gOrgY), sh = i32(e, gScrH);
 F(e, 0x16300, ev, 0, 0, 0);
 uint32_t was = e.rd32(0x6fbaad88);
 e.wr32(gBtnDown, 0);
 if (x < ox + 0x80 || ox + 0xa0 < x || y < oy - 0x5c + sh || oy - 0x3c + sh < y) {
  if (x < ox || i32(e, gScrW) / 2 - 1 < x || y < oy || oy - 0x31 + sh < y) {
   if (sub_BCB30(e, ev) == 0) e.wr32(ev + 0x18, 0);
   return;
  }
  if (O(e, 10587, e.rd32(gPlayer), 0, 0, 0, 0, 0) != 0) {
   P p = gBtns;
   for (int32_t i = 0; p < 0x6fb8d84c; i++, p += 0xe) {
    int32_t row = i32(e, p) + oy + sh;
    e.wr32(p + 4, 0);
    if (row - 0x1f6 < y && y < row - 0x1e0 && i32(e, p - 4) + ox < x && x < i32(e, p - 4) + ox + 0x28) {
     O(e, 10973, 0, 4, 0, 0, 0, 0);
     if (e.getKeyState(0x10) >= 0) {
      int32_t n = 1;
      do { int32_t k = n > 0x1f ? 0x20 : n; n -= k; F(e, 0x143E0, 0, 0, 0, 0); } while (n);
     }
     e.wr32(0x6fbcc3dc, 0); e.wr32(0x6fbcc3e0, 0); e.wr32(0x6fbcc3f8, 0); e.wr32(0x6fbcc3fc, 0);
     e.wr32(i + 0x18, 1); e.wr32(i + 0x1c, 0);   // decompile: *(iVar3+0x18) with iVar3 = loop index (kept verbatim)
     O(e, 511, i, 0, 0, 0, 0, 0);
     return;
    }
   }
  }
  e.wr32(0x6fbcc3dc, 0); e.wr32(0x6fbcc3e0, 0); e.wr32(0x6fbcc3f8, 0); e.wr32(0x6fbcc3fc, 0);
  e.wr32(ev + 0x18, 1); e.wr32(ev + 0x1c, 0);
  O(e, 511, ev, 0, 0, 0, 0, 0);
  return;
 }
 O(e, 10227, 0, 0, 0, 0, 0, 0);
 if (e.rd32(0x6fb8d5d0) != 0x26) { O(e, 10265, 0, 0, 0, 0, 0, 0); e.fatal(0); return; }
 e.wr32(0x6fbaad88, 0);
 if (was) {
  F(e, 0xBF720, 0, 0, 0, 0);
  uint32_t w = O(e, 10048, 0, 0, 0, 0, 0, 0);
  F(e, 0x8A4D0 /*FUN_6faecc60 slot*/, w, 0x6fb80628, 0, 0);
  F(e, 0xBE2E0, 0, 0, 0, 0);
 }
 int32_t h = i32(e, 0x6fba9e14);
 if (e.rd32(0x6fbaad84) == 0 && e.rd32(0x6fbaad90) == 0) {
  e.wr32(0x6fbcc418, 0); e.wr32(0x6fbcc414, 0);
  O(e, 10006, 0, 0, 0, 0, 0, 0);
  F(e, 0x3CC60 /*FUN_6fb3a4d0 slot*/, h, 0, 0, 0);
  return;
 }
 int32_t w = i32(e, gScrW);
 e.wr32(0x6fbcc418, (uint32_t)-((w + ((w >> 31) & 3)) >> 2)); e.wr32(0x6fbcc414, 1);
 O(e, 10006, 0, 0, 0, 0, 0, 0);
 int32_t q = (h + ((h >> 31) & 3)) >> 2;
 F(e, 0x3CC60, h - q, 0, (P)-q, 0);
}
// 1.13c D2Client+0xBE020
void sub_BE020(const Env& e, P bufA, P bufB, P fmtA, P fmtB, P dirStr) {
 e.snprintfPath(bufA, fmtA, dirStr);   // "%s\\Palette\\Loading\\pal.dat"
 e.snprintfPath(bufB, fmtB, dirStr);   // "%s\\Palette\\Loading\\pal.pl2"
 O(e, 10128, bufA, bufB, 0, 0, 0, 0);
}
// 1.13c D2Client+0xBE2B0
void sub_BE2B0(const Env& e) {
 uint32_t lp = (e.rd32(gCursX) & 0xffff) << 16 | (e.rd32(gCursY) & 0xffff);   // CONCAT22(x, y)
 e.sendMessageA(O(e, 10048, 0, 0, 0, 0, 0, 0), 0x200, 0, lp);
}
// 1.13c D2Client+0xBE430
void sub_BE430(const Env& e) {
 if (e.uStrlen(gMsg980) == 0) return;
 O(e, 10028, gMsg980, 0, 0, 0, 0, 0);
 int32_t w = (int32_t)O(e, 10083, 0, 0, 0, 0, 0, 0);
 int32_t y = i32(e, gMsgY) + 2;
 if (y - (w + 7) < 0) y = w + 7;
 if (i32(e, gScrH) - 0x1f < y) y = i32(e, gScrH) - 0x1f;
 O(e, 10150, (P)(y - 2), 0, 1, 0, 0, 0);
 for (int i = 0; i < 0x28; i++) e.wr32(gMsg980 + i * 4, 0);
}
// 1.13c D2Client+0xBE6D0  (__fastcall: ecx unused, edx = src string, stack = param_3)
void sub_BE6D0(const Env& e, uint32_t, P src, uint32_t p3) {
 if (!src) { e.wr16(gStr30, 0); return; }
 e.uStrcpy(gStr30, src);
 e.wr32(gStr30b, p3);
}
// 1.13c D2Client+0xBE760  (returns the same string; strips every non-terminal occurrence of string #1)
P sub_BE760(const Env& e, P s) {
 if (e.uStrlen(s) >= 500) { O(e, 10265, 0x37f, 0, 0, 0, 0, 0); e.fatal(0x37f); return 0; }
 P pat = O(e, 10003, 0, 0, 0, 0, 0, 0);
 int32_t n = e.uStrlen(pat);
 P d = e.uStrstr(s, pat);
 while (d && e.rd16(d + n * 2) != 0) {
  int32_t len = e.uStrlen(d), cnt = (len - n) * 2;
  // memmove(d, d + n*2 + 2, cnt): decompile's byte offsets (Unicode* stride 2) -> overlapping forward copy
  for (int32_t i = 0; i < cnt; i += 2) e.wr16(d + i, e.rd16(d + n * 2 + 2 + i));
  d = e.uStrstr(s, pat);
 }
 return s;
}
// 1.13c D2Client+0xBE800  (esi = wide string, upper-cases chars < 0x100 via the lang table)
void sub_BE800(const Env& e, P s) {
 if (!s) return;
 int32_t n = e.uStrlen(s);
 for (int32_t i = 0; i < n; i++) {
  uint32_t c = e.rd16(s + i * 2);
  if (c < 0x100) c = e.rd16(e.toUpperTable + c * 2);
  e.wr16(s + i * 2, c);
 }
}
// 1.13c D2Client+0xBE860
void sub_BE860(const Env& e) {
 F(e, 0x8C6B0, 0, 0, 0, 0);
 e.wr32(gColor, O(e, 10190, 0xff, 0, 0, 0, 0, 0));
 e.wr32(gColor, O(e, 10190, 0, 0, 0, 0, 0, 0));
 e.wr32(0x6fbcbfcc, O(e, 10190, 0, 0, 0, 0, 0, 0));
 e.wr8(0x6fbcc4b8, O(e, 10190, 0xff, 0, 0, 0, 0));
 e.wr8(0x6fbcc4b9, O(e, 10190, 0xf4, 0xc0, 0x4c, 0, 0, 0));
 F(e, 0x265E0, 0, 0, 0, 0);
 e.wr8(0x6fbcc4ba, O(e, 10190, 0, 0, 0xff, 0, 0, 0));
}
// 1.13c D2Client+0xBCEA0  stat-panel draw. Ordinal ids as in the decompile; the on-stack draw/text records
// (cel struct, wide strings) live in seam-owned scratch addressed through uintptr_t.
void sub_BCEA0(const Env& e, P scratch) {
 // scratch layout (seam-owned): +0x000 cel-record, +0x100 wide text buf (0x80 wchar), +0x300 ascii buf
 const P cel = scratch, wbuf = scratch + 0x100, abuf = scratch + 0x300;
 int32_t ox = i32(e, gOrgX), oy = i32(e, gOrgY), sh = i32(e, gScrH);
 P pl = e.rd32(gPlayer);
 e.wr32(cel + 0, 0); e.wr32(cel + 4, 5); e.wr32(cel + 8, 0xffffffff);
 O(e, 10041, cel, ox, oy - 0xe0 + sh, 0xffffffff, 5, 0);                 // left panel
 O(e, 10041, cel, ox + 0x100, oy - 0xe0 + sh, 0xffffffff, 5, 0);         // right panel
 O(e, 10041, cel, ox, oy - 0x30 + sh, 0xffffffff, 5, 0);
 O(e, 10041, cel, ox + 0x100, oy - 0x30 + sh, 0xffffffff, 5, 0);
 int32_t cx = i32(e, gCursY), cy = i32(e, gCursX);
 if (ox + 0x80 <= cx && cx <= ox + 0xa0 && oy - 0x5c + sh <= cy && cy <= oy - 0x3c + sh) {
  O(e, 10003, oy - 0x5f + sh, 0, 0, 0, 0, 0);
  O(e, 10085, 0, 0, 0, 0, 0, 0);
 }
 if (O(e, 10587, 0, 4, 0, 0, 0, 0) != 0) {   // stat-points-remaining header
  O(e, 10041, cel, ox + 3, oy - 0x74 + sh, 0xffffffff, 5, 0);
  O(e, 10184, 0, 0, 0, 0, 0, 0);
  O(e, 10003, 0, 0, 0, 0, 0, 0); O(e, 10028, 0, 0, 0, 0, 0, 0); O(e, 10150, oy - 0x7d + sh, 1, 0, 0, 0, 0);
  O(e, 10003, 0, 0, 0, 0, 0, 0); O(e, 10028, 0, 0, 0, 0, 0, 0); O(e, 10150, oy - 0x75 + sh, 1, 0, 0, 0, 0);
  e.sprintfA(abuf, 0 /*"%i"*/, O(e, 10587, 0, 4, 0, 0, 0, 0));
  O(e, 10184, 0, 0, 0, 0, 0, 0);
  e.win2Unicode(wbuf, abuf, 9);
  O(e, 10028, wbuf, 0, 0, 0, 0, 0); O(e, 10150, oy - 0x78 + sh, 0, 0, 0, 0, 0);
 }
 if (O(e, 10587, 0, 4, 0, 0, 0, 0) != 0) {   // +/- stat buttons
  for (P p = gBtns; p < 0x6fb8d84c; p += 0xe) {
   int32_t bx = i32(e, p - 4) + ox, by = oy + sh - 0x1e0 + i32(e, p);
   O(e, 10041, cel, bx + 5, by + 5, 0xffffffff, 5, 0);
   O(e, 10041, (e.rd32(p + 4) != 0), bx + 8, by + 1, 0xffffffff, 5, 0);
  }
 }
 O(e, 10184, 0, 0, 0, 0, 0, 0);
 for (uint32_t k = 0; k < 0x10e; k += 0x12) {   // 15 title strings, split on '\n'
  O(e, 10003, 0, 0, 0, 0, 0, 0);
  e.uStrncpy(wbuf, O(e, 10003, 0, 0, 0, 0, 0, 0), 99);
  int32_t y = i32(e, 0x6fb8d5e4 + k) + oy - 0x1e0 + sh;
  e.wr16(wbuf + 0x2d0 /*+0x11a-0x1e0 slot*/, 0);
  int32_t n = e.uStrlen(wbuf), i = 0;
  bool split = false;
  while (e.rd16(wbuf + i * 2) != 0) {
   if (e.rd16(wbuf + i * 2) == 10) {
    e.wr16(wbuf + i * 2, 0);
    if (i < n) { split = true; O(e, 10028, wbuf + 2, 0, 0, 0, 0, 0); O(e, 10150, y - 4, 0, 0, 0, 0, 0); O(e, 10028, wbuf, 0, 0, 0, 0, 0); O(e, 10150, y + 4, 0, 0, 0, 0, 0); }
    break;
   }
   i++;
  }
  if (!split) { O(e, 10028, wbuf, 0, 0, 0, 0, 0); O(e, 10150, y, 0, 0, 0, 0, 0); }
 }
 if ((e.rd32(0x6fbd34a0) & 0xff) == 0) {      // one-time layout nudge for resolution class 6..9
  uint32_t m = O(e, 10009, 0, 0, 0, 0, 0, 0);
  if (m > 5 && m < 10) { e.wr32(0x6fba52ec, e.rd32(0x6fba52ec) - 1); e.wr32(0x6fba5334, e.rd32(0x6fba5334) - 1); }
  e.wr8(0x6fbd34a0, 1);
 }
 if (pl) {
  F(e, 0x2FD30, pl, O(e, 10828, pl, 0, 0, 0, 0), 0, 0);
  F(e, 0x2FD30, pl, O(e, 10507, pl, 0, 0, 0, 0), 0, 0);
 }
 O(e, 10184, 0, 0, 0, 0, 0, 0);
 sub_BCB90(e, F(e, 0x2E350, pl, 1, 0, 0), 0);
 sub_BCB90(e, F(e, 0x2E350, pl, 0, 0, 0), 0);
 F(e, 0xBC520, 0, 0, 0, 0);
 sub_BC8F0(e, 0, 0, 0, 0);
 F(e, 0xBC760, 0, 0, 0, 0);
 O(e, 10184, 0, 0, 0, 0, 0, 0);
 if (e.rd32(0x6fbcb9c4) == 0) { e.snprintfPath(abuf, 0 /*"%s\\ui\\panel\\buysellbtn"*/, 0); e.wr32(0x6fbcb9c4, F(e, 0x9D630 /*FUN_6fadb420*/, 0, 0, 0, 0)); }
 O(e, 10041, (e.rd32(gBtnDown) != 0) + 10, ox + 0x80, oy - 0x3c + sh, 0xffffffff, 5, 0);
 if (pl && e.rd32(pl) == 0) {                   // player name
  O(e, 10028, 0, 0, 0, 0, 0, 0); O(e, 10150, oy - 0x1c7 + sh, 0, 0, 0, 0, 0);
  if (e.rd32(pl) != 0) { O(e, 10265, 0x5d2, 0, 0, 0, 0, 0); e.fatal(0x5d2); return; }
  P rec = F(e, 0x4D630, e.rd32(pl + 0xc), 0, 0, 0);
  P name = rec ? rec + 0x66 : O(e, 10920, pl, 0, 0, 0, 0);
  if (!name) { O(e, 10265, 0x5db, 0, 0, 0, 0, 0); e.fatal(0x5db); return; }
  P q = name; while (e.rd32(q) & 0xff) q++;
  if (0xb < (uint32_t)e.uWidth(name, (int32_t)(q - (name + 1))) + 1) O(e, 10184, 0, 0, 0, 0, 0, 0);
  e.win2Unicode(wbuf, name, 0x80);
  O(e, 10028, wbuf, 0, 0, 0, 0, 0); O(e, 10150, oy - 0x1c7 + sh, 0, 0, 0, 0, 0);
  O(e, 10184, 0, 0, 0, 0, 0, 0);
 }
 for (uint32_t r = 0; r < 18; r++) {            // stat rows, 0x10-byte records at gRows+0x1c
  P rw = 0x6fb8d6fc - 0x0 + r * 0x10;
  int32_t stat = i32(e, rw), cur = (int32_t)O(e, 10973, pl, stat, 0, 0, 0, 0), base = (int32_t)O(e, 10587, pl, stat, 0, 0, 0);
  int32_t color = 0, val = cur; bool clip = false;
  bool plain = (uint32_t)(stat - 6) > 0x27;
  if (!plain && (stat == 0xd || stat == 0x1e)) {   // text rows
   if (stat == 0xd) O(e, 10006, wbuf, cur, 0x80, 0, 0, 0);
   else {
    P u = pl; int32_t cl = u ? (int32_t)e.rd32(u + 4) : -1;
    if (stat != (int32_t)O(e, 10066, cl, 0, 0, 0, 0)) cur = (int32_t)O(e, 10399, cl, stat, 0, 0, 0);
    O(e, 10006, wbuf, cur, 0x80, 0, 0, 0);
   }
   O(e, 10028, wbuf, 0, 0, 0, 0, 0); O(e, 10150, i32(e, rw - 8) + oy - 0x1e0 + sh, 0, 0, 0, 0, 0);
   continue;
  }
  if (!plain && stat == 0x1f) {                    // defense
   val = (int32_t)O(e, 10672, pl, 0, 0, 0, 0);
   if (O(e, 10404, pl, 0, 0, 0, 0)) color = 3;
   uint32_t item = e.rd32(pl + 0x60);
   if (item && O(e, 10854, item, 0, 0, 0, 0) && O(e, 10494, pl, 0x65, 0, 0, 0)) color = 3;
   if (O(e, 10100, pl, 0, 0, 0, 0)) color = 1;
   clip = val > 999 || O(e, 10009, 0, 0, 0, 0, 0) == 6;
  } else if (!plain && (stat == 0x25 || stat == 0x27 || stat == 0x29 || stat == 0x2b || stat == 0x2d)) {   // resistances
   P rp = O(e, 10237, e.rd32(gCharSel) & 0xff, 0, 0, 0, 0);
   uint32_t sid = 0x26; int32_t extra = 0;
   if (stat == 0x27) { O(e, 10824, pl, 0, 0, 0, 0); if (O(e, 10254, pl, 0, 0, 0, 0)) color = 1; sid = 0x28; }
   else if (stat == 0x29) { O(e, 10843, pl, 0, 0, 0, 0); if (O(e, 10068, pl, 0, 0, 0, 0)) color = 1; sid = 0x2a; }
   else if (stat == 0x2b) { O(e, 11082, pl, 0, 0, 0, 0); if (O(e, 10355, pl, 0, 0, 0, 0)) color = 1; sid = 0x2c; }
   else if (stat == 0x2d) { O(e, 10722, pl, 0, 0, 0, 0); if (O(e, 10370, pl, 0, 0, 0, 0)) color = 1; sid = 0x2e; }
   extra = (int32_t)O(e, 10973, pl, sid, 0, 0, 0);
   int32_t v = cur;
   if (e.rd32(gFlag9854) == 0) { uint32_t d = e.rd32(gCharSel) & 0xff; if (d == 1) v -= 0x14; else if (d == 2) v -= 0x32; }
   else v += (int32_t)e.rd32(rp);
   extra += 0x4b; if (extra > 0x5e) extra = 0x5f;
   if (v < -99) v = -100;
   if (v < extra || (v = extra, color == 3)) { if (v < 0) color = 1; } else color = 4;
   val = v;
  } else if (!plain && stat >= 6 && stat <= 0xb) {  // 6..0xb: >>8 stats
   val = cur >> 8;
   if (stat == 6 && pl && e.rd32(pl) == 0 && e.rd32(pl + 0x10) != 0x11 && val >= 0 && val < 2) val = 1;
   if (stat == 7 || stat == 9 || stat == 0xb || stat == 6) { if (stat != 6 && base < cur) color = 3; else if (stat != 6 && cur < base) color = 1; }
   clip = true;
  } else { if (base < cur) color = 3; else if (cur < base) color = 1; }
  e.sprintfA(abuf, 0 /*"%ld"*/, (uint32_t)val);
  e.win2Unicode(wbuf, abuf, 0x80);
  e.wr16(wbuf + 0xfe, 0);
  uint32_t pushed = 0xe;
  if (clip && (val > 999 || (O(e, 10177, 0, 0, 0, 0, 0), i32(e, rw + 0xc - 0x10 + 0x4) - i32(e, rw - 0xc) <= 0))) pushed = O(e, 10184, 0, 0, 0, 0, 0, 0);
  O(e, 10028, wbuf, 0, 0, 0, 0, 0); O(e, 10150, i32(e, rw - 8) + oy - 0x1e0 + sh, color, 0, 0, 0, 0);
  if (pushed != 0xe) O(e, 10184, 0, 0, 0, 0, 0, 0);
 }
}
}

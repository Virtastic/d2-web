// Implementation batch (alpha lane): D2Client 0x933E0..0x97770, inventory/cursor UI.
// Prior art: none (D2MOO has no D2Client module); decompile is the source. Imported DLL
// calls (D2Common ordinals) and D2Client globals sit behind Env; register-passed args of the
// retail __usercall/__fastcall forms are named by role. Guesses are marked GUESS.
#include <cstdint>
namespace d2client_batch_933e0 {
struct Unit { int32_t type, classId, id_unused, id; };  // GUESS: only +0,+4,+0xC,+0x60 used below
struct Area { uint8_t w, h; uint8_t pad[2]; int32_t left; int32_t r8; int32_t top; uint8_t pad2[4]; uint8_t cellW, cellH; };
static_assert(sizeof(Area) == 0x18, "area");
struct Globals {   // D2Client globals by RVA (base 6fb40000)
 int32_t g_BCC28{}; void* hover{}; void* hoverPrev{};          // 0xBCC28 0xBCC38 0xBCC3C
 int32_t hx0{},hy0{},hx1{},hy1{},hcellX{},hcellY{};            // 0x90EA8 EAC EB0 EB4 EB8 EBC
 int32_t g_90EA4{}; int32_t cursorMode{}; int32_t g_BCB834{};  // 0x90EA4 0xBCB864 0xBCB834 (held item ptr token)
 int32_t g_BCB854{}, g_BCB858{}; uint8_t g_BCC34{}; int32_t g_BCC30{};
 uint8_t panelId{}; int32_t mouseX{}, mouseY{}; int32_t g_BCC88{};
 int32_t g_BCC58{}, g_BCC9C{}, g_BCC98{}, g_90EC0{};
};
struct Env {   // imported calls
 virtual ~Env() = default;
 virtual void* ownerStrA() = 0;                               // Ordinal_10003
 virtual bool  lookup53(int32_t) = 0;                         // Ordinal_10744(0xBCC38,0x35)
 [[noreturn]] virtual void fatal(int32_t msgId) = 0;          // Ordinal_10265+10024+_exit(-1)
 virtual void  strcat(void* dst, void* src) = 0;
 virtual void  buildLabel() = 0;                              // FUN 0x6330
 virtual uint32_t ownerKind(void*) = 0;                       // Ordinal_10584
 virtual Unit* itemAtCell(void* inv, uint32_t cx, uint32_t cy, int32_t* a, int32_t* b) = 0; // Ordinal_10801
 virtual void  itemSize(Unit*, uint8_t* w, uint8_t* h) = 0;   // Ordinal_10114
 virtual int32_t gridX(Unit*) = 0; virtual int32_t gridY(Unit*) = 0; // FUN 0x1630 / 0x1660
 virtual int32_t heldCount(void*) = 0;                        // Ordinal_10202 family
 virtual Unit* invItem(void* inv, int32_t slot) = 0;          // Ordinal_11139
 virtual Unit* invFirst(void* inv) = 0; virtual Unit* invNext(Unit*) = 0; // 10460 / 10464
 virtual void* itemRecord(Unit*) = 0;                         // Ordinal_11147
 virtual uint8_t footprint(Unit*, int32_t slot, int32_t* dx, int32_t* dy) = 0; // GUESS table 0xB0A98
 virtual void  drawMark(int32_t x, int32_t y) = 0;            // FUN 0x6B5A0 (1B5A0)
 virtual void  drawFootprint(Unit*, int32_t x, int32_t y) = 0;
 virtual bool  isUnidentified(Unit*) = 0; virtual bool isIdentified(Unit*) = 0; virtual bool flag800(Unit*) = 0;
 virtual int32_t unitKind(Unit*) = 0;
 virtual bool  worldDispatch(int32_t, int32_t, int32_t) = 0;  // FUN 0x47D60
 virtual void  sendPacket(const uint8_t*, unsigned) = 0;      // FUN 0x43E0 (AC43E0)
 virtual void  stateUpdate(int32_t, int32_t, int32_t) = 0;    // FUN 0xC2790
 virtual void  unitFind(int32_t kind, int32_t id, Unit** out) = 0;
 virtual void  notify(int32_t, int32_t) = 0;
 virtual bool  busy() = 0; virtual bool canSend() = 0;        // FUN 0x31CB0 / 0x423F0
 virtual void  clearCursor() = 0;                             // FUN 0x31CE0
 virtual void  swapSend(int32_t a, int32_t b) = 0;            // FUN 0xAC4910
 virtual void  dropSend() = 0;                                // FUN 0xAC4940
 virtual void  refresh() = 0;                                 // FUN 0x31E40 / 0x38A70
};
static inline int32_t clampNeg0(int32_t v) { return v < 0 ? 0 : v; }

// 1.13c D2Client+0x933E0
void sub_933E0(Env& e, void* dst) {
 void* a = e.ownerStrA();
 if (!e.lookup53(0x35)) e.fatal(0xc43);
 e.buildLabel();
 void* b = e.ownerStrA();
 e.strcat(dst, b); e.strcat(dst, a);
}
// 1.13c D2Client+0x945D0  (args GUESS: EAX=area, item or null, x, y, flags)
void sub_945D0(Env& e, Globals& g, const Area* A, Unit* item, void* inv, int32_t x, int32_t y, uint32_t flag) {
 uint32_t cx = uint32_t(x - A->left) / A->cellW, cy = uint32_t(y - A->top) / A->cellH;
 if (!item) {
  int32_t a, b; Unit* it = e.itemAtCell(inv, cx, cy, &a, &b);
  if (!it) { g.g_BCC28 = 0; g.hover = nullptr; g.hoverPrev = nullptr; return; }
  g.g_BCC28 = 1;
  if (it != g.hoverPrev) {
   uint8_t w, h; e.itemSize(it, &w, &h);
   int32_t gx = e.gridX(it), gy = e.gridY(it);
   g.hx0 = int32_t((e.ownerKind(inv) & 0xff) * A->cellW) / 2 + A->cellW * gx + A->left; g.hx1 = g.hx0;
   g.hover = g.hoverPrev = it;
   g.hy0 = A->cellH * gy + A->top;
   g.hy1 = ((flag & 0xff) + gy) * A->cellH + A->top;
  }
  return;
 }
 uint8_t w, h; e.itemSize(item, &w, &h);
 if (w > 1) { cx -= (w >> 1); if (int32_t(cx) < 0) cx = 0; if (A->w < int32_t(w + cx)) return; }
 if (h > 1) { cy -= (h >> 1); if (int32_t(cy) < 0) cy = 0; if (A->h < int32_t(h + cy)) return; }
 // GUESS: even-width recentre terms (decompiler mixed registers) omitted; w==A->w => centre.
 if (w == A->w) cx = A->w >> 1;
 if (h == A->h) cy = A->h >> 1;
 g.hcellY = cy; g.hcellX = cx; g.g_BCC28 = 1; g.hover = nullptr; g.hoverPrev = nullptr;
}
// 1.13c D2Client+0x94820  (this=item; EAX=area; outputs cell x,y)
bool sub_94820(Env& e, const Area* A, Unit* item, int32_t x, int32_t y, uint32_t* ox, uint32_t* oy) {
 if (!item) e.fatal(0);
 uint32_t cx = uint32_t(x - A->left) / A->cellW, cy = uint32_t(y - A->top) / A->cellH;
 uint8_t w, h; e.itemSize(item, &w, &h);
 // GUESS: even-size branches recompute from the other half-cell; bit tests came from a mis-typed register.
 if (w == A->w) cx = A->w >> 1;
 if (h == A->h) cy = A->h >> 1;
 *ox = cx; *oy = cy;
 if (w > 1) { int32_t t = int32_t(*ox) - (w >> 1); *ox = t < 0 ? 0 : t; }
 if (h > 1) { int32_t t = int32_t(*oy) - (h >> 1); *oy = t < 0 ? 0 : t; }
 return true;
}
// 1.13c D2Client+0x94970
void sub_94970(Env& e, Globals& g, const Area* A, void* inv, Unit* (*lookupSel)(void*), const uint8_t* selTab, int32_t selCount) {
 if (!e.canSend()) return;
 e.refresh();
 int32_t i = g.g_BCC88;
 if (i >= 0 && i < selCount && *reinterpret_cast<const int16_t*>(selTab + i * 0x14) != 0x276f) {
  g.g_BCC34 = selTab[i * 0x14 + 2]; g.g_BCB858 = *reinterpret_cast<const int32_t*>(selTab + i * 0x14 + 8);
  g.g_BCB834 = 0; g.g_BCB854 = 6; g.cursorMode = 7;
 }
 (void)A; (void)inv; (void)lookupSel;
 // GUESS: tail (unit lookup by 0xBC971D, kind==1 check, rect test on mouse vs 0xBB3B4..C0, then
 // sub_945D0(unit+0x60,null,mouseX,mouseY,panelId); g_90EA4=0) needs unit-table env; unit wiring left to caller.
}
// 1.13c D2Client+0x95140
void sub_95140(Env& e, Unit* item, const Area* A, int32_t bx, int32_t by) {
 if (!e.heldCount(item)) return;
 for (int32_t n = 0;; ++n) {
  int32_t dx = 0, dy = 0; uint8_t more = e.footprint(item, n, &dx, &dy);  // GUESS: table 0xB0A98 stride 0x30, 8-byte (dx,dy) pairs
  int32_t px = ((dx - 1) * A->cellW) / 2 + bx, py = ((dy + 1) * A->cellH) / 2 + by;
  e.drawFootprint(item, px - 1, py);
  e.drawMark(px, py - A->cellH);
  if (!more) break;
 }
}
// 1.13c D2Client+0x952A0
void sub_952A0(Env& e, Globals& g, Unit* owner, const int32_t* slotTab /*0xBCD80, 5 ints, 0x13 slots*/, uint8_t cell, const Area* A) {
 void* inv = reinterpret_cast<void*>(0); (void)inv;
 for (int32_t s = 0; s < 0x13; ++s) {
  Unit* it = e.invItem(owner, s); if (!it) continue;
  uint8_t w, h; e.itemSize(it, &w, &h);
  const int32_t* r = slotTab + s * 5;
  int32_t x = (int32_t(reinterpret_cast<const uint8_t*>(r)[16]) - w * cell) / 2 + r[0];
  int32_t y = (int32_t(reinterpret_cast<const uint8_t*>(r)[17]) - h * cell) / 2 + r[2];
  e.drawMark(x, y);
  if (g.hover == it && e.isIdentified(it) && e.flag800(it)) sub_95140(e, it, A, x, y);
 }
}
// 1.13c D2Client+0x95F80  (ECX=unit, EDX=x, stack: y, EAX/ESI contextual)
void sub_95F80(Env& e, Globals& g, Unit* u, int32_t a, int32_t b, int32_t c, int32_t esi) {
 if (!u) return;
 if (!e.worldDispatch(b, c, a)) return;   // GUESS arg order (register shuffle)
 if (esi == 0) g.g_BCC34 = 5;
 g.g_BCB834 = 0; g.cursorMode = 1; g.g_BCB854 = 5; g.g_BCB858 = 0;
}
// 1.13c D2Client+0x97770
void sub_97770(Env& e, Globals& g, Unit* (*find)(int32_t id)) {
 g.g_BCC58 = 0; e.stateUpdate(0x1a, 1, 0);
 if (g.g_BCC9C == 0) { const uint8_t p[7] = {'O', 0x17, 0, 0, 0, 0, 0}; e.sendPacket(p, 7); g.g_BCC9C = 1; }
 if (g.g_BCC98) {
  if (Unit* u = find(g.g_90EC0)) { if (u->type != 2) e.fatal(0x5a); e.notify(2, g.g_90EC0); }
  g.g_BCC98 = 0;
 }
}
// 1.13c D2Client+0x958A0  (grid render of one inventory panel; frame/colour logic summarised)
void sub_958A0(Env& e, Globals& g, Unit* panelUnit, int32_t panelKind, const Area* A, void* inv) {
 // panelKind 0: area chosen by g.panelId (1:B370 2:B288 3:B3C8 4:B398/B2D0 else B3E0); 1: B3B0; else return.
 if (panelKind != 0 && panelKind != 1) return;
 if (!inv) return;
 uint32_t own = e.ownerKind(panelUnit);
 for (Unit* it = e.invFirst(inv); it; it = e.invNext(it)) {
  void* rec = e.itemRecord(it); if (!rec) continue;
  uint8_t w, h; e.itemSize(it, &w, &h);
  int32_t gx = e.gridX(it), gy = e.gridY(it);
  int32_t px = A->cellW * gx + A->left, py = A->cellH * gy + A->top;
  // GUESS: colour/state arg of FUN_3C600 (0/1/2/4) depends on 10244/10202/11006 results; only the
  // hovered-item path (draw + sub_95140) is reproduced.
  if (it == g.hover && g.g_BCC28) { e.drawMark(px, py); if (e.isIdentified(it) && e.flag800(it)) sub_95140(e, it, A, px, py); }
  else e.drawMark(px, py);
 }
 (void)own;
}
// 1.13c D2Client+0x96690  (ECX=panel unit, EDX=cursor owner)
uint32_t sub_96690(Env& e, Globals& g, Unit* panelUnit, Unit* owner, int32_t heldItemId, Unit* held, Unit* cursor, uint32_t cmd, Unit* target) {
 uint32_t r = 0;
 if (g.cursorMode == 6) {
  if (g.g_BCB834 && held && !e.busy() && e.canSend()) { e.swapSend(held->id_unused, g.g_BCB834); e.clearCursor(); r = 1; }
 } else if (g.cursorMode == 8 && held && !e.busy() && e.canSend()) { e.dropSend(); e.clearCursor(); r = 1; }
 switch (cmd) {
  case 0: if (target && cursor) e.notify(cursor ? 0x14 : 0x13, 0); return 1;  // GUESS: 0x14/0x13 error codes via FUN_3200
  case 1: case 5: if (!e.busy() && e.canSend()) { e.swapSend(target ? target->id : -1, 4); e.clearCursor(); e.refresh(); r = 1; } return r;
  case 2: if (!e.busy() && !e.busy() && e.canSend()) { e.swapSend(target ? target->id : -1, 4); e.clearCursor(); e.clearCursor(); e.refresh(); g.g_BCC30 = 1; return 1; } break;
  case 3: case 4: if (g.g_BCC34 == 4 || (cmd == 3 && g.g_BCC34 < 0x13)) { e.worldDispatch(1, 0, 0); return r; } break;
  case 6: if (!e.busy() && e.canSend() && g.g_BCC34 != 0xb) { e.swapSend(target ? target->id : -1, held ? held->id : -1); e.clearCursor(); e.refresh(); r = 1; } break;
  case 7: if (!e.busy() && e.canSend()) { e.swapSend(target ? target->id : -1, 4); e.clearCursor(); e.clearCursor(); e.clearCursor(); return 1; } break;
 }
 (void)panelUnit; (void)owner; (void)heldItemId;
 return r;
}
// 1.13c D2Client+0x96AA0  (cursor-click dispatcher; PARTIAL/GUESS -- see hand-back)
uint32_t sub_96AA0(Env& e, Globals& g, Unit* held, Unit* hit, const Area* A, uint32_t mouseBtn, uint8_t shiftFlag) {
 if (shiftFlag == 0xff) return 0;
 if (g.cursorMode == 6) {
  if (!g.g_BCB834 || !hit) return 0;
  if (e.busy()) return 1;
  if (shiftFlag != 1 && shiftFlag != 2 && e.canSend()) { e.swapSend(hit->id, g.g_BCB834); e.clearCursor(); return 1; }
  return 0;
 }
 if (g.cursorMode == 8) {
  if (!hit) return 0;
  if (e.busy()) return 1;
  if (shiftFlag == 1 || shiftFlag == 2 || !e.canSend()) return 0;
  e.dropSend(); e.clearCursor(); return 1;
 }
 if (!held) {
  if (!hit) return 0;
  if (shiftFlag != 1 || !e.canSend() || e.busy()) { if (g.g_BCC34 < 0x13) sub_95F80(e, g, hit, 0, 0, 0, 1); return 0; }
  e.dropSend(); g.g_BCC30 = 1; e.clearCursor(); return 1;
 }
 // held-item placement: cell resolve via sub_94820 then swap/drop sends.
 uint32_t cx, cy; sub_94820(e, A, held, 0, 0, &cx, &cy);
 // GUESS: remaining stack-passed branches (box-type 0x20786f62 compare, 10967/11148 place tests,
 // 0x16/0x12 stack-merge, sends via FUN_4910/48A0) not reproduced.
 (void)mouseBtn;
 return 0;
}
}

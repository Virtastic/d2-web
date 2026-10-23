// Recovered from the decompile of D2Client+0x953B0 (inventory paper-doll slot highlights/marks).
// Register args (__usercall): EBX=offX, EBP=offY are named x0/y0 (GUESS: swapped roles are possible; the
// decompile adds EBP to x and EBX to y, kept as is). Slot table stride 0x14 at D2Client+0x11BCA0.
#include <cstdint>
namespace d2client_953b0 {
struct Slot { int32_t x; int32_t _4; int32_t y; int32_t _c; uint8_t w, h, pad[2]; };  // x@0xBCCA0+i*0x14, y@+8, w/h @+0x10/+0x11
static_assert(sizeof(Slot) == 0x14, "slot");
struct Globals {
 int32_t splitMode{}, altSet{};           // 0xBC9854 0xBCBC94 (-> mode -4)
 void* owner{};                           // 0xBCBBFC
 Slot slot[11]{};                         // 0xBCBCA0
 uint8_t rA[2]{}, rB[2]{};                // 0xBCBD00/01 (slot4 extra), 0xBCBD14/15 (slot5 extra)
 int32_t aX{}, aY{}, bX{}, bY{};          // 0xBCBD04 0xBCBD0C ; 0xBCBCF0 0xBCBCF8
 uint8_t cell[2]{};                       // 0xBBB3F4 (cell w,h)
 int32_t scrW{}, scrH{};                  // 0xA9E14 0xA9E18
 void* hover{}; int32_t hoverOn{};        // 0xBCC38 0xBCC2C
};
struct Env {
 virtual ~Env() = default;
 virtual void* inv(void* owner) = 0;                              // *(owner+0x60)
 virtual void prepare(void* owner, int32_t zero, int32_t mode) = 0; // Ordinal_10584 / FUN 0x8C0 (see spec)
 virtual void* itemAt(void* inv, int32_t slot) = 0;               // Ordinal_11139
 virtual void itemSize(void* it, uint8_t* w, uint8_t* h) = 0;     // Ordinal_10114
 virtual int32_t ownerClass(void* owner, void* inv) = 0;          // Ordinal_10051
 virtual bool twoHandedOrBlocked(void* it) = 0;                   // Ordinal_11144
 virtual void drawHighlight(int32_t col) = 0;                     // FUN 0x18970
 virtual void drawMark(int32_t x, int32_t y) = 0;                 // FUN 0x1B5A0
 virtual bool hasInv(void* it) = 0;                               // Ordinal_10757
 virtual bool isSocketed(void* it) = 0;                           // Ordinal_11151
 virtual bool flag(void* it, uint32_t mask, int line) = 0;        // Ordinal_10202 (inv.cpp)
 virtual void drawHeld(void* it, int32_t x, int32_t y) = 0;       // FUN 0x45140
 virtual bool isQuest(void* it) = 0;                              // Ordinal_10543
 virtual bool canUse(void* it, void* ctx) = 0;                    // Ordinal_10244
 virtual bool statFlag(void* it, int32_t v) = 0;                  // Ordinal_10195
 virtual bool ownerState(void* owner, int32_t st) = 0;            // Ordinal_10494
 virtual bool isEquippable(void* it) = 0;                         // Ordinal_11006
 virtual void finish() = 0;                                       // FUN 0x3E2F0
};
static bool inScr(const Globals& g, int32_t x, int32_t y) { return x > -1 && x < g.scrW && y > -1 && y < g.scrH; }

// 1.13c D2Client+0x953B0
void sub_953B0(Env& e, Globals& g, int32_t ebx, int32_t ebp, void* ctx) {
 int32_t mode = (g.splitMode && g.altSet) ? -4 : 0;
 void* inv = e.inv(g.owner);
 e.prepare(g.owner, 0, g.splitMode);
 for (int32_t i = 0; i < 11; i++) {
  void* it = e.itemAt(inv, i);
  if (!it) continue;
  uint8_t w = 0, h = 0; e.itemSize(it, &w, &h);
  int32_t cls = e.ownerClass(g.owner, inv);
  void* other = nullptr;
  auto sx = [&](int32_t a) { return ((int32_t)a - g.cell[0] * w) >> 1; };
  auto sy = [&](int32_t a) { return ((int32_t)a - g.cell[1] * h) >> 1; };
  if (i == 4) {
   other = e.itemAt(inv, 5);
   if (cls == 2 && !e.twoHandedOrBlocked(other) && !e.twoHandedOrBlocked(it)) {
    int32_t px = sx(g.rA[0]) + g.aX + ebp, py = sy(g.rA[1]) + g.aY + ebx;
    e.drawMark(px, py);
    if (g.aX > -1 && g.aX < g.scrW && g.aY > -1 && g.aY < g.scrH) e.drawHighlight(g.aX);
   }
  } else if (i == 5) {
   other = e.itemAt(inv, 4);
   if (cls == 2 && !e.twoHandedOrBlocked(other) && !e.twoHandedOrBlocked(it)) {
    int32_t px = sx(g.rB[0]) + g.bX + ebp, py = sy(g.rB[1]) + g.bY + ebx;
    e.drawMark(px, py);
    if (g.bX > -1 && g.bX < g.scrW && g.bY > -1 && g.bY < g.scrH) e.drawHighlight(g.bX);
   }
  }
  int32_t px = sx(g.slot[i].w) + g.slot[i].x, py = sy(g.slot[i].h) + g.slot[i].y;
  switch (i) {
   case 3: px += 3; break;
   case 4: case 5: px += ebp; py += ebx; break;
   case 8: px += 2; /*fallthrough*/
   case 2: case 6: case 7: case 10: py -= 2; break;
   case 9: px += 2; py -= 4; break;
  }
  (void)mode;
  const Slot& s = g.slot[i];
  bool sIn = inScr(g, s.x, s.y);
  if (it == g.hover && g.hoverOn) {
   if (sIn) e.drawHighlight(s.x);
   e.drawMark(px, py);
   if (e.hasInv(it) && e.isSocketed(it) && e.flag(it, 0x800, 0x673)) e.drawHeld(it, px, py);
  } else {
   bool plain = (!other && (e.isQuest(it) || e.twoHandedOrBlocked(it))) || !e.canUse(it, ctx) ||
                e.statFlag(it, 2) || (g.owner && e.ownerState(g.owner, 0x36)) || !e.isEquippable(it);
   if (plain) { if (sIn) e.drawHighlight(s.x); }
   else if (!e.flag(it, 0x10, 0x682) && sIn) e.drawHighlight(s.x);
   e.drawMark(px, py);
  }
 }
 e.finish();
}
}  // namespace d2client_953b0

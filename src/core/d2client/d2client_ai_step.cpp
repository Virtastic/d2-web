// D2Client monster-AI helpers 0x16EB0, 0x173E0, 0x17500.
// Decompile-derived from the cached Ghidra output; no D2Client prior art exists.
// Imported DLL calls (ordinals / other RVAs) go through Host; register-passed args are explicit params.
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace d2client_batch_a {
struct Unit { int32_t type, classId, id, mode, mode2, data, pad6, pad7; uint32_t seedLo, seedHi; int32_t pad10; int32_t path; };
static_assert(offsetof(Unit, mode2) == 0x10 && offsetof(Unit, data) == 0x14 && offsetof(Unit, seedLo) == 0x20 && offsetof(Unit, path) == 0x2C, "unit layout");
struct Host {
 virtual ~Host() = default;
 virtual int32_t unitX(Unit*) = 0;            // Ordinal_10867
 virtual int32_t unitY(Unit*) = 0;            // Ordinal_10750
 virtual int32_t pathX(int32_t path) = 0;     // *(path+0xC)
 virtual int32_t pathY(int32_t path) = 0;     // *(path+0x10)
 virtual int32_t room(int32_t x, int32_t y, int32_t extra) = 0;           // Ordinal_10331 (+FUN_1400)
 virtual int16_t collide(int32_t room, int32_t x, int32_t y, int32_t size, int32_t mask) = 0; // Ordinal_10573
 virtual void spawn(int32_t, int32_t, int32_t, int32_t) = 0;              // FUN_B57BF0
 virtual void walk(Unit*, int32_t* target) = 0;                           // FUN_AFDC70
 virtual int32_t pathFlags(Unit*) = 0, pathDir(Unit*) = 0;                // Ordinal_10471 / 11058
 virtual uint32_t* aiState(Unit*) = 0;                                    // data+0x30 slot
 virtual uint32_t* stateBlock(int32_t obj) = 0;                           // *(obj->path? +0x30)
};
// LCG shared by every function here: 64-bit seed {lo,hi}; next = lo*0x6ac690c5 + hi.
static uint32_t rnd(Unit* u) {
 uint64_t v = (uint64_t)u->seedLo * 0x6ac690c5u + u->seedHi;
 u->seedLo = (uint32_t)v; u->seedHi = (uint32_t)(v >> 32);
 return (uint32_t)v;
}
static bool isPathUnit(const Unit* u) { return u->type == 2 || (u->type > 3 && u->type < 6); }
static int32_t ux(Host& h, Unit* u) { return isPathUnit(u) ? h.pathX(u->path) : u->path ? h.unitX(u) : 0; }
static int32_t uy(Host& h, Unit* u) { return isPathUnit(u) ? h.pathY(u->path) : u->path ? h.unitY(u) : 0; }
// 1.13c D2Client+0x16EB0 : up to 10 attempts, scatter +-2 around (x,y); spawn on free cell
int32_t sub_16EB0(Host& h, Unit* u, int32_t x, int32_t y, int32_t room, int32_t count) {
 int32_t tries = 0;
 do {
  int32_t px = (int32_t)(rnd(u) % 5) - 2 + x;
  int32_t py = (int32_t)(rnd(u) % 5) - 2 + y;
  if (h.collide(h.room(px, py, 0), px, py, (int8_t)u->mode2, 0x3f11) == 0) { h.spawn(room, px, py, 1); --count; }
 } while (count > 0 && ++tries < 10);
 return count;
}
// 1.13c D2Client+0x173E0 : step by (+-mag, +-mag) from current position
void sub_173E0(Host& h, Unit* u, uint32_t mag, uint32_t aux) {
 int32_t x = ux(h, u), y = uy(h, u);
 uint32_t a = mag & 0xff, b = a;
 if (rnd(u) & 1) b = -a;
 if (rnd(u) & 1) a = -a;
 int32_t t[6] = {(int32_t)b + x, (int32_t)a + y, 0, h.pathFlags(u), (int32_t)(h.pathDir(u) & 0xff), 0};
 (void)aux; h.walk(u, t);
}
// 1.13c D2Client+0x17500 : step one cell toward target unit t, true if the cell was free
int32_t sub_17500(Host& h, Unit* self, Unit* t, int32_t x, int32_t y, uint8_t step) {
 int32_t sx = ux(h, self), sy = uy(h, self), tx = ux(h, t), ty = uy(h, t);
 int32_t dx = (sx < tx) ? -1 : (tx < sx ? 1 : 0), dy = (sy < ty) ? -1 : (ty < sy ? 1 : 0);
 if (h.collide(h.room(x + dx, y + dy, 1), x + dx, y + dy, 0, 0) != 0) return 0;
 int32_t t2[6] = {x + dx * step, y + dy * step, 0, h.pathFlags(self), (int32_t)(h.pathDir(self) & 0xff), 0};
 h.walk(self, t2); return 1;
}
}

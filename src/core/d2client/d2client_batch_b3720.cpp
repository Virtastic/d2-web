// Prior art used: ; D2MOO has no D2Client module. Derived from cached 1.13c decompile.
// Unrecovered callees are Env function pointers; register args are explicit params (guesses noted).
// Spawn-request word indices follow stack offsets in the decompile and are approximate where noted.
#include <cstdint>
#include <cstring>
namespace d2client_b3720 {
using U = uint32_t; using I = int32_t; using W = uintptr_t;
struct StaticPath { I w[5]; };                       // +0xC / +0x10 read for unit types 2,4,5
struct Unit { I type, classId, f2, id, f4, f5, f6, f7; U seedLo, seedHi; I f10; void* path; };
struct Req { W w[23]; };
struct Table3 { I a, b, c; };
struct Globals { U tick, z0, four000, z1, v19; };
struct EnvB3720 {
 W (*getRoom)(W);                                  // FUN_6fb560d0
 W (*spawn)(Req*);                                 // FUN_6fb630f0 -> unit
 I* (*motion)(W sp);                               // (sp[0x15])->+0x30 block or null
 void (*freeUnit)(Unit*);                          // FUN_6fb617c0
 U (*o10240)(Unit*); U (*o11029)(Unit*);
 I (*pathX)(void*); I (*pathY)(void*);             // Ordinal_10867 / 10750
 I (*level)(Unit*);                                // Ordinal_10737 / 10985
 U (*o10818)(Unit*); U (*o11018)(Unit*); void (*o10614)();
 void (*o10637)(W, I); void (*o10019)(W, I); void (*o10558)(W, I); void (*o10970)(W, I);
 void (*o10154)(W, I); void (*o10706)(W, I); void (*o10520)(W, I); void (*o10441)(W, I);
 W (*o10331)(); I (*o10482)(W, I, I, I); I (*o10573)();
 W (*findUnit)(I type, U id);                      // hash lookup, chain via +0xE4
 void (*fatal)(U msgId);                           // Ordinal_10265+10024+_exit; not [[noreturn]]
 const uint8_t* (*dataRow)(I idx);                 // sgptDataTables+0xb64, stride 0x1a4, null if out of range
 I (*cellShort)(U, I);                             // B3720 w2 read (+0x2c short), guess
 void (*f1b300)(W, I, I, I, I); W (*alloc3c)(); void (*f23b60)(W); void (*sub617c0b)(Unit*);
 void (*effect630f0)(Req*);
 void (*setMonsterAlloc)(W, W);
 const Table3* tbl4760;                            // 0x6fb8364c, up to 4+1 triples
 const Table3* tbl4b80;                            // 0x6fb836d0, 7 triples
 Globals* g; U (*tick)();
};
static U rng(Unit& u) { uint64_t v = uint64_t(u.seedLo) * 0x6ac690c5u + u.seedHi; u.seedLo = U(v); u.seedHi = U(v >> 32); return u.seedLo; }
static bool st(const Unit& u) { return u.type == 2 || (u.type > 3 && u.type < 6); }
static I ux(EnvB3720& e, Unit& u) { if (st(u)) return static_cast<StaticPath*>(u.path)->w[3]; return u.path ? e.pathX(u.path) : 0; }
static I uy(EnvB3720& e, Unit& u) { if (st(u)) return static_cast<StaticPath*>(u.path)->w[4]; return u.path ? e.pathY(u.path) : 0; }
static void ids(EnvB3720& e, Unit& u, Req& r, int a, int b) { r.w[a] = e.o10240(&u); r.w[b] = e.o11029(&u); }
static void vel(EnvB3720& e, W sp, I x1, I y1, I z1, I x2, I y2, I z2, bool both) {
 if (I* m = e.motion(sp)) { m[1] = x1; m[2] = y1; m[3] = z1; }
 if (both) if (I* m = e.motion(sp)) { m[4] = x2; m[5] = y2; m[6] = z2; }
}
// 1.13c D2Client+0xB3720
U sub_B3720(EnvB3720& e, Unit* u, U rA /*FUN_1330 result guess*/) {
 if (!u || u->classId < 0) return 1;
 const uint8_t* row = e.dataRow(u->classId);
 int16_t s; if (!row) return 1; std::memcpy(&s, row + 0x2c, 2); if (s < 0) return 1;
 W room = e.getRoom(0); if (!room) return 1;
 e.o10614();
 I sz = 1; I r6c; std::memcpy(&r6c, row + 0x6c, 4); if (r6c > 0) sz = r6c;
 I x = ux(e, *u), y = uy(e, *u);
 U r3 = rng(*u); (void)rng(*u); (void)rA;
 Req r{}; r.w[0] = 0x20; r.w[1] = room; ids(e, *u, r, 10, 12);
 r.w[2] = W(I(e.cellShort(r3, y - sz))); r.w[5] = W(U(r3 + U(x - sz)));
 e.spawn(&r); return 1;
}
// 1.13c D2Client+0xB3860
void sub_B3860(EnvB3720& e, Unit* u) {
 if (e.level(u) % 3 == 0) {
  U a = rng(*u), b = rng(*u);
  I x = ux(e, *u) + I(rng(*u) & 1) - 1;
  I y = uy(e, *u) + I(rng(*u) & 1) - 1;
  Req r{}; r.w[0] = 0x21; r.w[1] = e.getRoom(0);
  r.w[7] = W(I(a % 0x14) - 10 + x); r.w[8] = W(y - 10 + I(b % 0x14));
  r.w[5] = W(x); r.w[6] = W(y); ids(e, *u, r, 10, 12); r.w[4] = 0x21e; e.spawn(&r);
 }
 e.freeUnit(u);
}
// 1.13c D2Client+0xB39E0  (esi=unit seed owner, ebp=flag guess)
void sub_B39E0(EnvB3720& e, I count, I y, Unit* esi, U ebp, I x) {
 if (!esi || !count) return; W p = e.o10331(); if (!p) return;
 do {
  if (count > 3) { if (ebp) return; break; }
  I dx = 0x11 - I(rng(*esi) % 0x22), dy = 0x11 - I(rng(*esi) % 0x22);
  if (dx < -1 || dx > 1 || dy < -1) {
   dy += y;
   if (e.o10482(p, dx + x, dy, 4) == 0) { ebp = 0; x += dx; y = dy; }
  }
  count++;
 } while (ebp);
 Req r{}; r.w[0] = 1; ids(e, *esi, r, 8, 10); r.w[2] = 0x267; r.w[3] = W(x);
 e.spawn(&r); r.w[2] = 0x268; e.spawn(&r);
}
// 1.13c D2Client+0xB3B40  (eax=unit)
void sub_B3B40(EnvB3720& e, Unit* eax, W room) {
 e.o10331(); if (e.o10573() != 0) return;
 Req r{}; r.w[0] = 0x2001; e.getRoom(0); rng(*eax); r.w[1] = room;
 ids(e, *eax, r, 10, 12); r.w[5] = 1; r.w[6] = 3;
 W sp = e.spawn(&r); if (!sp) return;
 if (I* m = e.motion(sp)) { m[1] = 0; m[2] = 0; m[3] = 0x7d000; }
 if (I* m = e.motion(sp)) { m[4] = 0; m[5] = 0; m[6] = I(0xffffb000); }
 e.o10558(sp, 0x19); e.o10970(sp, 0x19); e.o10154(sp, 0x80);
}
// 1.13c D2Client+0xB3C40  (eax=unit)
void sub_B3C40(EnvB3720& e, Unit* eax, W room) {
 Req r{}; r.w[1] = room; r.w[0] = 2; r.w[4] = 0x11d;
 I a = 0x14 - I(rng(*eax) % 0x28), b = 0x14 - I(rng(*eax) % 0x28);
 r.w[7] = W(a); r.w[8] = W(b); ids(e, *eax, r, 10, 12);
 if (a == 0 && b == 0) { a = I(rng(*eax) % 0x13) + 1; r.w[6] = W(a); }
 W sp = e.spawn(&r); if (!sp) return;
 e.o10637(sp, a); e.o10019(sp, b);
 if (I* m = e.motion(sp)) { m[1] = 0; m[2] = 0; m[3] = I(0xffffb000); }
}
// 1.13c D2Client+0xB3D60  (eax=unit, room=param_1, ebp=speed guess)
void sub_B3D60(EnvB3720& e, Unit* eax, W room, I ebp) {
 static const I t[8] = { 1, 0, -1, 0, 0, 1, 0, -1 };
 rng(*eax); U d = rng(*eax) & 3;
 I x = ux(e, *eax), y = uy(e, *eax);
 I dx = t[d], dy = t[d + 4];
 static const U codes[4] = { 0x260, 0x261, 0x262, 0x263 };
 U m = rng(*eax) % 0x1e; U code = m < 4 ? codes[m] : 0x25f;
 Req r{}; r.w[0] = 3; r.w[1] = room; r.w[4] = code; r.w[5] = W(dx + x); r.w[6] = W(dy + y);
 ids(e, *eax, r, 10, 12); r.w[6] = W(-dx);
 W sp = e.spawn(&r); if (!sp) return;
 if (I* mo = e.motion(sp)) { mo[1] = 0; mo[2] = 0; mo[3] = ebp << 11; }
 e.o10637(sp, ux(e, *eax)); e.o10019(sp, uy(e, *eax));
}
// 1.13c D2Client+0xB3FC0  (this=room, eax=unit, ebp/esi/edi = vel guesses)
void sub_B3FC0(EnvB3720& e, W room, I code, Unit* eax, I ebp, I esi, I edi) {
 I tmp[2] = { 0, 0 };
 if (esi == 0 && ebp == 0) { e.fatal(0x1850); return; }
 Req r{}; r.w[0] = 1; r.w[1] = room; r.w[4] = W(code); ids(e, *eax, r, 10, 12);
 r.w[5] = W(ux(e, *eax)); r.w[5] = W(I(rng(*eax) % 0x14) - 10 + I(r.w[5]));
 r.w[6] = W(uy(e, *eax)); r.w[6] = W(I(rng(*eax) % 0x14) - 10 + I(r.w[6]));
 W sp = e.spawn(&r); if (!sp) return;
 const uint8_t* row = e.dataRow(code); int16_t s = 0; if (row) std::memcpy(&s, row + 0x96, 2);
 if (s != 0x19) { e.fatal(0x1861); return; }
 tmp[0] = esi * -0x19; (void)tmp;                   // Ordinal_11087(tmp,..) x2 unrecovered
 vel(e, sp, edi << 11, I(0xc8000) * 0 + tmp[1] * 0x800, 0xc8000, ebp << 11, esi << 11, I(0xffff8000), true);
 e.sub617c0b(nullptr);                              // FUN_6fb77fb0(sp) guess: see effect630f0
 e.setMonsterAlloc(sp, 0);
}
// 1.13c D2Client+0xB44A0
void sub_B44A0(EnvB3720& e, Unit* u) {
 U v = e.level(u) & 0x80000003u;
 I m = I(v) < 0 ? I(((v - 1) | 0xfffffffcu) + 1) : I(v);
 if (m == 0) {
  Req r{}; r.w[0] = 1; r.w[1] = e.getRoom(0); ids(e, *u, r, 10, 12);   // field set guess
  r.w[5] = W(ux(e, *u)); r.w[6] = W(uy(e, *u)); rng(*u); rng(*u);
  e.spawn(&r);
 }
 e.freeUnit(u);
}
// 1.13c D2Client+0xB45D0  (ecx=unit, edx=p2)
U sub_B45D0(EnvB3720& e, Unit* u, I p2) {
 if (p2 != 0) return 0;
 U id = e.o11018(u);
 W t = e.findUnit(3, id); if (!t) return 1;
 Unit* tu = reinterpret_cast<Unit*>(t);
 if (tu->type != 3) { e.fatal(0x5a); return 0; }
 Req r{}; r.w[1] = e.getRoom(0); ids(e, *u, r, 10, 12);
 e.level(tu);
 r.w[5] = W(ux(e, *tu)); r.w[6] = W(uy(e, *tu));       // coords/type words guess
 W sp = e.spawn(&r); if (sp) e.o10637(sp, tu->id);
 return 1;
}
// 1.13c D2Client+0xB4760
void sub_B4760(EnvB3720& e, Unit* u) {
 I v = e.level(u); int i = 0;
 while (e.tbl4760[i].a >= v) { if (++i >= 4) break; }
 if (v % e.tbl4760[i].b == 0) {
  I x = ux(e, *u), y = uy(e, *u);
  Req r{}; r.w[0] = 1; r.w[1] = e.getRoom(0); r.w[4] = 0x15f;
  e.o10240(u); r.w[9] = e.o10240(u); r.w[12] = e.o11029(u);
  for (I k = 0; k < e.tbl4760[i].c; k++) {
   I a = I(rng(*u) % 10) - 5; if ((a < 0 ? -a : a) < 2) a = 5;
   I b = I(rng(*u) % 10) - 5; if ((b < 0 ? -b : b) < 2) b = 5;
   r.w[5] = W(x + a * 2); r.w[6] = W(y + b * 2);
   W sp = e.spawn(&r); if (sp) e.o10637(sp, u->id);
  }
 }
 if (v < 0x19) e.o10637(reinterpret_cast<W>(u), 3);
 else if (v <= 0x4a) e.o10637(reinterpret_cast<W>(u), 2);
 if (v < 2) {
  U id = e.o10818(u); W m = e.findUnit(2, id);
  if (m) {
   if (reinterpret_cast<Unit*>(m)->type != 2) { e.fatal(0x5a); return; }
   e.f1b300(m, 3, 0, 0, 0); e.setMonsterAlloc(m, 0);   // m[0x19]==0 -> m[0x19]=alloc(0x3c)
  }
 }
 e.freeUnit(u);
}
// 1.13c D2Client+0xB49F0  (ecx=unit, edx=p2, edi=y guess)
U sub_B49F0(EnvB3720& e, Unit* u, I p2, I edi) {
 if (p2 != 0) return 0;
 e.o10818(u); e.o10614(); e.o11018(u);
 I x = ux(e, *u), y = uy(e, *u);
 Req r{}; r.w[0] = 0x21; r.w[1] = e.getRoom(0); r.w[4] = 0x15d; r.w[5] = W(x); r.w[6] = W(y);
 ids(e, *u, r, 10, 12);
 for (I i = 0; i < y; i++) {
  r.w[5] = W(I(rng(*u) % 0x14) - 10 + x); r.w[6] = W(edi - 10 + I(rng(*u) % 0x14));
  if (I(r.w[5]) == x && I(r.w[6]) == edi) { i--; continue; }
  W sp = e.spawn(&r); if (sp) e.f23b60(sp);
 }
 return 1;
}
// 1.13c D2Client+0xB4B80
void sub_B4B80(EnvB3720& e, Unit* u) {
 U v = U(e.level(u)); W room = e.getRoom(0);
 if (v == 0x96) { e.g->tick = e.tick(); e.g->z0 = 0; e.g->four000 = 4000; e.g->z1 = 0; e.g->v19 = 0x19; }
 I thr = 0x145;
 for (int i = 0; i < 7; i++, thr -= 0x19) {
  if (thr < I(v)) break;
  if (v == U(thr)) {
   I y = uy(e, *u), x = ux(e, *u);
   if (room) {
    Req r{}; r.w[0] = 1; r.w[1] = room; r.w[4] = 0x154;
    r.w[5] = W(e.tbl4b80[i].a + x); r.w[6] = W(e.tbl4b80[i].b + y);
    r.w[11] = e.o10240(u); r.w[12] = e.o11029(u);
    W sp = e.spawn(&r);
    if (sp) { e.o10706(sp, e.tbl4b80[i].c); e.o10558(sp, 0x19); e.o10970(sp, 0x19); }   // last two args guess
   }
  }
 }
 if (I(v) < 0xa5 && (v & 1) == 0) {
  Req r{}; r.w[0] = 0x21; r.w[1] = room; r.w[5] = W(ux(e, *u)); r.w[6] = W(uy(e, *u));
  r.w[7] = W(ux(e, *u) - 0xd); r.w[8] = W(uy(e, *u) + 3);
  ids(e, *u, r, 10, 12); r.w[4] = 0x153;
  W sp = e.spawn(&r);
  if (sp) { vel(e, sp, 0, 0, 0x23000, 0, 0, I(0xfffff800), true); e.o10706(sp, 0x15); e.o10520(sp, 0); }
 }
 if (I(v) < 0x96) {
  I x = ux(e, *u), y = uy(e, *u);
  Req r{}; r.w[0] = 0x21;
  r.w[7] = W(I(rng(*u) % 5) + 1 + x - 0xd); r.w[8] = W(I(rng(*u) % 10) - 5 + y + 3);
  r.w[1] = room; r.w[5] = W(x - 0xd); r.w[6] = W(y + 3);
  ids(e, *u, r, 10, 12); r.w[4] = 0x96;
  W sp = e.spawn(&r); if (sp) e.o10441(sp, 0x40);
 }
 e.freeUnit(u);
}
} // namespace d2client_b3720

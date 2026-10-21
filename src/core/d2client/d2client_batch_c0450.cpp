// D2Client 0xC0450..0xC2F40 batch: env-seam implementations from the decompile.
// Globals are retail VAs (image base 0x6fb50000) read/written through Env; unrecovered callees and imported
// ordinals are Env function pointers (names positional: fNNNN = D2Client RVA, oNNNNN = ordinal).
#include <cstdint>
namespace d2client_batch_c0450 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t); uint8_t (*rd8)(P); void (*wr8)(P, uint8_t);
 uint16_t (*rd16)(P); void (*wr16)(P, uint16_t);
 P dataTables;                       // *sgptDataTables
 P bitMasks10;                       // gdwBitMasks[0x10]
 P str_loadScreenGlobal, str_loadScreenLocal, str_fmtGlobal, str_fmtLocal; // sprintf format/dir strings
 P buf_c980; P str_c8359f; P tbl_fb80580, tbl_fb80864, tbl_fba51bc; // misc data addresses
 P (*f4d850)(P); P (*f4d630)(P, int32_t); bool (*f4dc60)(int32_t, int32_t, int32_t);
 void (*win2Unicode)(P, P, int32_t); void (*personalize)(P, P, P, int32_t, int32_t); void (*ustrcpy)(P, P);
 uint32_t (*o10009)(); P (*o10003)(); void (*o10070)(int32_t, int32_t, int32_t, int32_t);
 int32_t (*o10651)(); int32_t (*o11142)(P); uint32_t (*o11080)(); uint32_t (*o11168)(P);
 int32_t (*o10012)(uint32_t, uint32_t); void (*o10023)(uint32_t, uint32_t, int32_t); int32_t (*o10013)();
 uint32_t (*o10265)(uint32_t); void (*o10024)(P, uint32_t); void (*exit_)(int32_t);
 P (*f55d90)(); int32_t (*f70590)(); void (*f54da0)(P);
 void (*f6e860)(); void (*f0f090)(); void (*f31750)(); void (*f6e020)();
 uint32_t (*getTick)(); void (*o10065)(uint32_t); int32_t (*o10043)(int32_t, int32_t);
 P (*o10331)(P, uint32_t); int32_t (*o10031)(); int32_t (*o10494)(P, int32_t); int32_t (*o10871)(P, int32_t);
 P (*f12b0)(); int32_t (*o10973)(P, int32_t, int32_t); void (*f1b190)(P, int32_t, int32_t, int32_t);
 void (*f4b00)(); int32_t (*o10085)(int32_t, int32_t, int32_t);
 void (*f71580)(); uint8_t (*o10190)(int32_t, int32_t, int32_t); void (*o10014)(int32_t, int32_t, int32_t, int32_t, uint32_t, int32_t);
 int32_t (*o10086)(uint32_t); void (*o10041)(const int32_t*, int32_t, int32_t, uint32_t, int32_t, int32_t); void (*o10040)();
 P (*fdb420)(int32_t);
 uint32_t (*ustrlen)(P); P (*f1a80)(); int32_t (*o10083)(); void (*f6f370)(int32_t, int32_t, uint32_t);
 void (*o10227)(); void (*f72790)(int32_t, int32_t, int32_t); void (*faf7b20)(); int32_t (*f55440)();
 void (*feccc0)(int32_t, P); void (*fecc60)(int32_t, P); int32_t (*o10048)(); int32_t (*o423)(const char*, const char*, int32_t, P);
 void (*o10066)();
 int32_t (*o10043z)(); void (*o10021z)(int32_t); void (*o10062z)(); // register/stack-arg forms (args not recovered)
 void (*fd6cd0)(); void (*fed470)(); void (*f27d30)(); void (*f2cb80)(); void (*f12f60)(); void (*fe5920)(); void (*ffffc0)();
 void (*f0afa0)(); void (*ff6640)(); void (*f11440)(); void (*f5a380)(); void (*f5a190)(); void (*f09740)(int32_t);
 void (*f6f720)(); void (*f3bf10)(); void (*f4fa90)(); void (*fccad0)(); void (*f3b840)(); void (*f53ad0)(); void (*f13d90)();
 void (*o10897)(int32_t, uint32_t);
};
// ---- D2Client+0xC0450 ----
// 1.13c D2Client+0xC0450  (this=param, out=in_EAX; edx result taken as this->0xc id)
void sub_C0450(const Env& e, P self, P out, P p1, P p2, int32_t p3) {
 P pc = e.f4d850(self);
 int32_t id;
 if (!self) id = -1;
 else {
  if ((int32_t)e.rd32(self + 0x10) == 0x11) {
   e.wr32(out, 0);
   uint16_t local[512] = {0};
   e.win2Unicode((P)local, pc, 0x80);
   e.personalize(p2, (P)local, e.o10003(), p3, (int32_t)e.o10009());
   e.wr32(p1, 1);
   return;
  }
  id = (int32_t)e.rd32(self + 0xc);
 }
 P q = e.f4d630(self, id);
 uint32_t pl = e.rd32(0x6fbcbbfc);
 int32_t plId = pl ? (int32_t)e.rd32(pl + 0xc) : -1;
 int32_t v = (id == plId || !q) ? 0 : (int32_t)e.rd32(q + 0x14);
 v = (v * 128) / 100;
 e.wr32(out, (uint32_t)v);
 e.wr32(p1, 0);
 int32_t a = self ? (int32_t)e.rd32(self + 0xc) : -1;
 if (e.f4dc60(v, plId, a)) e.wr32(p1, 1);
 e.win2Unicode(p2, pc, 0x80);
}
// ---- D2Client+0xC07C0 ----
// 1.13c D2Client+0xC07C0
void sub_C07C0(const Env& e, int32_t p) {
 if (e.rd8(0x6fbc9721)) return;
 int32_t r = e.f70590();
 uint32_t x = e.rd32(0x6fb8bc54); int32_t y = (int32_t)e.rd32(0x6fb8bc58) - 0x48;
 if (!p) { e.f55d90(); e.o10070(y, 0, 1, r); return; }
 P s = e.f55d90();
 e.wr16(0x6fbcc980, 0);
 if (s) { e.ustrcpy(0x6fbcc980, s); e.wr32(0x6fbcca24, (uint32_t)y); e.wr32(0x6fbcca28, (uint32_t)r); e.wr32(0x6fbcca20, x); }
}
// 1.13c D2Client+0xC0CE0
void sub_C0CE0(const Env& e) { if (e.rd32(0x6fbcc394)) e.f54da0(0x6fbba608); }
// 1.13c D2Client+0xC1340
void sub_C1340(const Env& e) {
 uint32_t pl = e.rd32(0x6fbcbbfc);
 if (e.rd32(0x6fbcc414) == 3 || !pl) return;
 uint32_t n = e.rd32(0x6fbcc4d4);
 if (!n) return;
 while (e.rd32(n + 4) != 7 || e.rd32(n + 0xc) != e.rd32(pl + 0xc) || e.rd32(n + 0x20) != 0) {
  n = e.rd32(n + 0x30); if (!n) return;
 }
 uint32_t id = e.rd32(n + 8);
 if (id == 0xffffffffu) return;
 uint32_t m = e.rd32(0x6fbcc4d4); uint32_t rec = 0;
 for (;;) {
  if (e.rd32(m + 8) == id) { rec = m + 0x24; break; }
  m = e.rd32(m + 0x30); if (!m) break;
 }
 if (e.rd32(rec + 8) != 1) return;
 P u = e.rd32(0x6fbba808 + (id & 0x7f) * 4);
 if (!u) return;
 while (e.rd32(u + 0xc) != id) { u = e.rd32(u + 0xe4); if (!u) return; }
 if (e.rd32(u) != 1) { uint32_t c = e.o10265(0); e.o10024(e.str_c8359f, c); e.exit_(-1); return; }
 if (e.o10013() == 0) {
  int32_t a = (int32_t)e.rd32(0x6fbc9960) - (int32_t)e.rd32(0x6fbcc418);
  e.wr32(0x6fb8bc54, (uint32_t)(e.o10651() - a));
  int32_t b = (int32_t)e.rd32(0x6fbc995c) - 8;
  e.wr32(0x6fb8bc58, (uint32_t)(e.o11142(u) - b));
  sub_C07C0(e, 1); return;
 }
 uint32_t u3 = e.o11080(), u4 = e.o11168(u);
 if (e.o10012(u3, u4) == 0) return;
 e.o10023(u3, u4, 0);
 // ponytail: decompile's stack-pointer arithmetic (ppiStack_2c/auStack_4) is a Ghidra stack artifact; base is the
 // pre-adjust local address, taken as 0 here -- vector must pin it before this row VERIFIES.
 int32_t base = 0, w = (int32_t)e.rd32(0x6fb8bc48), adj = 0;
 uint32_t mode = e.rd32(0x6fbcc414);
 if (mode == 1) adj = -((w + ((w >> 31) & 3)) >> 2);
 else if (mode == 2) adj = (w + ((w >> 31) & 3)) >> 2;
 e.wr32(0x6fb8bc58, 0);
 e.wr32(0x6fb8bc54, (uint32_t)(base + adj));
 sub_C07C0(e, 1);
}
// 1.13c D2Client+0xC14F0
void sub_C14F0(const Env& e) {
 uint32_t h = e.rd32(0x6fbcb9d0);
 if (h) {
  e.o10065(h);
  e.o10043(0x7a, 0);
  e.wr32(0x6fbcb9d0, 0);
  e.o10065(0);
  e.o10024(e.rd32(0x6fbcb9d4), 0);
  e.f0f090(); e.f6e860(); e.f31750();
  uint32_t t = e.getTick();
  if (e.rd32(0x6fbc981c) < t + 2000) e.wr32(0x6fbc981c, t + 2000);
 }
 e.wr32(0x6fbcb9cc, 0); e.wr32(0x6fbcb9d0, 0); e.wr32(0x6fbcb9b0, 0);
}
// 1.13c D2Client+0xC1580  (the sprintf'd path is dead in the decompile: fdb420(0) takes only 0)
void sub_C1580(const Env& e) {
 sub_C14F0(e);
 e.f6e020();
 e.o10009(); // selects "DATA\\LOCAL" vs "DATA\\GLOBAL" format; result unused downstream
 e.wr32(0x6fbcb9d0, (uint32_t)e.fdb420(0));
 e.wr32(0x6fbcbbb4, 0); e.wr32(0x6fbcb9b0, 1); e.wr32(0x6fbcc4f8, 0);
 e.wr32(0x6fbaad78, e.rd32(0x6fbcc3a8));
}
// 1.13c D2Client+0xC1610
void sub_C1610(const Env& e, P ecx) {
 uint32_t pl = e.rd32(0x6fbcbbfc);
 if (!e.rd32(0x6fbcc3b8) || !pl || !e.o10331(pl, ecx) || !e.rd32(0x6fbcb9e4)) return;
 int32_t y = (int32_t)e.rd32(0x6fb8bc4c) - (int32_t)e.rd32(0x6fba9e74);
 int32_t x = (int32_t)e.rd32(0x6fb8bc48) - (int32_t)e.rd32(0x6fba9e78);
 if (e.o10031() == 2) y = (int32_t)e.rd32(0x6fb8bc4c) - (int32_t)e.rd32(0x6fba9e70);
 P dt = e.rd32(e.dataTables);
 for (int32_t i = 0; i < (int16_t)e.rd16(dt + 0x170); i++) {
  int32_t s = (int16_t)e.rd16(e.rd32(dt + 0x16c) + i * 2);
  if (e.o10494(pl, s) && e.o10871(pl, s)) {
   P r = e.f12b0();
   if (r && (e.rd8(e.bitMasks10) & e.rd8(r + 0x10)) && (int16_t)e.rd16(r + 0xe) > 0 &&
       (int16_t)e.rd16(r + 0xe) < (int32_t)e.rd32(e.rd32(e.dataTables) + 0xbc0)) {
    int16_t k = (int16_t)e.rd16(r + 0x18);
    if (k >= 0 && k <= (int32_t)e.rd32(e.rd32(e.dataTables) + 0xbd4)) {
     int32_t lv = e.o10973(pl, k, 0);
     if (lv > 0 && lv < 4) { e.f1b190(pl, (int16_t)e.rd16(r + 0xe) - 1 + lv, x, y); x -= (int32_t)e.rd32(0x6fbd3408); }
    }
   }
  }
  dt = e.rd32(e.dataTables);
 }
}
// 1.13c D2Client+0xC1760
void sub_C1760(const Env& e, int32_t p) {
 if (e.o10043z() == 0) e.o10021z(0);
 uint32_t mode = e.rd32(0x6fbcc394);
 uint32_t t = e.getTick();
 if (e.rd32(0x6fbc97fc) < t) { e.f4b00(); e.wr32(0x6fbc97fc, t + 5000); e.wr32(0x6fbc9800, t); }
 if (e.o10043z() == 0) {
  e.o10062z();
  if (e.o10085(1, 0, 0)) {
   if (!e.rd32(0x6fbcb9d0)) {
    sub_C1580(e);
    if (!e.rd32(0x6fbcb9d0)) { uint32_t c = e.o10265(0x19c6); e.o10024(e.str_c8359f, c); e.exit_(-1); return; }
   }
   int32_t W = (int32_t)e.rd32(0x6fb8bc48), H = (int32_t)e.rd32(0x6fb8bc4c);
   if (mode == 6 || mode == 8 || mode == 7 || mode == 9) {
    if (!e.rd8(0x6fbcbbb8)) { e.wr8(0x6fbcbbb8, e.o10190(0x60, 0, 0)); e.wr8(0x6fbcbbb9, e.o10190(0xff, 0xff, 0)); }
    if (!e.rd32(0x6fbaad78)) {
     e.o10014(W / 2 - 0x82, H / 2 + 0x9e, W / 2 + 0x82, H / 2 + 0xb2, e.rd8(0x6fbcbbb9), 1);
     e.o10014(W / 2 - 0x80, H / 2 + 0xa0, W / 2 + 0x80, H / 2 + 0xb0, 0, 5);
     uint32_t f8 = e.rd32(0x6fbcc4f8);
     if (f8) e.o10014(W / 2 - 0x80, H / 2 + 0xa0, f8 + (W / 2 - 0x80), H / 2 + 0xb0, e.rd8(0x6fbcbbb8), 5);
    }
   }
   int32_t n = e.o10086(e.rd32(0x6fbcb9d0));
   if (n <= (int32_t)e.rd32(0x6fbcbbb4)) e.wr32(0x6fbcbbb4, (uint32_t)(n - 1));
   int32_t fr = (int32_t)e.rd32(0x6fbcbbb4);
   e.o10041(&fr, W / 2 - 0x80, H / 2 + 0x80, 0xffffffffu, 5, 0);
   e.o10040();
   e.wr32(0x6fbcc3dc, 0); e.wr32(0x6fbcc3e0, 0); e.wr32(0x6fbcc3f8, 0); e.wr32(0x6fbcc3fc, 0);
  }
 }
 if (p) e.wr32(0x6fbcbbb4, e.rd32(0x6fbcbbb4) + 1);
}
// 1.13c D2Client+0xC19B0
void sub_C19B0(const Env& e) {
 if (e.ustrlen(0x6fbcc780) == 0) return;
 P u = e.f1a80();
 if (!u || e.rd32(u) != 1) return;
 uint32_t c = 0; P x = e.rd32(u + 0x14);
 if (x && (e.rd8(x + 0x16) & 8)) c = 4;
 if (x && (e.rd8(x + 0x16) & 4)) c = 3;
 e.f6f370(6, e.o10083() + 0x27, c);
}
// 1.13c D2Client+0xC1A20
void sub_C1A20(const Env& e) {
 P u = e.f1a80();
 if (!u || e.rd32(u) != 1) return;
 int32_t y = e.o10083() + 0x27;
 e.f6f370(6, y, e.rd32(0x6fbccc30));
 if (e.rd32(0x6fbcca30) || e.rd32(0x6fbcc780)) y += e.o10083();
 e.f6f370(6, y, e.rd32(0x6fbcc778));
}
static void fatalExit(const Env& e) { uint32_t c = e.o10265(0x139b); e.o10024(e.str_c8359f, c); e.exit_(-1); }
static void freeHandle(const Env& e, P a) {
 uint32_t h = e.rd32(a);
 if (h) { e.o10065(h); e.o10043(0x7a, 0); e.wr32(a, 0); }
}
// 1.13c D2Client+0xC2D10
void sub_C2D10(const Env& e) {
 uint32_t was = e.rd32(0x6fbaad80);
 e.o10227();
 if ((int32_t)e.rd32(0x6fb8d5d0) != 0x26) { fatalExit(e); return; }
 e.wr32(0x6fbaad80, 0);
 if (was) e.fecc60(e.o10048(), e.tbl_fb80580);
 e.fd6cd0();
 if (e.rd32(0x6fbcbbfc)) { e.fed470(); e.f27d30(); }
 e.o10066(); e.f2cb80(); e.f12f60(); e.fe5920(); e.ffffc0(); e.f0afa0(); e.ff6640(); e.f11440(); e.f5a380();
 uint32_t k = e.rd8(0x6fbacdd1);
 if (k) e.o10897(0, k);
 e.wr8(0x6fbacdd1, 0);
 e.f5a190(); e.f09740(0); e.f6f720(); e.f3bf10(); e.f4fa90(); e.fccad0(); e.f3b840(); e.f53ad0();
 freeHandle(e, 0x6fbc54ec); freeHandle(e, 0x6fbcb9c0); freeHandle(e, 0x6fbcb9c8);
 freeHandle(e, 0x6fbaad58); freeHandle(e, 0x6fbcb9c4); freeHandle(e, 0x6fbaad54);
 e.wr32(0x6fbcc434, 0);
 if (e.rd32(0x6fbd349c) == 1) { e.wr32(0x6fbd349c, 0); e.fecc60(e.o10048(), e.tbl_fba51bc); }
 e.f13d90();
 freeHandle(e, 0x6fbcc544);
}
// 1.13c D2Client+0xC2F40  (switch table PTR_DAT_6fb8d538 at 0x6fb8d538, stride 4; panel +0x54 = kind)
void sub_C2F40(const Env& e) {
 uint8_t sc[4];
 if (e.o423("Diablo II", "Mini Panel", 1, (P)sc)) return;
 uint32_t old = e.rd32(0x6fbaadd4);
 e.o10227();
 int32_t cnt = (int32_t)e.rd32(0x6fb8d5d0);
 if (cnt != 0x26) { fatalExit(e); return; }
 if (old == 0) {
  if (e.rd32(0x6fbc3a68)) return;
  for (int32_t i = 0; i < cnt; i++) {
   if (e.rd32(0x6fbaad80 + i * 4) != 1) continue;
   uint32_t k = e.rd32(e.rd32(0x6fb8d538 + i * 4) + 0x54);
   if (k == 1) e.f72790(i, 1, 0);
   else if (k == 4) { if (e.rd8(0x6fbc9721)) e.faf7b20(); }
   else if (k != 0) return;
  }
 }
 uint32_t cur = old; uint32_t pl = e.rd32(0x6fbcbbfc);
 if (!pl || (e.rd32(pl) == 0 && e.f55440() == 0)) { cur = (cur == 0); e.wr32(0x6fbaadd4, cur); }
 if (old == 0) { if (cur) e.feccc0(e.o10048(), e.tbl_fb80864); }
 else if (!cur) e.fecc60(e.o10048(), e.tbl_fb80864);
}
}

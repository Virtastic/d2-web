// D2Client 0xCB120..0xCB624 batch: MSVC7.1 CRT/EH helpers as plain C++ (SEH/register-ABI approximated).
// Unrecovered callees are Env seams; retail memory via rd/wr seams.
#include <cstdint>
namespace d2client_batch_cb120 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t); uint8_t (*rd8)(P); void (*wr8)(P, uint8_t);
 P ptd;                         // __getptd() result; _curexception at +0x?? (ptdCurExc)
 uint32_t curExcOff;
 void (*inconsistency)();       // _inconsistency
 uint32_t cookie;               // DAT_6fb8b780
 P (*callSettingFrame)(P, P, uint32_t);  // __CallSettingFrame_12
 uint32_t exceptionList;        // fs:[0] shadow
 uint8_t (*tolower_)(uint8_t); int (*iscntrl_)(uint8_t);
 uint8_t decimalPoint;          // DAT_6fb8b914
 void (*cvtDouble)(P out, P str); void (*cvtInt)(P out, P str);   // FUN_6fb7c2d0 / c315
 void (*shift)(P s, int32_t d); // __shift
 void (*fptostr)(P buf, int32_t ndig, P strflt);                  // __fptostr
 void (*fltout)(uint32_t lo, uint32_t hi, P strflt, P buf);       // FUN_6fb7c48b
 void (*memset_)(P, uint8_t, uint32_t);
 double fdivA, fdivB, fdivC;    // _DAT_6fb875c8/d0/d8
 P k32; P (*getModuleHandleA)(const char*); P (*getProcAddress)(P, const char*);
 uint32_t (*callProcFeature)(P, uint32_t);
};
static void strcpyAt(const Env& e, P d, const char* s) { do e.wr8(d++, (uint8_t)*s); while (*s++); }
static P strend(const Env& e, P p) { while (e.rd8(p)) ++p; return p; }
// 1.13c D2Client+0xCB120
void FindAndUnlinkFrame(const Env& e, P f) {
 P head = e.ptd + e.curExcOff;
 if (f == e.rd32(head)) { e.wr32(head, e.rd32(f + 4)); return; }
 P n = e.rd32(head);
 for (;;) {
  if (e.rd32(n + 4) == 0) { e.inconsistency(); return; }
  P nx = e.rd32(n + 4);
  if (nx == f) break;
  n = nx;
 }
 e.wr32(n + 4, e.rd32(f + 4));
}
// 1.13c D2Client+0xCB16C
P CallCatchBlock2(const Env& e, P reg, P funcInfo, P handler, int32_t state, uint32_t flags) {
 (void)funcInfo; (void)state; // ponytail: SEH frame push/pop not expressible; handler invoked directly
 return e.callSettingFrame(handler, reg, flags);
}
// 1.13c D2Client+0xCB1C5  (strip to 'e', insert decimal point there, shift tail right)
void sub_CB1C5(const Env& e, P s) {
 bool stop = e.tolower_(e.rd8(s)) == 'e';
 while (!stop) { ++s; stop = e.iscntrl_(e.rd8(s)) == 0; }
 uint8_t c = e.rd8(s); e.wr8(s, e.decimalPoint);
 uint8_t n;
 do { ++s; n = e.rd8(s); e.wr8(s, c); c = n; } while (e.rd8(s));
}
// 1.13c D2Client+0xCB266
void fassign(const Env& e, int32_t flag, P arg, P num) {
 uint32_t tmp[2] = {0, 0};
 if (flag) { e.cvtDouble((P)tmp, num); e.wr32(arg, tmp[0]); e.wr32(arg + 4, tmp[1]); return; }
 e.cvtInt((P)tmp, num); e.wr32(arg, tmp[0]);
}
// 1.13c D2Client+0xCB2C1  (strflt = in_EAX, buf = in_EBX register args)
void cftoe2(const Env& e, P strflt, P buf, int32_t ndec, int32_t caps, char g) {
 if (g) e.shift(buf + (e.rd32(strflt) == 0x2d), 1);
 if (e.rd32(strflt) == 0x2d) e.wr8(buf++, 0x2d);
 P p = buf;
 if (ndec > 0) { p = buf + 1; e.wr8(buf, e.rd8(p)); e.wr8(p, e.decimalPoint); }
 P q = p + ndec + (g == 0);
 strcpyAt(e, q, "e+000");
 if (caps) e.wr8(q, 0x45);
 if (e.rd8(e.rd32(strflt + 12)) != '0') {
  int32_t x = (int32_t)e.rd32(strflt + 4) - 1;
  if (x < 0) { x = -x; e.wr8(q + 1, 0x2d); }
  if (x > 99) { e.wr8(q + 2, e.rd8(q + 2) + x / 100); x %= 100; }
  if (x > 9) { e.wr8(q + 3, e.rd8(q + 3) + x / 10); x %= 10; }
  e.wr8(q + 4, e.rd8(q + 4) + x);
 }
}
// 1.13c D2Client+0xCB36F
P cftoe(const Env& e, P dbl, P buf, int32_t ndec, int32_t caps) {
 uint32_t sf[4], scratch[6];
 e.fltout(e.rd32(dbl), e.rd32(dbl + 4), (P)sf, (P)scratch);
 e.fptostr(buf + (ndec > 0) + (sf[0] == 0x2d), ndec + 1, (P)sf);
 cftoe2(e, (P)sf, buf, ndec, caps, 0);
 return buf;
}
// 1.13c D2Client+0xCB3DF
P cftof2(const Env& e, P strflt, P buf, uint32_t ndec, char g) {
 int32_t dp = (int32_t)e.rd32(strflt + 4);
 bool neg = e.rd32(strflt) == 0x2d;
 if (g && (uint32_t)(dp - 1) == ndec) { e.wr8(buf + neg + (dp - 1), '0'); e.wr8(buf + neg + (dp - 1) + 1, 0); }
 P p = buf;
 if (neg) { e.wr8(p++, 0x2d); }
 if (dp < 1) { e.shift(p, 1); e.wr8(p++, '0'); } else p += dp;
 if ((int32_t)ndec > 0) {
  e.shift(p, 1); e.wr8(p, e.decimalPoint);
  if (dp < 0) {
   uint32_t n = (uint32_t)-dp;
   if (g || (int32_t)n <= (int32_t)ndec) ndec = n;
   e.shift(p + 1, (int32_t)ndec); e.memset_(p + 1, '0', ndec);
  }
 }
 return buf;
}
// 1.13c D2Client+0xCB47B
P cftof(const Env& e, P dbl, P buf, uint32_t ndec) {
 uint32_t sf[4], scratch[6];
 e.fltout(e.rd32(dbl), e.rd32(dbl + 4), (P)sf, (P)scratch);
 e.fptostr(buf + (sf[0] == 0x2d), (int32_t)(sf[1] + ndec), (P)sf);
 cftof2(e, (P)sf, buf, ndec, 0);
 return buf;
}
// 1.13c D2Client+0xCB4E2
void cftog(const Env& e, P dbl, P buf, uint32_t ndec, int32_t caps) {
 uint32_t sf[4], scratch[6];
 e.fltout(e.rd32(dbl), e.rd32(dbl + 4), (P)sf, (P)scratch);
 int32_t before = (int32_t)sf[1] - 1;
 P d = buf + (sf[0] == 0x2d);
 e.fptostr(d, (int32_t)ndec, (P)sf);
 int32_t x = (int32_t)sf[1] - 1; sf[1] = (uint32_t)x;
 if (x < -4 || (int32_t)ndec <= x) { cftoe2(e, (P)sf, buf, (int32_t)ndec, caps, 1); return; }
 if (before < x) { P q = strend(e, d); e.wr8(q - 1, 0); }
 cftof2(e, (P)sf, buf, ndec, 1);
}
// 1.13c D2Client+0xCB581
P cfltcvt(const Env& e, P dbl, P buf, int32_t fmt, int32_t ndec, int32_t caps) {
 if (fmt == 0x65 || fmt == 0x45) return cftoe(e, dbl, buf, ndec, caps);
 if (fmt == 0x66) return cftof(e, dbl, buf, (uint32_t)ndec);
 cftog(e, dbl, buf, (uint32_t)ndec, caps);
 return 0;
}
// 1.13c D2Client+0xCB5E4
uint32_t ms_p5_test_fdiv(const Env& e) {
 volatile double a = e.fdivA, b = e.fdivB, c = e.fdivC;
 return a < b - (b / c) * c;
}
// 1.13c D2Client+0xCB624
void ms_p5_mp_test_fdiv(const Env& e) {
 P h = e.getModuleHandleA("KERNEL32");
 if (h) { P f = e.getProcAddress(h, "IsProcessorFeaturePresent"); if (f) { e.callProcFeature(f, 0); return; } }
 ms_p5_test_fdiv(e);
}
} // namespace d2client_batch_cb120

// D2Client 0xCC48B..0xCC7C0 batch: MSVC CRT helpers as plain C++; Win32/unrecovered callees are Env seams.
#include <cstdint>
namespace d2client_batch_cc48b {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 void (*dtold)(uint32_t* out12, const void* dbl);               // ___dtold
 uint32_t (*cd43)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, P); // D2Client+0xCCD43
 void (*f7bc0)(P dst, P src);                                    // D2Client+0x7BC0 (string copy)
 uint32_t (*getHwCw)(); void (*setHwCw)(uint32_t);               // fnstcw / fldcw
 P filter;                                                       // DAT_6fbd3714
 int (*isBadRead)(P, uint32_t); int (*isBadWrite)(P, uint32_t); int (*isBadCode)(P);
 void (*terminate)(); void (*abortMsg)(uint32_t); void (*raise)(int); void (*exit)(int);
 int32_t (*callFilter)(P fn, P arg);
};
// 1.13c D2Client+0xCC48B
P sub_CC48B(const Env& e, uint32_t lo, uint32_t hi, P out, P str) {
 uint32_t dbl[2] = {lo, hi}, x[3]; uint8_t buf[0x20] = {0};
 e.dtold(x, dbl);
 uint32_t n = e.cd43(x[0], x[1], x[2] & 0xffff, 0x11, 0, (P)buf);
 e.wr32(out + 8, n);
 e.wr32(out, (uint32_t)(int32_t)(int8_t)buf[2]);
 e.wr32(out + 4, (uint32_t)(int32_t)(int16_t)(buf[0] | buf[1] << 8));
 e.f7bc0(str, (P)(buf + 4));
 e.wr32(out + 12, (uint32_t)str);
 return out;
}
// 1.13c D2Client+0xCC503 (bx = x87 control word)
uint32_t sub_CC503(uint32_t bx) {
 uint32_t r = 0; bx &= 0xffff;
 if (bx & 1) r = 0x10;
 if (bx & 4) r |= 8;
 if (bx & 8) r |= 4;
 if (bx & 0x10) r |= 2;
 if (bx & 0x20) r |= 1;
 if (bx & 2) r |= 0x80000;
 uint32_t rc = bx & 0xc00;
 if (rc == 0x400) r |= 0x100; else if (rc == 0x800) r |= 0x200; else if (rc == 0xc00) r |= 0x300;
 if ((bx & 0x300) == 0) r |= 0x20000; else if ((bx & 0x300) == 0x200) r |= 0x10000;
 if (bx & 0x1000) r |= 0x40000;
 return r;
}
// 1.13c D2Client+0xCC595 (ebx = abstract control word)
uint32_t sub_CC595(uint32_t b) {
 uint32_t r = (b & 0x10) != 0;
 if (b & 8) r |= 4;
 if (b & 4) r |= 8;
 if (b & 2) r |= 0x10;
 if (b & 1) r |= 0x20;
 if (b & 0x80000) r |= 2;
 uint32_t rc = b & 0x300;
 if (rc == 0x100) r |= 0x400; else if (rc == 0x200) r |= 0x800; else if (rc == 0x300) r |= 0xc00;
 if ((b & 0x30000) == 0) r |= 0x300; else if ((b & 0x30000) == 0x10000) r |= 0x200;
 if (b & 0x40000) r |= 0x1000;
 return r;
}
// 1.13c D2Client+0xCC623
uint32_t sub_CC623(const Env& e, uint32_t nv, uint32_t mask) {
 uint32_t old = sub_CC503(e.getHwCw());
 uint32_t nw = (old & ~mask) | (nv & mask);
 e.setHwCw(sub_CC595(nw)); // approximate: retail's __hw_cw result is consumed by fldcw
 return nw;
}
// 1.13c D2Client+0xCC655
uint32_t sub_CC655(const Env& e, uint32_t nv, uint32_t mask) { return sub_CC623(e, nv, mask & 0xfff7ffff); }
// 1.13c D2Client+0xCC66B (exception pointers: [0] -> record; rec[0]=code, rec[4]=nparams, rec[5]=magic)
int32_t sub_CC66B(const Env& e, P ep) {
 P r = e.rd32(ep);
 if (e.rd32(r) == 0xE06D7363u && e.rd32(r + 0x10) == 3 && (e.rd32(r + 0x14) == 0x19930520 || e.rd32(r + 0x14) == 0x19930521)) e.terminate();
 if (e.filter && !e.isBadCode(e.filter)) return e.callFilter(e.filter, ep);
 return 0;
}
// 1.13c D2Client+0xCC6D9
int sub_CC6D9(const Env& e, P p, uint32_t n) { return !e.isBadRead(p, n); }
// 1.13c D2Client+0xCC6F5
int sub_CC6F5(const Env& e, P p, uint32_t n) { return !e.isBadWrite(p, n); }
// 1.13c D2Client+0xCC711
int sub_CC711(const Env& e, P fn) { return !e.isBadCode(fn); }
// 1.13c D2Client+0xCC729 (no [[noreturn]]: seam members return in tests)
void sub_CC729(const Env& e) { e.abortMsg(10); e.raise(0x16); e.exit(3); }
static int addl(const Env& e, uint32_t a, uint32_t b, P res) { uint64_t s = (uint64_t)a + b; e.wr32(res, (uint32_t)s); return (int)(s >> 32); }
// 1.13c D2Client+0xCC762
void sub_CC762(const Env& e, P a, P b) {
 if (addl(e, e.rd32(a), e.rd32(b), a)) {
  if (addl(e, e.rd32(a + 4), 1, a + 4)) e.wr32(a + 8, e.rd32(a + 8) + 1);
 }
 if (addl(e, e.rd32(a + 4), e.rd32(b + 4), a + 4)) e.wr32(a + 8, e.rd32(a + 8) + 1);
 addl(e, e.rd32(a + 8), e.rd32(b + 8), a + 8);
}
// 1.13c D2Client+0xCC7C0
void sub_CC7C0(const Env& e, P p) {
 uint32_t a = e.rd32(p), b = e.rd32(p + 4), c = e.rd32(p + 8);
 e.wr32(p, a * 2); e.wr32(p + 4, b * 2 | a >> 31); e.wr32(p + 8, c << 1 | b >> 31);
}
}

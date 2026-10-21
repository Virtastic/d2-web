// D2Client 0xCA588..0xCB0D7 batch: MSVC CRT (FPU adjust, __alldiv, C++ EH helpers) as plain C++.
// Unrecovered callees / CRT globals are Env seams; retail memory via rd32/wr32.
#include <cstdint>
namespace d2client_batch_ca588 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*wr32)(P, uint32_t);
 P fptrap;                     // PTR___fptrap_6fb8b8e8 .. +0x14 (6 slots)
 P cfltcvt, lab_b201, fassign, f_b1c5, lab_b24c;   // slot values
 uint32_t (*ms_p5_mp_test_fdiv)();   // CB5D2
 void (*setdefaultprecision)();      // CB624
 P datFdiv;                    // DAT_6fbd3710
 P datB780;                    // DAT_6fb8b780
 P excList;                    // ExceptionList (fs:[0]) cell
 void (*rtlUnwind)(P, P, P, P);
 P unwindRet;                  // 0x6fb7ae4a
 P translatorGuardHandler;
 uint32_t (*internalCxxFrameHandler)(P, P, P, P, P, int32_t, P, uint32_t);   // CBE55
 P (*getptd)();                // 72F7; _tiddata
 void (*callNLG)(P ptd, uint32_t code, P args);   // ptd->_NLG_dwCode
 void (*inconsistency)();      // CBF2C
 void (*jump)(P);              // indirect continuation call
 P (*getLocal8)();             // &local_8 slot for TranslatorGuardHandler
};
constexpr uint32_t kPtdCurException = 0x1c; // ponytail: _ptiddata::_curexception offset (x86 CRT)
// 1.13c D2Client+0xCA588
uint32_t adj_fdiv_m32i(uint32_t eax) { return eax; } // both branches: fdivp st(i),st
// 1.13c D2Client+0xCA5BC
uint32_t adj_fdivr_m32(uint32_t eax, uint32_t p1) { (void)p1; return eax; } // exp==0xff skips the fdivrp
// 1.13c D2Client+0xCAC6C
void fptrap_init(const Env& e) {
 const P v[6] = {e.cfltcvt, e.lab_b201, e.fassign, e.f_b1c5, e.lab_b24c, e.cfltcvt};
 for (int i = 0; i < 6; i++) e.wr32(e.fptrap + i * 4, (uint32_t)v[i]);
}
// 1.13c D2Client+0xCACA4
void fpmath(const Env& e, int32_t p1) {
 fptrap_init(e);
 e.wr32(e.datFdiv, e.ms_p5_mp_test_fdiv());
 if (p1) e.setdefaultprecision();
}
// 1.13c D2Client+0xCAD40
int64_t alldiv(uint32_t lo1, uint32_t hi1, uint32_t lo2, uint32_t hi2) {
 // The CRT routine is exact signed 64-bit truncating division.
 int64_t a = (int64_t)(((uint64_t)hi1 << 32) | lo1), b = (int64_t)(((uint64_t)hi2 << 32) | lo2);
 if (b == 0) return 0; // hardware #DE in retail
 if (a == INT64_MIN && b == -1) return INT64_MIN;
 return a / b;
}
// 1.13c D2Client+0xCADEA
void JumpToContinuation(const Env& e, P target) {
 e.wr32(e.excList, e.rd32(e.rd32(e.excList)));
 e.jump(target);
}
// 1.13c D2Client+0xCAE21
void UnwindNestedFrames(const Env& e, P frame, P rec) {
 P saved = e.rd32(e.excList);
 e.rtlUnwind(frame, e.unwindRet, rec, 0);
 e.wr32(rec + 4, e.rd32(rec + 4) & 0xfffffffd);
 e.wr32(saved, e.rd32(e.excList));
 e.wr32(e.excList, (uint32_t)saved);
}
// 1.13c D2Client+0xCAE73
uint32_t CxxFrameHandler(const Env& e, P rec, P reg, P ctx, P disp, P funcInfoEax) {
 return e.internalCxxFrameHandler(rec, reg, ctx, disp, funcInfoEax, 0, 0, 0);
}
// 1.13c D2Client+0xCAEE4
int32_t CallSETranslator(const Env& e, P rec, P reg, P ctx, P disp, P funcInfo, int32_t p6, P p7) {
 (void)ctx; (void)disp; (void)p6; (void)p7; (void)funcInfo;
 if (rec == 0x123) { e.wr32(reg, 0x6fb7af7f); return 1; }
 // Guard frame (TranslatorGuardRN) is linked into the SEH chain; translator invoked via ptd->_NLG_dwCode.
 P ptd = e.getptd();
 e.callNLG(ptd, e.rd32(rec), rec);
 return 0;
}
// 1.13c D2Client+0xCAFAB
int32_t TranslatorGuardHandler(const Env& e, P rec, P rn, P ctx, P disp) {
 (void)disp;
 if (e.rd32(rn + 8) == e.rd32(e.datB780)) {
  if ((e.rd32(rec + 4) & 0x66) == 0) {
   e.internalCxxFrameHandler(rec, e.rd32(rn + 0x10), ctx, 0, e.rd32(rn + 0xc), (int32_t)e.rd32(rn + 0x14), e.rd32(rn + 0x18), 1);
   if (e.rd32(rn + 0x24) == 0) UnwindNestedFrames(e, rn, rec);
   P l8 = e.getLocal8();
   CallSETranslator(e, 0x123, l8, 0, 0, 0, 0, 0);
   e.jump(e.rd32(l8));
   return 0;
  }
  e.wr32(rn + 0x24, 1);
 } else {
  e.wr32(rec + 4, e.rd32(rec + 4) | 8);
 }
 return 1;
}
// 1.13c D2Client+0xCB05D  (_s_FuncInfo: nTryBlocks @4, pTryBlockMap @8; entry 0x14: tryLow,tryHigh,catchHigh,..)
uint32_t GetRangeOfTrysToCheck(const Env& e, P fi, int32_t p2, int32_t p3, uint32_t* lo, uint32_t* hi) {
 uint32_t n = e.rd32(fi + 4), v1 = n, v5 = n, v4 = n, v3;
 P map = e.rd32(fi + 8);
 while (v3 = v5, p2 >= 0) {
  if (v1 == 0xffffffffu) e.inconsistency();
  v1--;
  P ent = map + (P)v1 * 0x14;
  if (((int32_t)e.rd32(ent + 4) < p3 && p3 <= (int32_t)e.rd32(ent + 8)) || (v5 = v3, v1 == 0xffffffffu)) {
   p2--; v5 = v1; v4 = v3;
  }
 }
 v1++;
 *lo = v1; *hi = v4;
 if (e.rd32(fi + 4) < v4 || v4 < v1) e.inconsistency();
 return (uint32_t)(map + (P)v1 * 0x14);
}
// 1.13c D2Client+0xCB0D7
P CreateFrameInfo(const Env& e, P fi, uint32_t p2) {
 P ptd = e.getptd();
 e.wr32(fi, p2);
 e.wr32(fi + 4, e.rd32(ptd + kPtdCurException));
 e.wr32(ptd + kPtdCurException, (uint32_t)fi);
 return fi;
}
}

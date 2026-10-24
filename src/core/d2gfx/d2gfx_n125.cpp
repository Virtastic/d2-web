// D2Gfx MSVC7.1 CRT C++ EH glue + DrawLine export. x86 FS:[0] / ESP jumps go through host seams (wasm has no SEH).
#include <cstdint>
namespace d2gfx_n125 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*wr32)(P, uint32_t);
 P excList;                         // FS:[0] cell
 void (*jump)(P);                   // indirect continuation jump
 P (*getptd)();                     // D2Gfx+0x28FE
 void (*callNLG)(P ptd, P arg);     // call [ptd+0x74]
 uint32_t (*internalCxx)(P rec, P reg, P ctx, P disp, P fi, int32_t, P, uint32_t);   // D2Gfx+0xC7A1
 void (*unwindNested)(P frame, P rec);                      // D2Gfx+0xBBF5
 void (*frameUnwind)(P frame, P fi, int32_t target);        // D2Gfx+0xC006
 void (*inconsistency)();           // D2Gfx+0xC878
 uint32_t (*findHandler)(P, P, P, P, P, uint8_t, int32_t, P);   // D2Gfx+0xC59D
 P datTranslator;                   // 6FA906E0
 void (*slot)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);  // renderer table [0x11268]+0xC0
 void (*callHook)(P fn, P, P, P, P, P, int32_t, P, uint8_t);
 void (*unlink)(P eax);             // D2Gfx+0x1250
};
// D2Gfx 0x0B9C0 (export 10010; probe-proven copy in d2gfx_disp.cpp): DrawLine, six args forwarded to the renderer table slot +0xC0.
void sub_B9C0(const Env& e, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t f, uint32_t g) { e.slot(a, b, c, d, f, g); }
// 1.13c D2Gfx+0x0BBBE
void JumpToContinuation(const Env& e, P target) {
 e.wr32(e.excList, e.rd32(e.rd32(e.excList)));
 e.jump(target);
}
// 1.13c D2Gfx+0x0BCB8
int32_t CallSETranslator(const Env& e, P rec, P reg) {
 if (rec == 0x123) { e.wr32(reg, 0x6fa8bd53); return 1; }
 e.callNLG(e.getptd(), rec);   // translator guard frame linked on FS:[0] in retail
 return 0;
}
// 1.13c D2Gfx+0x0BD7F
int32_t TranslatorGuardHandler(const Env& e, P rec, P rn, P ctx, P l8) {
 if (e.rd32(rn + 8) == e.rd32(e.datTranslator)) {
  if ((e.rd32(rec + 4) & 0x66) == 0) {
   e.internalCxx(rec, e.rd32(rn + 0x10), ctx, 0, e.rd32(rn + 0xc), (int32_t)e.rd32(rn + 0x14), e.rd32(rn + 0x18), 1);
   if (e.rd32(rn + 0x24) == 0) e.unwindNested(rn, rec);
   CallSETranslator(e, 0x123, l8);
   e.jump(e.rd32(l8));
   return 0;
  }
  e.wr32(rn + 0x24, 1);
 } else e.wr32(rec + 4, e.rd32(rec + 4) | 8);
 return 1;
}
// 1.13c D2Gfx+0x0C478
void CatchIt(const Env& e, P rec, P funcInfo, P esi, P edi, P nested, P cont) {
 if (!nested) nested = esi;
 e.unwindNested(nested, rec);
 e.frameUnwind(esi, funcInfo, (int32_t)e.rd32(edi));
 e.wr32(esi + 0xc, e.rd32(edi + 4) + 1);
 if (cont) JumpToContinuation(e, cont);   // cont = CallCatchBlock result
}
// 1.13c D2Gfx+0x0C7A1
uint32_t InternalCxxFrameHandler(const Env& e, P rec, P reg, P ctx, P disp, P fi, int32_t catchDepth, P frame, uint8_t isRethrow) {
 if ((e.rd32(fi) & 0x1fffffff) != 0x19930520) e.inconsistency();
 if (e.rd32(rec + 4) & 0x66) {
  if (e.rd32(fi + 4) != 0 && catchDepth == 0) e.frameUnwind(reg, fi, -1);   // unwind to state -1
 } else if (e.rd32(fi + 0xc) != 0) {
  P hook;   // 0xE06D7363 C++ throw with ThrowInfo->pForwardCompat: call it instead of FindHandler
  if (e.rd32(rec) == 0xe06d7363 && e.rd32(rec + 0x14) > 0x19930520 && (hook = e.rd32(e.rd32(rec + 0x1c) + 8)) != 0)
   e.callHook(hook, rec, reg, ctx, disp, fi, catchDepth, frame, isRethrow);
  else
   e.findHandler(rec, reg, ctx, disp, fi, isRethrow, catchDepth, frame);
 }
 return 1;
}
// 1.13c D2Gfx+0x0CB80 Unwind@6fa8cb80: funclet, eax=[ebp+4], jmp 0x1250
void Unwind_cb80(const Env& e, P ebp) { e.unlink(e.rd32(ebp + 4)); }
// 1.13c D2Gfx+0x0CBA5 Unwind@6fa8cba5: funclet, eax=[ebp+0xc], jmp 0x1250
void Unwind_cba5(const Env& e, P ebp) { e.unlink(e.rd32(ebp + 0xc)); }
}

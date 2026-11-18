// D2Client 0xCB64D..0xCBF2C batch: MSVC7.1 C++ EH runtime (TypeMatch .. _inconsistency).
// Approximate: SEH frames/unwind/longjmp are not expressible, so unrecovered callees and the
// unaff_* register arguments are explicit Env members / parameters. Retail memory via rd32/wr32.
#include <cstdint>
namespace d2client_batch_cb64d {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 uint8_t (*rd8)(P);
 int32_t (*strcmp_)(P, P);                         // 0x4A60
 P (*getptd)();                                    // __getptd; ptd fields at +0x58 purecall,+0x50 unexpected? (positional)
 P ptdPurecall, ptdUnexpected, ptdTranslator, ptdLocinfo;  // offsets within ptd (_purecall,_unexpected,_translator,ptlocinfo)
 P ptrTerminate;                                   // PTR_terminate_6fbaa5a4 (value)
 void (*abort_)();                                 // _abort
 void (*callSettingFrame)(P, P, uint32_t);         // __CallSettingFrame_12
 void (*callMemberFn1)(P, P);                      // FID_conflict__CallMemberFunction1
 void (*fn6fb7b76d)();                             // 0xCB76D
 P (*createFrameInfo)(P, P); void (*findAndUnlinkFrame)(P);
 P (*callCatchBlock2)(P, P, P, P, int32_t);
 int32_t (*isExcObjToBeDestroyed)(P); void (*abnormalTermination)();
 int32_t (*validateRead)(P, int); int32_t (*validateWrite)(P, int); int32_t (*validateExecute)(P);
 P (*adjustPointer)(P, P); void (*memmove_)(P, P, uint32_t);
 void (*unwindNestedFrames)(P, P); void (*jumpToContinuation)(P, P);
 int32_t (*callSETranslator)(P, P, P, P, P, int32_t, P);
 P (*getRangeOfTrysToCheck)(P, int32_t, int32_t, uint32_t*, uint32_t*);
};
static void terminate_(const Env& e);
// 1.13c D2Client+0xCBF2C
void _inconsistency(const Env& e) {
 if (e.ptrTerminate) { /* ponytail: registered handler is a code ptr, called via env */ }
 terminate_(e);
}
// 1.13c D2Client+0xCBEF7
static void terminate_(const Env& e) {
 P ptd = e.getptd();
 (void)e.rd32(ptd + e.ptdLocinfo);  // handler (ptlocinfo slot) invoked by platform; then _abort
 e.abort_();
}
void terminate(const Env& e) { terminate_(e); }
// 1.13c D2Client+0xCB64D  (esi=catchable, edi=handler type; adjectives = param_1->adjectives)
int32_t TypeMatch(const Env& e, uint32_t adjectives, P esi, P edi) {
 P t = e.rd32(esi + 4);
 if (t && e.rd8(t + 8)) {
  P u = e.rd32(edi + 4);
  if (t != u && e.strcmp_(t + 8, u + 8) != 0) return 0;
  if ((((e.rd8(edi) & 2) && !(e.rd8(esi) & 8)) || ((adjectives & 1) && !(e.rd8(esi) & 1))) ||
      ((adjectives & 2) && !(e.rd8(esi) & 2))) return 0;
 }
 return 1;
}
// 1.13c D2Client+0xCB6BA
void FrameUnwindToState(const Env& e, P frame, P /*unused*/, P funcInfo, int32_t target) {
 int32_t st = (int32_t)e.rd32(frame + 8);
 P ptd = e.getptd();
 e.wr32(ptd + e.ptdPurecall, e.rd32(ptd + e.ptdPurecall) + 1);
 while (st != target) {
  if (st < 0 || (int32_t)e.rd32(funcInfo + 4) <= st) _inconsistency(e);
  P ent = e.rd32(funcInfo + 8) + (P)st * 8;
  st = (int32_t)e.rd32(ent);
  if (e.rd32(ent + 4)) { e.wr32(frame + 8, (uint32_t)st); e.callSettingFrame(e.rd32(ent + 4), frame, 0x103); }
 }
 e.fn6fb7b76d();
 if (st != target) _inconsistency(e);
 e.wr32(frame + 8, (uint32_t)st);
}
// 1.13c D2Client+0xCB788
void DestructExceptionObject(const Env& e, P exc) {
 if (!exc) return;
 P fn = e.rd32(e.rd32(exc + 0x1c) + 4);
 if (fn) e.callMemberFn1(e.rd32(exc + 0x18), fn);
}
// 1.13c D2CMP+0x19197
// 1.13c D2Client+0xCB941  (SEH finally part of CallCatchBlock; locals passed in)
void CallCatchBlock_finally(const Env& e, P edi, P exc, uint32_t savedFrame, P frameInfo,
                            P oldUnexpected, P oldTranslator, int32_t flag48, int32_t flag20) {
 e.wr32(edi - 4, savedFrame);
 e.findAndUnlinkFrame(frameInfo);
 P ptd = e.getptd(); e.wr32(ptd + e.ptdUnexpected, (uint32_t)oldUnexpected);
 e.wr32(ptd + e.ptdTranslator, (uint32_t)oldTranslator);
 if (e.rd32(exc) == 0xE06D7363u && e.rd32(exc + 0x10) == 3 &&
     (e.rd32(exc + 0x14) == 0x19930520 || e.rd32(exc + 0x14) == 0x19930521) && flag48 == 0 && flag20 != 0 &&
     e.isExcObjToBeDestroyed(e.rd32(exc + 0x18))) { e.abnormalTermination(); DestructExceptionObject(e, exc); }
}
// 1.13c D2Client+0xCB7EC
P CallCatchBlock(const Env& e, P exc, P regNode, P ctx, P funcInfo, P handler, int32_t adj, uint32_t /*unused*/) {
 P fi = e.createFrameInfo(0, e.rd32(exc + 0x18));
 P ptd = e.getptd();
 P oldU = e.rd32(ptd + e.ptdUnexpected), oldT = e.rd32(ptd + e.ptdTranslator);
 e.wr32(ptd + e.ptdUnexpected, (uint32_t)exc); e.wr32(ptd + e.ptdTranslator, (uint32_t)ctx);
 P r = e.callCatchBlock2(regNode, funcInfo, 0, handler, adj);
 CallCatchBlock_finally(e, regNode, exc, e.rd32(regNode - 4), fi, oldU, oldT, 0, 1);
 return r;
}
// 1.13c D2Client+0xCB9B0  (in_ECX=catchable, in_EDX=frame base, param_2=handler type)
void BuildCatchObject(const Env& e, P exc, P handler, P catchable, P edx, P adjA, P adjB) {
 if (!e.rd32(handler + 4) || !e.rd8(e.rd32(handler + 4) + 8)) return;
 if (!e.rd32(handler + 8) && !(e.rd8(handler + 3) & 0x80)) return;
 P dst = edx;
 if ((int32_t)e.rd32(handler) >= 0) dst = e.rd32(handler + 8) + 0xc + edx;
 P src = e.rd32(exc + 0x18);
 bool ok = false;
 if (!(e.rd32(handler) & 8)) {
  if (!(e.rd8(catchable) & 1)) {
   if (!e.rd32(catchable + 0x18)) {
    if (e.validateRead(src, 1) && e.validateWrite(dst, 1)) { e.memmove_(dst, e.adjustPointer(e.rd32(catchable + 0x14), adjA), e.rd32(catchable + 0x14)); return; }
   } else if (e.validateRead(src, 1) && e.validateWrite(dst, 1) && e.validateExecute(e.rd32(catchable + 0x18))) {
    e.adjustPointer(1, adjA); e.callMemberFn1(dst, e.rd32(catchable + 0x18)); return;
   }
  } else if (e.validateRead(src, 1) && e.validateWrite(dst, 1)) {
   e.memmove_(dst, src, e.rd32(catchable + 0x14));
   if (e.rd32(catchable + 0x14) != 4 || e.rd32(dst) == 0) return;
   e.wr32(dst, (uint32_t)e.adjustPointer(adjA, adjB)); return;
  }
 } else if (e.validateRead(src, 1) && e.validateWrite(dst, 1)) {
  e.wr32(dst, (uint32_t)src); ok = true;
  e.wr32(dst, (uint32_t)e.adjustPointer(adjA, adjB)); return;
 }
 (void)ok; _inconsistency(e);
}
// 1.13c D2Client+0xCBB2C  (unaff regs: ecx=catchable?, ebx=catch obj ctx, esi=frame, edi=tryblock)
void CatchIt(const Env& e, P exc, P ctx, P funcInfo, P handler, P catchable, P esi, P edi, int32_t hasObj,
             P ebx, P ebp, P nested, P edx, P adjA, P adjB) {
 if (hasObj) BuildCatchObject(e, exc, ebp, catchable, edx, adjA, adjB);
 if (!nested) nested = esi;
 e.unwindNestedFrames(nested, exc);
 FrameUnwindToState(e, esi, 0, funcInfo, (int32_t)e.rd32(edi));
 e.wr32(esi + 0xc, e.rd32(edi + 4) + 1);
 (void)ebx;
 P cont = CallCatchBlock(e, exc, esi, ctx, funcInfo, handler, 0x100, 0);
 if (cont) e.jumpToContinuation(cont, esi);
}
// 1.13c D2Client+0xCBB93
void FindHandlerForForeignException(const Env& e, P exc, P regNode, P ctx, P disp, P funcInfo,
                                    int32_t state, int32_t catchDepth, P frame,
                                    void (*catchIt)(const Env&, P, P, P, P, P, P)) {
 if (e.rd32(exc) == 0x80000003u) return;
 P ptd = e.getptd();
 if (e.rd32(ptd + 0x58 /*_NLG_dwCode*/) &&
     e.callSETranslator(exc, regNode, ctx, disp, funcInfo, catchDepth, frame)) return;
 uint32_t lo = 0, hi = 0;
 P t = e.getRangeOfTrysToCheck(funcInfo, catchDepth, state, &lo, &hi);
 for (; lo < hi; lo++, t += 0x14) {
  if ((int32_t)e.rd32(t) <= state && state <= (int32_t)e.rd32(t + 4)) {
   P ha = e.rd32(t + 0x10) + (P)(e.rd32(t + 8) - 1) * 0x10;
   P ty = e.rd32(ha + 4);
   if (!ty || !e.rd8(ty + 8)) catchIt(e, exc, regNode, ctx, disp, funcInfo, t);
  }
 }
}
// 1.13c D2Client+0xCBC51
void FindHandler(const Env& e, P exc, P regNode, P ctx, P disp, P funcInfo, uint8_t isRethrow,
                 int32_t catchDepth, P frame,
                 void (*catchIt)(const Env&, P, P, P, P, P, P), P esi, P edi) {
 int32_t state = (int32_t)e.rd32(regNode + 8);
 if (state < -1 || (int32_t)e.rd32(funcInfo + 4) <= state) _inconsistency(e);
 auto isCxx = [&](P x) { return e.rd32(x) == 0xE06D7363u && e.rd32(x + 0x10) == 3 &&
                        (e.rd32(x + 0x14) == 0x19930520 || e.rd32(x + 0x14) == 0x19930521); };
 uint8_t rethrown = 0;
 if (e.rd32(exc) == 0xE06D7363u) {
  if (isCxx(exc) && e.rd32(exc + 0x1c) == 0) {
   P ptd = e.getptd();
   P cur = e.rd32(ptd + e.ptdUnexpected);
   if (!cur) return;
   exc = cur; ctx = e.rd32(ptd + e.ptdTranslator); rethrown = 1;
   if (!e.validateRead(exc, 1)) _inconsistency(e);
   if (e.rd32(exc) == 0xE06D7363u && isCxx(exc) && e.rd32(exc + 0x1c) == 0) _inconsistency(e);
  }
  if (e.rd32(exc) == 0xE06D7363u && isCxx(exc)) {
   uint32_t lo = 0, hi = 0;
   P t = e.getRangeOfTrysToCheck(funcInfo, catchDepth, state, &lo, &hi);
   for (; lo < hi; lo++, t += 0x14) {
    if ((int32_t)e.rd32(t) <= state && state <= (int32_t)e.rd32(t + 4)) {
     // ponytail: catchable-type walk -> TypeMatch -> CatchIt; CatchIt register args unrecovered, via callback
     P tbl = e.rd32(e.rd32(exc + 0x1c) + 0xc);
     for (int32_t n = (int32_t)e.rd32(t + 8); n > 0; n--)
      for (int32_t k = (int32_t)e.rd32(tbl), q = 0; k > 0; k--, q++)
       if (TypeMatch(e, e.rd32(e.rd32(t + 0x10)), e.rd32(tbl + 4 + 4 * q), edi)) { catchIt(e, exc, regNode, ctx, disp, funcInfo, t); goto next; }
    }
    next:;
   }
   if (isRethrow) DestructExceptionObject(e, exc);
   (void)esi; (void)rethrown;
   return;
  }
 }
 if (!isRethrow) FindHandlerForForeignException(e, exc, regNode, ctx, disp, funcInfo, state, catchDepth, frame, catchIt);
 else terminate_(e);
}
// 1.13c D2Client+0xCBE55
uint32_t InternalCxxFrameHandler(const Env& e, P exc, P regNode, P ctx, P disp, P funcInfo, int32_t catchDepth,
                                 P frame, uint8_t isRethrow, void (*catchIt)(const Env&, P, P, P, P, P, P)) {
 if ((e.rd32(funcInfo) & 0x1fffffff) != 0x19930520) _inconsistency(e);
 if ((e.rd8(exc + 4) & 0x66) == 0) {
  if (e.rd32(funcInfo + 0xc)) {
   if (e.rd32(exc) == 0xE06D7363u && e.rd32(exc + 0x14) > 0x19930520u && e.rd32(e.rd32(exc + 0x1c) + 8))
    return 1;  // ponytail: indirect handler (exc->throwInfo->forwardCompat) not callable via Env
   FindHandler(e, exc, regNode, ctx, disp, funcInfo, isRethrow, catchDepth, frame, catchIt, 0, 0);
  }
 } else if (e.rd32(funcInfo + 4) && catchDepth == 0) FrameUnwindToState(e, regNode, disp, funcInfo, -1);
 return 1;
}
}

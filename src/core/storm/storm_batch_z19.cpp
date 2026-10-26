#include <cstdint>
#include <cstring>
// Seams: retail globals/callees (32-bit pointers carried as uint32_t).
extern uint32_t z19_lastError;                       // DAT_6fc45388
void z19_SetLastError(uint32_t e);
uint32_t z19_StrLen(const char* s);                  // Storm+0x3AFD0
void* z19_Alloc(uint32_t n, const char* file, int line, int flags); // Storm+0x1C580
void z19_RegPath(uint32_t dst, const char* prefix, uint32_t src);   // Storm+0x3B080
void z19_Inconsistency();                            // CRT _inconsistency
uint32_t* z19_Ptd();                                 // __getptd(); [+0x1c]=_curexception (per-thread data)
extern uint32_t z19_curexception_off;

// 1.13c Storm+0x3BBF0: SStrDup -- allocate strlen+1, copy.
char* z19_SStrDup(const char* s) {
  if (!s) { z19_lastError = 0x57; z19_SetLastError(0x57); return nullptr; }
  uint32_t n = z19_StrLen(s) + 1;
  char* d = (char*)z19_Alloc(n, "..\\3rdParty\\STORM\\SOURCE\\SSTR.CPP", 0x1f2, 0);
  memcpy(d, s, n);
  return d;
}
// 1.13c Storm+0x3BC50: registry base path; flags&2 -> Battle.net, else Blizzard Entertainment.
int z19_RegBase(uint8_t flags, uint32_t dst, uint32_t src) {
  if (!dst || !src) { z19_lastError = 0x57; z19_SetLastError(0x57); return 0; }
  z19_RegPath(dst, (flags & 2) ? "Software\\Battle.net\\" : "Software\\Blizzard Entertainment\\", src);
  return 1;
}
// GUESSED (SEH/asm CRT stubs, no portable body): 3C4D5 3C505 3C50C 3C55E 3C5CF 3C857 3C8E9 are intentionally empty.
// 1.13c Storm+0x3C4D5: _JumpToContinuation -- pops SEH list, jumps; EMPTY (x86 asm).
void z19_JumpToContinuation(void*, void*) {}
// 1.13c Storm+0x3C505: _CallMemberFunction1 -- tail-jumps to target; EMPTY (x86 asm).
void z19_CallMemberFunction1(uint32_t, void*) {}
// 1.13c Storm+0x3C50C: _UnwindNestedFrames -- RtlUnwind then clear EH_UNWINDING bit; EMPTY (SEH).
void z19_UnwindNestedFrames(void*, uint32_t* rec) { rec[1] &= 0xfffffffdu; }
// 1.13c Storm+0x3C55E: ___CxxFrameHandler -- forwards to InternalCxxFrameHandler; EMPTY (EAX-passed FuncInfo).
uint32_t z19_CxxFrameHandler(void*, void*, void*, void*) { return 0; }
// 1.13c Storm+0x3C5CF: _CallSETranslator -- SEH translator call; EMPTY (SEH frame).
int z19_CallSETranslator(void*, void*, void*, void*, void*, int, void*) { return 0; }

struct z19_TryBlock { uint32_t tryLow, tryHigh, catchHigh, nCatches, pCatches; };
struct z19_FuncInfo { uint32_t magic, maxState, pUnwind, nTry, pTryBlockMap; };
// 1.13c Storm+0x3C748: _GetRangeOfTrysToCheck
z19_TryBlock* z19_GetRangeOfTrysToCheck(z19_FuncInfo* fi, int depth, int state, uint32_t* lo, uint32_t* hi) {
  uint32_t n = fi->nTry;
  z19_TryBlock* map = (z19_TryBlock*)(uintptr_t)fi->pTryBlockMap;
  uint32_t cur = n, end = n;
  while (depth >= 0) {
    uint32_t prev = cur;
    if (n == 0xffffffffu) z19_Inconsistency();
    n--;
    if ((map[n].tryHigh < (uint32_t)state && (uint32_t)state <= map[n].catchHigh) || n == 0xffffffffu) {
      depth--; cur = n; end = prev;
    } else cur = prev;
  }
  n++;
  *lo = n; *hi = end;
  if (fi->nTry < end || end < n) z19_Inconsistency();
  return map + n;
}
// 1.13c Storm+0x3C7C2: _CreateFrameInfo -- push frame onto per-thread list.
uint32_t* z19_CreateFrameInfo(uint32_t* fr, uint32_t obj) {
  uint32_t* ptd = z19_Ptd();
  fr[0] = obj; fr[1] = ptd[z19_curexception_off / 4]; ptd[z19_curexception_off / 4] = (uint32_t)(uintptr_t)fr;
  return fr;
}
// 1.13c Storm+0x3C80B: _FindAndUnlinkFrame
void z19_FindAndUnlinkFrame(uint32_t* fr) {
  uint32_t* ptd = z19_Ptd(); uint32_t& head = ptd[z19_curexception_off / 4];
  if ((uint32_t)(uintptr_t)fr == head) { head = fr[1]; return; }
  uint32_t* p = (uint32_t*)(uintptr_t)head;
  for (;;) {
    if (!p[1]) { z19_Inconsistency(); return; }
    if ((uint32_t)(uintptr_t)fr == p[1]) break;
    p = (uint32_t*)(uintptr_t)p[1];
  }
  p[1] = fr[1];
}
// 1.13c Storm+0x3C857: _CallCatchBlock2 -- builds SEH frame, calls __CallSettingFrame; EMPTY (SEH/asm).
void* z19_CallCatchBlock2(void*, void*, void*, int, uint32_t) { return nullptr; }
// 1.13c Storm+0x3C8E9: __fpmath -- x87 init + P5 FDIV test; EMPTY (x87 control word).
void z19_fpmath(int) {}

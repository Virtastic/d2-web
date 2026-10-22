// b69 batch: BNClient CRT/helper rows. Skipped (mangled register decompile, SEH or direct Win32):
// 0xA9D0 0xB3A0 0xBC70 0xBD40 0xF0A0 0xF240 0xF3C0 0xF430 0x1788F 0x17A07
#include <cstdint>
#include <initializer_list>

struct FrameInfo { void* pExceptionObject; FrameInfo* pNext; };
struct PerThreadData { FrameInfo* curexception; int32_t purecall; };  // fields of the CRT _tiddata used here

extern "C" void BNClient_inconsistency();
extern "C" int Storm_SStrCopy(char* dst, const char* src, unsigned max);  // Ordinal_501
extern "C" void BNClient_Send(int len, int msgId);                        // 0x2EB00

// 1.13c BNClient+0x17843 (_FindAndUnlinkFrame; ptd = __getptd())
void FindAndUnlinkFrame(PerThreadData* ptd, FrameInfo* f) {
    if (f == ptd->curexception) { ptd->curexception = f->pNext; return; }
    FrameInfo* p = ptd->curexception;
    for (;;) {
        if (!p->pNext) { BNClient_inconsistency(); return; }
        if (p->pNext == f) break;
        p = p->pNext;
    }
    p->pNext = f->pNext;
}

// 1.13c BNClient+0x178F0 (__alldiv)
int64_t alldiv(int64_t a, int64_t b) { return a / b; }

// 1.13c BNClient+0x17ABA
void DecrementPurecall(PerThreadData* ptd) {
    if (ptd->purecall > 0) ptd->purecall--;
}

// 1.13c BNClient+0xF050
void SendServerIniName() {
    char buf[260];
    int n = Storm_SStrCopy(buf, "bnserver-D2DV.ini", 0x104);
    BNClient_Send(n + 9, 0x33);
}

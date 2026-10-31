// b69 batch: BNClient CRT/helper rows. Skipped (mangled register decompile, SEH or direct Win32):
// 0xA9D0 0xB3A0 0xBC70 0xBD40 0xF0A0 0xF240 0xF3C0 0xF430 0x1788F 0x17A07
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct FrameInfo { void* pExceptionObject; FrameInfo* pNext; };
struct PerThreadData { FrameInfo* curexception; int32_t purecall; };  // fields of the CRT _tiddata used here

extern "C" void BNClient_inconsistency();
extern "C" int Storm_SStrCopy(char* dst, const char* src, unsigned max);  // Ordinal_501
extern "C" void BNClient_Send(int len, int msgId, const void* payload);   // 0x2EB00 (EDI = payload)

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
// Payload (EDI): u32 0x80000004 (root key), u32 0 (request id), then the name with its NUL, n+1 bytes.
// The sender copies n+9 of these bytes (ECX = n+9, id 0x33), so the frame is n+13 bytes.
void SendServerIniName() {
    char buf[260];
    uint8_t payload[8 + 260];
    const uint32_t rootKey = 0x80000004u, requestId = 0;
    std::memcpy(payload, &rootKey, 4);
    std::memcpy(payload + 4, &requestId, 4);
    int n = Storm_SStrCopy(buf, "bnserver-D2DV.ini", 0x104);
    std::memcpy(payload + 8, buf, (size_t)n + 1);
    BNClient_Send(n + 9, 0x33, payload);
}

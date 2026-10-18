// D2Client+0xBB34 = MSVC 2003 CRT _chsize(fd, size): truncate or extend an open file
//. Every external effect is a callee, so the rule is expressed against
// the host seam in retail's call order; the probe supplies the scripted returns.
#include <cstdint>
#include "client_host.h"

namespace {
constexpr uint32_t kLseek = 0x8678, kWrite = 0x5943, kSetmode = 0xBCE1, kGetOsHandle = 0xA63A;
constexpr uint32_t kErrno = 0x579C, kDosErrno = 0x57A5;
// The two imports are called through their IAT slots: 0x6FB7EEB8 / 0x6FB7EF34 minus image base 0x6FAB0000.
constexpr uint32_t kSetEndOfFile = 0xCEEB8, kGetLastError = 0xCEF34;
static_assert(kSetEndOfFile + 0x6FAB0000u == 0x6FB7EEB8u, "SetEndOfFile IAT slot");
static_assert(kGetLastError + 0x6FAB0000u == 0x6FB7EF34u, "GetLastError IAT slot");
constexpr uint32_t kChunk = 0x1000;

inline uint32_t go(D2cHost h, uint32_t rva, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
    D2cCall c = {};
    c.rva = rva;
    c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return h(&c);
}
}  // namespace

// 1.13c D2Client+0xBB34. cdecl; [esp+4] = fd, [esp+8] = size (signed); returns 0 or -1. The stack
// buffer retail memsets and writes is the 0x1000-byte zero array below; it is never read, only its
// address is passed to _write (the recorder logs that stack pointer as 0xB3000000).
extern "C" int32_t d2_client_bb34(uint32_t fd, int32_t size, D2cHost h) {
    uint32_t cur = go(h, kLseek, fd, 0, 1, 0);            // SEEK_CUR
    if (cur == 0xFFFFFFFFu) return -1;
    uint32_t end = go(h, kLseek, fd, 0, 2, 0);            // SEEK_END
    if (end == 0xFFFFFFFFu) return -1;
    int32_t delta = (int32_t)((uint32_t)size - end);      // unsigned difference, signed test
    int32_t rc = 0;
    if (delta < 0) {                                      // shrink
        go(h, kLseek, fd, (uint32_t)size, 0, 0);          // SEEK_SET; the result is ignored
        uint32_t hFile = go(h, kGetOsHandle, fd, 0, 0, 0);
        uint32_t ok = go(h, kSetEndOfFile, hFile, 0, 0, 0);
        rc = (ok != 0) ? 0 : -1;
        if (rc == -1) {
            *(uint32_t*)(uintptr_t)go(h, kErrno, 0, 0, 0, 0) = 0xD;             // EACCES
            *(uint32_t*)(uintptr_t)go(h, kDosErrno, 0, 0, 0, 0) = go(h, kGetLastError, 0, 0, 0, 0);
        }
    } else if (delta > 0) {                               // grow
        alignas(4) uint8_t zero[kChunk] = {};             // retail: _memset(local_1008, 0, 0x1000)
        int32_t mode = (int32_t)go(h, kSetmode, fd, 0x8000, 0, 0);   // O_BINARY
        for (;;) {
            uint32_t count = (delta < (int32_t)kChunk) ? (uint32_t)delta : kChunk;
            int32_t n = (int32_t)go(h, kWrite, fd, (uint32_t)(uintptr_t)zero, count, 0);
            if (n == -1) {
                if (*(uint32_t*)(uintptr_t)go(h, kDosErrno, 0, 0, 0, 0) == 5)   // ERROR_ACCESS_DENIED
                    *(uint32_t*)(uintptr_t)go(h, kErrno, 0, 0, 0, 0) = 0xD;
                rc = -1;
                break;
            }
            delta = (int32_t)((uint32_t)delta - (uint32_t)n);
            if (delta <= 0) break;
        }
        go(h, kSetmode, fd, (uint32_t)mode, 0, 0);        // restore the saved mode
    }
    go(h, kLseek, fd, cur, 0, 0);                         // restore the position
    return rc;
}

namespace {
// The three kernel32 imports are raw IAT-slot calls (retail 0x6FB7EF80 / 0x6FB7EF74 / 0x6FB7EF48),
// and the two literals are at 0x6FB7F174 ("mscoree.dll") / 0x6FB7F164 ("CorExitProcess").
constexpr uint32_t kGetModuleHandleA = 0xCEF80, kGetProcAddress = 0xCEF74, kExitProcess = 0xCEF48;
static_assert(kGetModuleHandleA + 0x6FAB0000u == 0x6FB7EF80u, "GetModuleHandleA IAT slot");
static_assert(kGetProcAddress + 0x6FAB0000u == 0x6FB7EF74u, "GetProcAddress IAT slot");
static_assert(kExitProcess + 0x6FAB0000u == 0x6FB7EF48u, "ExitProcess IAT slot");
}  // namespace

// 1.13c D2Client+0x34DD = MSVC 2003 CRT ___crtExitProcess(code). Rule: hModule =
// GetModuleHandleA("mscoree.dll"); if (hModule) { p = GetProcAddress(hModule, "CorExitProcess");
// if (p) (*p)(code); } ExitProcess(code). No D2Client global or struct is touched, and no path returns
// in retail (int3 pad). The host seam stands in for the three imports; the indirect CorExitProcess call
// is reported as a D2cCall whose rva is the address GetProcAddress returned.
extern "C"
void d2_client_34dd(int32_t code, D2cHost h) {
    static const char kMscoree[] = "mscoree.dll";
    static const char kCorExit[] = "CorExitProcess";
    uint32_t mod = go(h, kGetModuleHandleA, (uint32_t)(uintptr_t)kMscoree, 0, 0, 0);
    if (mod != 0) {
        uint32_t p = go(h, kGetProcAddress, mod, (uint32_t)(uintptr_t)kCorExit, 0, 0);
        if (p != 0) {
            D2cCall c = {};
            c.rva = p;
            c.s[0] = (uint32_t)code;
            h(&c);
        }
    }
    go(h, kExitProcess, (uint32_t)code, 0, 0, 0);
}

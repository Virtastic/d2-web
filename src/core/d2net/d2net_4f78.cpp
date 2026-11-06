// 1.13c D2Net+0x4F78 ___crtGetStringTypeA: cdecl(type, src, cch, out, cp, lcid, err).
// The API mode (0xB140) is probed once: GetStringTypeW(1, L"", 1, &t) succeeding = 1 (wide), else GetLastError 0x78 = 2
// (ANSI); any other error leaves 0, which also takes the ANSI path. Wide: cp 0 = __lc_codepage (0xB138); n =
// MultiByteToWideChar(cp, err ? 9 : 1, src, cch, 0, 0), a zeroed n-word stack buffer (_calloc_crt only when the stack
// probe fails), the second MultiByteToWideChar(cp, 1, ...) and GetStringTypeW(type, w, its count, out). ANSI: lcid 0 =
// __lc_handle (0xB128), cp 0 = __lc_codepage; ___ansicp(lcid) -1 fails; another codepage is converted by __convertcp
// (cp to that page, the length through &cch) and the copy freed after GetStringTypeA(lcid, type, s, cch, out).
#include <alloca.h>
#include <cstdint>
#include <cstring>
namespace d2net_4f78 {
struct Host {
    int (*GetStringTypeW)(uint32_t type, const uint16_t* src, int cch, uint16_t* out);
    uint32_t (*GetLastError)();
    int (*MBToWC)(uint32_t cp, uint32_t flags, const char* src, int cch, uint16_t* dst, int dstlen);
    int (*GetStringTypeA)(uint32_t lcid, uint32_t type, const char* src, int cch, uint16_t* out);
    int (*ansicp)(uint32_t lcid);                                           // 0x5909
    char* (*convertcp)(uint32_t from, uint32_t to, const char* src, int* plen, char* dst, int dstlen);   // 0x5950
    void* (*calloc_crt)(uint32_t n, uint32_t size);                         // 0x1F61
    void (*free_)(void*);                                                   // 0x1EF0
};
struct Globals { uint32_t mode, lc_handle, lc_codepage; };   // 0xB140, 0xB128, 0xB138
extern const uint16_t kEmptyW[1];
const uint16_t kEmptyW[1] = { 0 };                           // the module's L"" (0x8FAC)
// 1.13c D2Net+0x4F78
int crtGetStringTypeA(const Host& h, Globals& g, uint32_t type, const char* src, int cch, uint16_t* out,
                      uint32_t cp, uint32_t lcid, int err) {
    if (g.mode == 0) {
        uint16_t t[2];
        if (h.GetStringTypeW(1, kEmptyW, 1, t)) g.mode = 1;
        else if (h.GetLastError() == 0x78) g.mode = 2;
    }
    if (g.mode == 2 || g.mode == 0) {
        if (lcid == 0) lcid = g.lc_handle;
        if (cp == 0) cp = g.lc_codepage;
        const int acp = h.ansicp(lcid);
        if (acp == -1) return 0;
        char* conv = nullptr;
        if ((uint32_t)acp != cp) {
            conv = h.convertcp(cp, (uint32_t)acp, src, &cch, nullptr, 0);
            if (!conv) return 0;
            src = conv;
        }
        const int r = h.GetStringTypeA(lcid, type, src, cch, out);
        if (conv) h.free_(conv);
        return r;
    }
    if (g.mode != 1) return 0;
    if (cp == 0) cp = g.lc_codepage;
    const int n = h.MBToWC(cp, err ? 9 : 1, src, cch, nullptr, 0);
    if (n == 0) return 0;
    uint16_t* w = (uint16_t*)alloca(((uint32_t)n * 2 + 3) & ~3u);
    bool heap = false;
    if (w) memset(w, 0, (size_t)n * 2);
    else {
        w = (uint16_t*)h.calloc_crt(2, (uint32_t)n);
        if (!w) return 0;
        heap = true;
    }
    int r = 0;
    const int m = h.MBToWC(cp, 1, src, cch, w, n);
    if (m != 0) r = h.GetStringTypeW(type, w, m, out);
    if (heap) h.free_(w);
    return r;
}
}

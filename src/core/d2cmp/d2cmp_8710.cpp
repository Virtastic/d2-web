// CRT __convertcp: converts a string between two code pages through UTF-16. The Win32 NLS calls and the
// CRT heap are the host's (env); the wide scratch buffer is retail's alloca (zeroed, wlen UTF-16 units).
#include <cstdint>
#include <cstring>
#include <vector>

struct D2Cmp8710Env {
    uint32_t (*getCPInfo)(uint32_t cp, uint32_t* maxCharSize);   // BOOL; CPINFO.MaxCharSize is its first dword
    int32_t (*mb2wc)(uint32_t cp, uint32_t flags, const char* s, int32_t n, uint16_t* w, int32_t wn);
    int32_t (*wc2mb)(uint32_t cp, uint32_t flags, const uint16_t* w, int32_t wn, char* s, int32_t sn, const char* def, int32_t* used);
    void* (*calloc_)(uint32_t num, uint32_t size);   // 0x40F3
    void (*free_)(void* p);                           // 0x2EBE
};

// 1.13c D2CMP+0x8710
extern "C" char* d2cmp_8710(const D2Cmp8710Env* e, uint32_t from, uint32_t to, const char* src, uint32_t* len, char* dst,
                            int32_t dstLen) {
    char* out = nullptr;
    const uint32_t n = *len;
    if (from == to) return nullptr;
    bool single = false;
    uint32_t mx = 0;
    if (e->getCPInfo(from, &mx) && mx == 1 && e->getCPInfo(to, &mx) && mx == 1) single = true;
    uint32_t wlen = 0;
    if (single) {
        wlen = n;
        if (n == 0xffffffffu) wlen = uint32_t(strlen(src)) + 1;
    } else {
        wlen = uint32_t(e->mb2wc(from, 1, src, int32_t(n), nullptr, 0));
        if (!wlen) return nullptr;
    }
    std::vector<uint16_t> w(wlen, 0);
    if (e->mb2wc(from, 1, src, int32_t(n), w.data(), int32_t(wlen))) {
        if (!dst) {
            if ((single || (wlen = uint32_t(e->wc2mb(to, 0, w.data(), int32_t(wlen), nullptr, 0, nullptr, nullptr))) != 0) &&
                (out = static_cast<char*>(e->calloc_(1, wlen))) != nullptr) {
                const uint32_t r = uint32_t(e->wc2mb(to, 0, w.data(), int32_t(wlen), out, int32_t(wlen), nullptr, nullptr));
                if (!r) { e->free_(out); out = nullptr; }
                else if (n != 0xffffffffu) *len = r;
            }
        } else if (e->wc2mb(to, 0, w.data(), int32_t(wlen), dst, dstLen, nullptr, nullptr)) {
            out = dst;
        }
    }
    return out;
}

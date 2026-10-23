#include <cstdint>
#include <cstring>
#include <vector>

// BNClient send-composers; send(buf,len,id) is FUN_EB00, strings are NUL-terminated (ord 501 = strncpy-like).
// Decompile is register-mangled; layouts are the best reading (see ponytail note: unverified by vector).
typedef int (*d2_bnsend)(const uint8_t* buf, uint32_t len, uint32_t id);
static void put32(std::vector<uint8_t>& b, uint32_t v) { for (int i = 0; i < 4; ++i) b.push_back(uint8_t(v >> (8 * i))); }
static void puts0(std::vector<uint8_t>& b, const char* s) { b.insert(b.end(), s, s + strlen(s) + 1); }

// 1.13c BNClient+0x0bc70: id 0x26 = {u32 1, u32 n, u32 extra, str name, str a[n]}
bool d2_bnclient_bc70(const char* name, uint32_t n, const char* const* a, uint32_t extra, d2_bnsend send) {
    std::vector<uint8_t> b;
    put32(b, 1); put32(b, n); put32(b, extra);
    puts0(b, name);
    for (uint32_t i = 0; i < n; ++i) puts0(b, a[i]);
    return send(b.data(), (uint32_t)b.size(), 0x26) >= 0;
}

// 1.13c BNClient+0x0bd40: id 0x27 = {u32 1, u32 n, str name, str a[n], str b[n]}
bool d2_bnclient_bd40(const char* name, uint32_t n, const char* const* a, const char* const* c, d2_bnsend send) {
    std::vector<uint8_t> b;
    put32(b, 1); put32(b, n);
    puts0(b, name);
    for (uint32_t i = 0; i < n; ++i) puts0(b, a[i]);
    for (uint32_t i = 0; i < n; ++i) puts0(b, c[i]);
    return send(b.data(), (uint32_t)b.size(), 0x27) >= 0;
}

// 1.13c BNClient+0x0c520: id 0x0A = {str s1, str s2(<=0x100)}
bool d2_bnclient_c520(const char* s1, const char* s2, d2_bnsend send) {
    std::vector<uint8_t> b;
    puts0(b, s1);
    b.insert(b.end(), s2, s2 + strnlen(s2, 0x100)); b.push_back(0);
    return send(b.data(), (uint32_t)b.size(), 0x0A) >= 0;
}

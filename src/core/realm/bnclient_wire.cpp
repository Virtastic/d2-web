// src/core/realm/bnclient_wire.cpp -- the Battle.net client path's wasm-resident state. The gateway the
// player committed (BNGatewayAccess: count at +8, current index at +0xC), the gateway list buffer and the
// lobby's NewsTable array live here, and every access goes through the proven BNClient bodies
// (bnclient_gateway_access.cpp, bnclient_batch_1000.cpp, the CRT leaves), one copy each. The page's JS keeps the
// screens, the config file and the persisted region name; it asks these exports for the committed gateway and
// hands the news entries and the gateway list over.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include "bnclient_gateway_access.h"

typedef void (*BnSmemFreeFn)(uint32_t ptr, const char* file, uint32_t line, uint32_t flags);
typedef uint32_t (*BnSmemAllocFn)(uint32_t size, const char* file, uint32_t line, uint32_t flags);
typedef uint32_t (*BnSmemReAllocFn)(uint32_t ptr, uint32_t size, const char* file, uint32_t line, uint32_t flags);
struct BnCrtStream { uint32_t ptr; int32_t cnt; uint32_t base; uint32_t flag; };
void bn_zero16_1070(uint32_t* object);
void bn_news_table_dtor_10c0(uint32_t self, BnSmemFreeFn smem_free);
void bn_news_table_dtor_1110(uint32_t self, BnSmemFreeFn smem_free);
void bn_news_table_release_1160(uint32_t self, BnSmemFreeFn smem_free);
uint32_t bn_capacity_step_1180(uint32_t n, uint32_t* header);
extern "C" void bn_news_table_grow_11c0(uint32_t new_capacity, uint32_t* header, BnSmemReAllocFn smem_realloc,
                                        BnSmemAllocFn smem_alloc, BnSmemFreeFn smem_free);
char* bn_strncpy_1af0(char* dest, const char* src, size_t count);
int32_t bn_strnicmp(const char* str1, const char* str2, uint32_t max_count);
uint32_t bn_strtoul(const char* str, const char** end_ptr, int radix);
void bn_gateway_pick_closest_zone(uint32_t self, int32_t gmt, const BnetGatewayHost& host);
const char* bn_gateway_nth(uint32_t self, int32_t index, const BnetGatewayHost& host);
extern "C" void d2_bn_write_multi_char(BnCrtStream* f, uint8_t ch, int32_t count, int32_t* written,
                                       int32_t (*flsbuf)(int32_t, BnCrtStream*));

namespace {

uint32_t g_gateway[9];    // the live BNGatewayAccess image (36 bytes)
uint32_t g_gwlist[9];     // a second image holding the gateway list buffer (+8 count, +0xC current, +0x10 buffer, +0x14 bytes)
uint32_t g_news[4];       // the NewsTable array header (0x6FF3F8CC): [0] capacity, [1] count, [2] records, [3] growth step;
                          // a record is 0x408 bytes: +0 flag, +4 timestamp, +8 text (0x400)

// SMemFree seam: Storm's allocator is the heap here; the tag/line/flags arguments are retail's, unused.
void bn_free(uint32_t ptr, const char*, uint32_t, uint32_t) { std::free((void*)(uintptr_t)ptr); }
// SMemAlloc / SMemReAlloc seams: the array must stay addressable by retail's 32-bit slots.
uint32_t bn_alloc(uint32_t size, const char*, uint32_t, uint32_t) {
    void* p = std::malloc(size);
    if (p && (uintptr_t)p > 0xFFFFFFFFu) { std::free(p); return 0; }
    return (uint32_t)(uintptr_t)p;
}
uint32_t bn_realloc(uint32_t ptr, uint32_t size, const char*, uint32_t, uint32_t) {
    void* p = std::realloc((void*)(uintptr_t)ptr, size);
    if (p && (uintptr_t)p > 0xFFFFFFFFu) { std::free(p); return 0; }
    return (uint32_t)(uintptr_t)p;
}
int32_t bn_flsbuf_fail(int32_t, BnCrtStream*) { return -1; }
uint32_t bn_sstr_len(const char* s) { return (uint32_t)std::strlen(s); }
void bn_log_nothing(const char*, ...) {}
void bn_log_indent_nothing(int32_t, uint32_t) {}
BnetGatewayHost gw_host() {
    BnetGatewayHost h{};
    h.log_printf = bn_log_nothing;
    h.log_indent = bn_log_indent_nothing;
    h.sstr_len = bn_sstr_len;
    return h;
}

}  // namespace

// 1.13c BNClient+0x01020 -- retail's empty selection-changed hook (ret 0): the commit calls it where
// BNGatewayAccess would notify, and nothing happens.
void bn_noop_1020() {}

extern "C" {

// Commit the gateway: stage a zeroed image (0x1070), set count/current, then assign it over the live one (0x1050).
EMSCRIPTEN_KEEPALIVE void d2_bnet_gateway_commit(int32_t count, int32_t index) {
    uint32_t staged[9];
    std::memcpy(staged, g_gateway, sizeof staged);
    bn_zero16_1070(staged);
    staged[2] = (uint32_t)count;
    staged[3] = (uint32_t)index;
    d2_bnclient_gateway_access_assign(g_gateway, staged);
    bn_noop_1020();
}

EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_gateway_count() { return (int32_t)bn_gateway_num_1030(g_gateway); }
EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_gateway_current() { return (int32_t)bn_gateway_cur_1040(g_gateway); }

// Drop the held news (the NewsTable release step, 0x1160). Retail leaves the pointer, so it is cleared here.
EMSCRIPTEN_KEEPALIVE void d2_bnet_news_clear() {
    bn_news_table_release_1160((uint32_t)(uintptr_t)g_news, bn_free);
    std::memset(g_news, 0, sizeof g_news);
}

// One news entry, appended in timestamp order the way SID_NEWS_INFO's body (0x12C30) inserts: room for one more
// record comes from the capacity step (0x1180) and the array growth (0x11C0); the text is copied with the CRT's
// strncpy (0x1AF0), 0x400 bytes, zero padded. Returns the new count, -1 when the array cannot be grown.
EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_news_put(uint32_t ts, const char* text) {
    uint32_t n = g_news[1] + 1;
    if (n > g_news[0]) {
        uint32_t step = g_news[3] ? g_news[3] : bn_capacity_step_1180(n, g_news);
        bn_news_table_grow_11c0((n + step - 1) / step * step, g_news, bn_realloc, bn_alloc, bn_free);
        if (!g_news[2]) { std::memset(g_news, 0, sizeof g_news); return -1; }
    }
    uint8_t* rec = (uint8_t*)(uintptr_t)g_news[2] + (size_t)g_news[1] * 0x408u;
    std::memset(rec, 0, 8);
    std::memcpy(rec + 4, &ts, 4);
    bn_strncpy_1af0((char*)rec + 8, text, 0x400);
    g_news[1] = n;
    return (int32_t)n;
}

EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_news_count() { return (int32_t)(g_news[2] ? g_news[1] : 0); }

// End of the Battle.net session (realm flow dropped): the destructor body at 0x1110.
EMSCRIPTEN_KEEPALIVE void d2_bnet_news_destroy() {
    bn_news_table_dtor_1110((uint32_t)(uintptr_t)g_news, bn_free);
    std::memset(g_news, 0, sizeof g_news);
}

// A new login replaces the session: the destructor body at 0x10C0.
EMSCRIPTEN_KEEPALIVE void d2_bnet_news_reset() {
    bn_news_table_dtor_10c0((uint32_t)(uintptr_t)g_news, bn_free);
    std::memset(g_news, 0, sizeof g_news);
}

// The gateway list as retail's buffer: "<version>\0<selection>\0" then name, ZONE, display name per gateway, all NUL
// terminated (Nth, 0x14CC0, finds gateway i's name at string 3i-1). `blob` is the whole buffer, `count` the gateways.
EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_gateway_list(const char* blob, uint32_t bytes, int32_t count) {
    if (g_gwlist[4]) std::free((void*)(uintptr_t)g_gwlist[4]);
    std::memset(g_gwlist, 0, sizeof g_gwlist);
    void* buf = std::malloc((size_t)bytes + 1);
    if (!buf || (uintptr_t)buf > 0xFFFFFFFFu) { std::free(buf); return -1; }
    std::memcpy(buf, blob, bytes);
    ((char*)buf)[bytes] = 0;
    g_gwlist[2] = (uint32_t)count;
    g_gwlist[4] = (uint32_t)(uintptr_t)buf;
    g_gwlist[5] = bytes;
    return count;
}

// The gateway whose name matches, case-insensitively (BNClient's __strnicmp 0x165A, over the NUL too); 0-based, -1 if none.
EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_gateway_find(const char* name) {
    const BnetGatewayHost host = gw_host();
    const uint32_t len = (uint32_t)std::strlen(name) + 1;
    for (int32_t i = 1; i <= (int32_t)g_gwlist[2]; i++)
        if (bn_strnicmp(bn_gateway_nth((uint32_t)(uintptr_t)g_gwlist, i, host), name, len) == 0) return i - 1;
    return -1;
}

// PickClosestZone (0x15130): the gateway whose ZONE (read with strtol 0x1898) is nearest the host's minute bias;
// 0-based, -1 when there is no list.
EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_gateway_closest(int32_t bias_minutes) {
    if (!g_gwlist[4] || g_gwlist[2] == 0) return -1;
    bn_gateway_pick_closest_zone((uint32_t)(uintptr_t)g_gwlist, bias_minutes, gw_host());
    return (int32_t)g_gwlist[3] - 1;
}

// The list's version number: the buffer's first string through _strtoul (0x18AF), as GetGatewayList reads it.
EMSCRIPTEN_KEEPALIVE uint32_t d2_bnet_gateway_version() {
    const char* buf = (const char*)(uintptr_t)g_gwlist[4];
    return buf ? bn_strtoul(buf, nullptr, 10) : 0;
}

// `count` copies of `ch` written through the CRT's _write_multi_char (0x27F9, running _write_char 0x27C6) into a
// string stream over `out`; returns how many were written. The connecting popup's "%.*s" dots are this fill.
EMSCRIPTEN_KEEPALIVE int32_t d2_bnet_fill(uint8_t* out, int32_t ch, int32_t count) {
    if (count <= 0 || (uintptr_t)out > 0xFFFFFFFFu) return 0;
    BnCrtStream f{(uint32_t)(uintptr_t)out, count, (uint32_t)(uintptr_t)out, 0x40};
    int32_t written = 0;
    d2_bn_write_multi_char(&f, (uint8_t)ch, count, &written, bn_flsbuf_fail);
    return written;
}

}

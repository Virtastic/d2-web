// BNClient 0x128C0: publish the BNNews table (the entries from the first unexpired one) into the
// "BNNews-D2DV" cache record through 0x13A00. BNNews.cpp in the retail source paths.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// One table entry, 0x408 bytes: u32 flag, u32 timestamp, char text[0x400].
struct Bn128c0Entry {
    uint32_t flag;
    uint32_t ts;
    char text[0x400];
};

// The table globals: 0x6FF3F8D0 count, 0x6FF3F8D4 entries, 0x6FF3F8DC first kept index,
// 0x6FF3F8E0 newest-news stamp.
struct Bn128c0State {
    uint32_t count;
    Bn128c0Entry* entries;
    uint32_t base;
    uint32_t newest;
};

struct Bn128c0Env {
    void (*fatal)(uint32_t line);                      // Fog#10265 -> Fog#10024 -> _exit(-1)
    void* (*alloc)(uint32_t size, const char* file, uint32_t line);  // Fog#10042
    void (*free_)(void* p, const char* file, uint32_t line);         // Fog#10043
    void (*now_filetime)(uint32_t ft[2]);              // GetSystemTime + SystemTimeToFileTime
    // 0x13A00: ECX = name, EDX = data, stack (length, stamp*, expire1, expire2).
    int32_t (*cache_store)(const char* name, const void* data, uint32_t length, const uint32_t* stamp,
                           int32_t expire1, int32_t expire2);
    const char* cache_name;                            // *(char**)0x6FF3E0F0 = "BNNews-D2DV"
};

static const char kBnNewsFile[] = "..\\Source\\D2BNClient\\BNNews.cpp";

// 1.13c BNClient+0x128C0 (418 B), no arguments. Record: u32 newest, u32 n, then n x (u32 ts, text NUL).
// More than 0xFF entries past the base is fatal (line 0x13B); any text of 0x400+ chars, or a failed
// allocation, returns 0 without storing. The store's stamp is the current system FILETIME and both
// expiries are -1. Returns the store's result.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t bnclient_news_publish_128c0(const Bn128c0State* s, const Bn128c0Env* e) {
    uint32_t n = s->count - s->base;
    if (n > 0xFF) e->fatal(0x13B);
    uint32_t size = 8;
    for (uint32_t i = s->base; i < s->count; ++i) {
        uint32_t len = (uint32_t)std::strlen(s->entries[i].text);
        if ((int32_t)len > 0x3FF) return 0;
        size += len + 5;
    }
    uint8_t* buf = (uint8_t*)e->alloc(size, kBnNewsFile, 0x154);
    if (!buf) return 0;
    std::memcpy(buf, &s->newest, 4);
    std::memcpy(buf + 4, &n, 4);
    uint8_t* p = buf + 8;
    for (uint32_t i = s->base; i < s->count; ++i) {
        const Bn128c0Entry& en = s->entries[i];
        std::memcpy(p, &en.ts, 4);
        uint32_t len = (uint32_t)std::strlen(en.text);
        std::memcpy(p + 4, en.text, len + 1);
        p += len + 5;
    }
    uint32_t ft[2];
    e->now_filetime(ft);
    int32_t r = e->cache_store(e->cache_name, buf, size, ft, -1, -1);
    e->free_(buf, kBnNewsFile, 0x17B);
    return r;
}

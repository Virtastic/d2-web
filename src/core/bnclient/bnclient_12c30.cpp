// BNClient 0x12C30: the SID_NEWS_INFO (0x46) body, reached by the 0x11660 tail jump. Merges the
// received news entries into the BNNews table, then publishes it (0x128C0) and runs 0x12810.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct Bn12c30Entry {
    uint32_t flag;
    uint32_t ts;
    char text[0x400];
};

// 0x6FF3F8CC..0x6FF3F8E0: the growable array header [cap, count, entries, growth], then the first
// kept index and the newest stamp. 0x6FF3F63C: the message-of-the-day callback (ECX = 0, EDX = text).
struct Bn12c30State {
    uint32_t cap;
    uint32_t count;
    Bn12c30Entry* entries;
    uint32_t growth;
    uint32_t base;
    uint32_t newest;
    void (*motd)(uint32_t zero, const char* text);
};

struct Bn12c30Env {
    void (*fatal)(uint32_t line);                       // Fog#10265 -> Fog#10024 -> _exit(-1)
    uint32_t (*growth_step)(uint32_t n, Bn12c30State* hdr);  // 0x1180 (header = &state->cap)
    void (*reserve)(uint32_t new_cap, Bn12c30State* hdr);    // 0x11C0, updates cap/entries
    void (*publish)();                                  // 0x128C0
    void (*after)();                                    // 0x12810
};

// 1.13c BNClient+0x12C30 (handler body; ECX = payload, EDX = size, size unused).
// Payload: +0 u8 count (sign-extended), +1 u32 last logon, +5 u32 oldest, +9 u32 newest, then count
// entries {u32 ts, text NUL}. A count above 0x100 is fatal (line 0x77). An entry with ts 0 goes to the
// MOTD callback if one is set. Otherwise the table (kept sorted by ts) is searched from the top: an
// equal ts overwrites that entry's text (strncpy 0x400) and clears its flag; else the entry is
// inserted after the last smaller ts, growing the array to a multiple of the growth step first. Then
// newest is stored, base becomes the first index with ts >= oldest, publish and 0x12810 run. Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_news_info_12c30(const uint8_t* payload, uint32_t size,
                                                                  Bn12c30State* s, const Bn12c30Env* e) {
    (void)size;
    int32_t count = (int8_t)payload[0];
    if ((uint32_t)count > 0x100) e->fatal(0x77);
    uint32_t oldest, newest;
    std::memcpy(&oldest, payload + 5, 4);
    std::memcpy(&newest, payload + 9, 4);
    const uint8_t* p = payload + 0xD;
    for (int32_t k = 0; k < count; ++k) {
        uint32_t ts;
        std::memcpy(&ts, p, 4);
        const char* text = (const char*)(p + 4);
        p += 4 + std::strlen(text) + 1;
        if (ts == 0) {
            if (s->motd) s->motd(0, text);
            continue;
        }
        int32_t i = (int32_t)s->count - 1;
        while (i >= 0 && s->entries[i].ts > ts) --i;
        if (i >= 0 && s->entries[i].ts == ts) {
            s->entries[i].flag = 0;
            std::strncpy(s->entries[i].text, text, 0x400);
            continue;
        }
        uint32_t at = (uint32_t)(i + 1);
        uint32_t n = s->count + 1;
        if (n > s->cap) {
            uint32_t step = s->growth ? s->growth : e->growth_step(n, s);
            e->reserve((n + step - 1) / step * step, s);
        }
        s->count = n;
        for (uint32_t j = n - 1; j > at; --j) std::memcpy(&s->entries[j], &s->entries[j - 1], sizeof(Bn12c30Entry));
        s->entries[at].flag = 0;
        s->entries[at].ts = ts;
        std::strncpy(s->entries[at].text, text, 0x400);
    }
    s->newest = newest;
    uint32_t b = 0;
    while (b < s->count && s->entries[b].ts < oldest) ++b;
    s->base = b;
    e->publish();
    e->after();
    return 1;
}

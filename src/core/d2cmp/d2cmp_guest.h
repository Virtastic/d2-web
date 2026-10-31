// src/core/d2cmp/d2cmp_guest.h -- shared helpers for the D2CMP ports written against a flat guest
// image (`m` + 32-bit retail address), the style of d2cmp_1060.cpp. Register arguments of the retail
// routines are explicit parameters; Storm and out-of-file callees are D2CmpGuestEnv seams.
#pragma once
#include <cstdint>
#include <cstring>

namespace d2cmpg {
inline uint32_t rd(uint8_t* m, uint32_t a) { uint32_t v; memcpy(&v, m + a, 4); return v; }
inline void wr(uint8_t* m, uint32_t a, uint32_t v) { memcpy(m + a, &v, 4); }

// The Storm intrusive-list unlink retail inlines everywhere: link = {prev, next-or-~terminator}.
// If prev is set, the back slot is ~next (terminator) or link + (next - prev->next); the slot gets
// prev, prev->next gets next, both fields are zeroed. Field reads are fresh, as in retail.
inline void unlink(uint8_t* m, uint32_t link) {
    uint32_t p = rd(m, link);
    if (p == 0) return;
    uint32_t n = rd(m, link + 4);
    uint32_t slot = ((int32_t)n < 0) ? ~n : link + (n - rd(m, p + 4));
    wr(m, slot, p);
    wr(m, rd(m, link) + 4, rd(m, link + 4));
    wr(m, link, 0);
    wr(m, link + 4, 0);
}

// Retail tag string addresses (passed to Storm as the "file" argument).
constexpr uint32_t kTagCelEntry = 0x6fe32be8;  // "__AUTCELENTRY__"
constexpr uint32_t kTagCelList  = 0x6fe3417c;  // "__AUCELLIST__"
constexpr uint32_t kTagTsList   = 0x6fe34144;  // "__AV__TSExplicitList_UCELLIST__"
}

struct D2CmpGuestEnv {
    uint32_t (*alloc)(uint8_t* m, uint32_t size, uint32_t tag, int32_t line, uint32_t flags);                // Storm #401
    void (*free)(uint8_t* m, uint32_t p, uint32_t tag, int32_t line, uint32_t flags);                        // Storm #403
    uint32_t (*realloc)(uint8_t* m, uint32_t p, uint32_t size, uint32_t tag, int32_t line, uint32_t flags);  // Storm #405
    void (*f1490)(uint8_t* m, uint32_t list);                // D2CMP+0x1490 (list dtor)
    void (*f1880)(uint8_t* m, uint32_t self, int32_t mode);  // D2CMP+0x1880
    void (*f1f30)(uint8_t* m, uint32_t list);                // D2CMP+0x1F30 (EDI=list, drain)
    // [*(table)+4] thiscall(table, bucket, a2, a3): the table's node factory.
    uint32_t (*vnew)(uint8_t* m, uint32_t table, uint32_t bucket, uint32_t a2, uint32_t a3);
    uint32_t scratch;  // guest address of 12 bytes standing in for 0x2220's stack-local list
};

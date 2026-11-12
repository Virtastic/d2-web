// 1.13c D2Client+0x76790 (582 B, no args, bare ret): draws the party panel rows, at most 8, one per player after the local one.
// Globals: the row list 0x11BFD8 (next at +0xCD), the player list 0x11BC14 (next at +0x80, party id word at +0x22, id at +0x10),
// the local unit 0x11BBFC (id at +0xC).
// My party word = the head's party id when at least two players share it, else 0xFFFF. Per row (node = the next player):
//   sv = node's full ESI word (node address high half | party id) when two or more players share the party, else 0xFFFF;
//   mode = 0; with two or more sharing a party other than 0xFFFF: mode = 2 (and the leave button is live) when it is mine, else 8;
//   with a local unit: 9D630 (ECX = that share count, EDX = node id); a hit whose +0x34 list is set and whose node id is not mine
//   walks the list for my id: flag bit 8 there gives mode 1.
//   D3A8 (ECX 0), 75780(row+0xA0, y, mode, sv16 != 0xFFFF), 74ED0(EAX y+12, ECX row[+0xB9], mode), 758C0(EAX y+12, row[+0xBD], mode),
//   754A0(row, y, mode) when the leave button is live and row[+0xC1] is set else 761C0(row), 75690(EAX row[+0x18], ECX y, mine == sv16, sv),
//   D3A8 (ECX 1); y starts 0x5A, step 0x26.
// EAX: D3A8's last return when the row list runs out; the player list head when no row, no next player or the 8-row cap ends it.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c76790Host {
    uint32_t (*lookup)(uint32_t ecx, uint32_t edx);                       // 9D630
    uint32_t (*font)(uint32_t ecx);                                       // D3A8 (D2Win 10184)
    void (*label)(uint32_t p, int32_t y, uint32_t mode, uint32_t flag);   // 75780
    void (*b74ed0)(uint32_t eax, uint32_t ecx, uint32_t mode);            // 74ED0
    void (*b758c0)(uint32_t eax, uint32_t v, uint32_t mode);              // 758C0
    void (*leave)(uint32_t row, int32_t y, uint32_t mode);                // 754A0
    void (*idle)(uint32_t row);                                           // 761C0
    void (*name)(uint32_t eax, int32_t ecx, uint32_t eq, uint32_t sv);    // 75690
};

static inline uint32_t d2c_76790_rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint16_t d2c_76790_rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }

static uint32_t d2c_76790_share(uint32_t head, uint16_t pty) {
    uint32_t n = 0;
    for (uint32_t p = head; p; p = d2c_76790_rd32(p + 0x80)) if (d2c_76790_rd16(p + 0x22) == pty) ++n;
    return n;
}

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_76790(uint32_t rows, uint32_t players, uint32_t unit, const D2c76790Host* h) {
    uint16_t mine = 0xffff;
    if (players) { uint16_t t = d2c_76790_rd16(players + 0x22); if (d2c_76790_share(players, t) > 1) mine = t; }
    uint32_t node = players ? d2c_76790_rd32(players + 0x80) : 0;
    uint32_t eax = players;
    int32_t y = 0x5a;
    uint32_t i = 0;
    for (uint32_t row = rows; row; ) {
        eax = players;
        if (!node || i >= 8) return eax;
        const uint16_t pt = d2c_76790_rd16(node + 0x22);
        const uint32_t n = d2c_76790_share(players, pt);
        const uint32_t sv = n > 1 ? ((node & 0xffff0000u) | pt) : 0xffffu;
        uint32_t mode = 0; bool live = false;
        if (n > 1 && pt != 0xffff) { if (mine == (uint16_t)sv) { mode = 2; live = true; } else mode = 8; }
        if (unit) {
            const uint32_t me = d2c_76790_rd32(unit + 0xc), id = d2c_76790_rd32(node + 0x10);
            const uint32_t r = h->lookup(n, id);
            if (r) {
                const uint32_t lh = d2c_76790_rd32(r + 0x34);
                if (lh && id != me)
                    for (uint32_t q = d2c_76790_rd32(lh); q; q = d2c_76790_rd32(q + 8))
                        if (d2c_76790_rd32(q) == me) { if (d2c_76790_rd32(q + 4) & 8) mode = 1; break; }
            }
        }
        h->font(0);
        h->label(row + 0xa0, y, mode, (uint16_t)sv != 0xffff);
        h->b74ed0((uint32_t)(y + 0xc), d2c_76790_rd32(row + 0xb9), mode);
        h->b758c0((uint32_t)(y + 0xc), d2c_76790_rd32(row + 0xbd), mode);
        if (live && d2c_76790_rd32(row + 0xc1)) h->leave(row, y, mode); else h->idle(row);
        h->name(d2c_76790_rd32(row + 0x18), y, mine == (uint16_t)sv, sv);
        row = d2c_76790_rd32(row + 0xcd); y += 0x26; ++i; node = d2c_76790_rd32(node + 0x80);
        eax = h->font(1);
    }
    return eax;
}

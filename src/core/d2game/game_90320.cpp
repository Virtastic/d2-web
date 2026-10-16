// 1.13c D2Game+0x90320 (EBX = game, stack = char sel, int mode; ret 8): periodic sweep of the game's 0x44-byte slot table (game +0x1D24 = {count, base}).
// Written from the 1.13c decompile over the retail 32-bit record layout; every callee and GetTickCount is a seam call, ids as the capture's stubs.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#pragma pack(push, 1)
struct D2G90320Ent { uint32_t ref, f4; uint8_t pad08[0x14]; uint32_t one; uint8_t armedPre; uint8_t pad21; uint8_t sel, flag; uint8_t pad24[3]; uint8_t armed; uint32_t t; uint8_t pad2c[0x14]; uint32_t id; };
#pragma pack(pop)
static_assert(sizeof(D2G90320Ent) == 0x44, "slot is 0x44 bytes");
static_assert(offsetof(D2G90320Ent, f4) == 4 && offsetof(D2G90320Ent, one) == 0x1C && offsetof(D2G90320Ent, armedPre) == 0x20 && offsetof(D2G90320Ent, sel) == 0x22, "slot fields");
static_assert(offsetof(D2G90320Ent, flag) == 0x23 && offsetof(D2G90320Ent, armed) == 0x27 && offsetof(D2G90320Ent, t) == 0x28 && offsetof(D2G90320Ent, id) == 0x40, "slot fields");
struct D2G90320Unit { uint32_t id, ref, next; };   // unit +0xC, +4, +0xE4 (chain link, index + 1; 0 ends)
// game +0x1C = ordinal-10387 argument word, +0x1320 = 128 unit hash heads (index + 1), +0x1D24 = slot table; handles: game 0xA1, unit j 0xB1 + j, slot i 0xE0 + i.
struct D2G90320Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); };   // 1 tick, 2 90170(game, slot), 3 A448(g1c, 0), 4 1370(unit), 5 48390(r), 6 902E0(slot, game, unit)

static void GAME_90320(const D2G90320Seam& s, D2G90320Ent* ents, int32_t count, uint32_t g1c, int8_t sel, int32_t mode, const uint32_t* head, const D2G90320Unit* u)
{
    const uint32_t tick = s.call(1, nullptr);
    for (int32_t i = 0; i < count; ++i) {
        D2G90320Ent* e = &ents[i];
        if (e->sel != (uint8_t)sel || e->flag != 1 || e->one != 1 || e->armedPre != 1) continue;
        if (mode == 0) {
            if (tick > e->t + 240000u) { e->t = tick; e->armed = 1; }
            continue;
        }
        uint32_t j = head[e->id & 0x7F];
        while (j && u[j - 1].id != e->id) j = u[j - 1].next;   // first unit with this id; a ref mismatch does not look further
        if (j && u[j - 1].ref == e->ref) {
            const uint32_t h = 0xB1 + (j - 1), w1[1] = { h };
            const uint32_t r = s.call(4, w1), w2[1] = { r };
            if (s.call(5, w2)) e->armed = 1; else { const uint32_t w3[3] = { 0xE0u + i, 0xA1, h }; s.call(6, w3); }
            continue;
        }
        const uint32_t wa[2] = { 0xA1, 0xE0u + i }, wb[2] = { g1c, 0 };
        s.call(2, wa);
        e->armedPre = 0; e->t = 0;
        e->f4 = s.call(3, wb);
    }
}

// Replay hook for kind 3467 (s3271). set(i, v): D2Vec90320 words 0..14 (sel mode tick n nu uid[4] uf4[4] rMask rOrd), then ent[3][17] from 15; out(i): entO[51], nCalls, over, log[12][8].
static uint32_t g9In[66], g9Out[53 + 96], g9Ord, g9Cnt;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_90320_set(int i, unsigned v) { g9In[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_game_90320_out(int i) { return g9Out[i]; }
static uint32_t g9Call(uint32_t id, const uint32_t* w)
{
    // Capture h9 logs 0/2/2/1/1/3 argument words for ids 1..6;
    // its defaulted L arguments pad the rest, rather than reading beyond w.
    const uint32_t argc = id == 6 ? 3 : (id == 2 || id == 3 ? 2 : (id == 4 || id == 5 ? 1 : 0));
    if (g9Out[51] < 12) { uint32_t* q = &g9Out[53 + 8 * g9Out[51]]; q[0] = id; for (uint32_t k = 0; k < 3; k++) q[1 + k] = k < argc ? w[k] : 0; ++g9Out[51]; } else ++g9Out[52];
    switch (id) {
    case 1: return g9In[2];
    case 3: return g9In[14] + g9Ord++;
    case 4: return 0xC1;
    case 5: return (g9In[13] >> g9Cnt++) & 1;
    }
    return 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_90320_run()
{
    memset(g9Out, 0, sizeof g9Out); g9Ord = g9Cnt = 0;
    D2G90320Ent ents[3]; memcpy(ents, &g9In[15], sizeof ents);
    uint32_t head[128] = {}; D2G90320Unit u[4] = {};
    for (uint32_t j = 0; j < g9In[4]; ++j) {   // chains append in order, as the capture built them
        u[j] = { g9In[5 + j], g9In[9 + j], 0 };
        uint32_t* p = &head[u[j].id & 0x7F]; while (*p) p = &u[*p - 1].next; *p = j + 1;
    }
    GAME_90320({ g9Call }, ents, (int32_t)g9In[3], 0x5C1C0DE, (int8_t)g9In[0], (int32_t)g9In[1], head, u);
    memcpy(&g9Out[0], ents, sizeof ents);
}

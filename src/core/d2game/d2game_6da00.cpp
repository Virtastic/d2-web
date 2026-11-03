// 1.13c D2Game+0x6DA00 AI first-step entry (retail layout). ECX = the AI context {+0 game, +4 unit, +8 previous step, +0x10 unit
// id}. Returns 0 when the unit's AI block (+0x14) already has a step (+0xC) or the unit kind (+0x10) is not 0; else numbers the
// step (previous +0xC + 1, 0 without one), flags the unit (+0xC4 |= 1), runs the mode change, optionally replaces the unit's
// speech text (+0xA4) through the string lookup of "%d" over ai[+4] + 0xE63 (event 6 at frame + 0x12C), then runs the action
// node's handler (node byte 0 indexes the table of 12-byte entries below the count; a null entry or an index past it takes the
// default handler, a bad code pointer is fatal), the node's timer event (node +0x10: kind 5 at byte * 0x4B0 + frame + 1) and, for a
// monster, the record's delay event (kind 1 at (rec +0xDC >> 8) + frame + 1) unless record +0x109 is set or +0x141 clear.
// The callees and the two module globals ride in Env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2G6DA00Env {
    void     (*stateA)(uint32_t unit);                                           // 0xA4DE
    void     (*mode)(uint32_t unit, uint32_t m);                                 // 0xA4A2
    void     (*textFree)(uint32_t a, uint32_t b);                                // 0xAAF6 (game +0x1C, unit +0xA4)
    void     (*fmt)(char* buf, uint32_t v);                                      // wsprintfA(buf, "%d", v)
    uint32_t (*lookup)(uint32_t g1c, char* buf, uint32_t frame);                 // 0xAACC
    uint32_t (*node)(uint32_t unit);                                             // 0xB174
    uint32_t (*record)(uint32_t id);                                             // 0xA520
    void     (*event)(uint32_t game, uint32_t unit, uint32_t kind, uint32_t v);  // 0x8EA80
    int      (*badCode)(uint32_t fn);                                            // IsBadCodePtr
    void     (*call)(uint32_t fn, uint32_t ctx, uint32_t node);                  // table handler (ECX ctx, EDX node)
    void     (*dflt)(uint32_t ctx, uint32_t node);                               // 0x69C00
    void     (*fatal)(uint32_t idx);                                             // 0xAA3C path, never returns
    const uint32_t* table;                                                       // 0xFA5F8, 12-byte entries
    const uint32_t* count;                                                       // 0x107D4C
};

static inline uint32_t rd_6da00(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr_6da00(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_6DA00(uint32_t ctx, const D2G6DA00Env* e)
{
    const uint32_t game = rd_6da00(ctx), unit = rd_6da00(ctx + 4), prev = rd_6da00(ctx + 8);
    const uint32_t ai = rd_6da00(unit + 0x14);
    if (rd_6da00(ai + 0xC) || rd_6da00(unit + 0x10)) return 0;
    wr_6da00(ai + 0xC, prev ? rd_6da00(prev + 0xC) + 1 : 0);
    e->stateA(unit);
    wr_6da00(unit + 0xC4, rd_6da00(unit + 0xC4) | 1);
    e->mode(unit, 1);
    if (rd_6da00(ctx + 4) && rd_6da00(rd_6da00(ctx + 4) + 0xA4)) e->textFree(rd_6da00(game + 0x1C), rd_6da00(rd_6da00(ctx + 4) + 0xA4));
    char buf[20];
    e->fmt(buf, (uint16_t)(*(const uint8_t*)(uintptr_t)(rd_6da00(rd_6da00(ctx + 4) + 0x14) + 4) + 0xE63));
    const uint32_t txt = e->lookup(rd_6da00(game + 0x1C), buf, rd_6da00(game + 0xA8));
    if (txt) {
        wr_6da00(rd_6da00(ctx + 4) + 0xA4, txt);
        e->stateA(rd_6da00(ctx + 4));
        wr_6da00(rd_6da00(ctx + 4) + 0xC4, rd_6da00(rd_6da00(ctx + 4) + 0xC4) | 0x100);
        e->event(game, rd_6da00(ctx + 4), 6, rd_6da00(game + 0xA8) + 0x12C);
    }
    const uint32_t nd = e->node(unit);
    const uint32_t idx = *(const uint8_t*)(uintptr_t)nd;
    const uint32_t fn = idx < *e->count ? e->table[idx * 3] : 0;
    if (fn) {
        if (e->badCode(fn)) e->fatal(idx);
        e->call(fn, ctx, nd);
    } else e->dflt(ctx, nd);
    const uint32_t tb = *(const uint8_t*)(uintptr_t)(nd + 0x10);
    if (tb) e->event(rd_6da00(ctx), unit, 5, tb * 0x4B0 + rd_6da00(game + 0xA8) + 1);
    if (rd_6da00(unit + 0x10) == 1) {
        const uint32_t rc = e->record(rd_6da00(ctx + 0x10));
        if (!*(const uint8_t*)(uintptr_t)(rc + 0x109) && *(const uint8_t*)(uintptr_t)(rc + 0x141))
            e->event(rd_6da00(ctx), rd_6da00(ctx + 4), 1, (rd_6da00(rc + 0xDC) >> 8) + rd_6da00(game + 0xA8) + 1);
    }
    return 1;
}

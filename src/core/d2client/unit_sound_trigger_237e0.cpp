// 1.13c D2Client batch lane (implementation + marker only): the unit sound trigger at +0x237E0, from the
// cached decompile and the disassembly. D2Client has no D2MOO module. Callees are host seams; the
// register arguments (ESI/EAX/ECX/EDI) are explicit. Unit/record bytes are the serialized 32-bit layouts.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

struct D2c237Env {
    uint32_t (*mode223d0)(void*);            // ESI unit -> unit mode/state (edi)
    int32_t  (*type22430)(void*);            // ESI unit -> 1 monster, 2 player
    int32_t  (*class22490)(void*);           // ESI unit -> class id
    uint32_t (*get22040)(void*);             // EAX unit -> +0x80
    void     (*set22010)(void*, uint32_t);   // EAX unit, push value
    uint32_t (*get220a0)(void*);             // EAX unit -> +0x7c
    int32_t  (*sel224f0)(void*);             // ESI unit
    int32_t  (*sel22570)(void*, uint32_t);   // ESI unit, push mode
    int32_t  (*has225d0)(void*, int32_t);    // ECX unit, EAX category
    void     (*remove23070)(void*);          // EDI unit
    int32_t  (*count10522)(void*, uint32_t); // unit, DAT_6fbcbbfc
    int32_t  (*ord10331)(void*);
    int32_t  (*ord10057)(int32_t);
    int32_t  (*ord1700)(void*);              // EAX unit -> monster base id
    int32_t  monstatsCount;                  // *(sgptDataTables+0xa80)
    uint8_t* (*monSound2a70)(void*);         // EAX unit -> MonSounds row (+0x68 cat, +0x6c cooldown) or null
    int32_t  (*near881b0)(void);             // 1 when in range
    uint32_t (*rand39640)(uint32_t);         // ESI = n
    void     (*play38a70)(void*, int, int, int);
    uint32_t* now;                           // DAT_6fbc97f8
    uint32_t* lastPlay;                      // DAT_6fbcc4c0
    uint32_t* nextDelay;                     // DAT_6fbcc4c8
    uint32_t  bbfc;                          // DAT_6fbcbbfc
    void     (*fatal)(uint32_t);             // Ordinal_10265(0xc8f) -> 10024 -> _exit(-1)
};

static uint8_t rd8(const void* p, unsigned o) { return static_cast<const uint8_t*>(p)[o]; }
static uint32_t rd32(const void* p, unsigned o) { uint32_t v; std::memcpy(&v, static_cast<const uint8_t*>(p) + o, 4); return v; }
static void wr32(void* p, unsigned o, uint32_t v) { std::memcpy(static_cast<uint8_t*>(p) + o, &v, 4); }

// 1.13c D2Client+0x237E0 (unit on stack)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_237e0(uint8_t* u, const D2c237Env* e) {
    const uint32_t m = e->mode223d0(u);
    const int32_t kind = e->type22430(u);
    if (kind == 2) {
        if (e->class22490(u) == 0x1a && m == 0 && e->get22040(u) == 0 && e->count10522(u, e->bbfc) < 0x14) {
            e->play38a70(u, 0, 0, 0);
            e->set22010(u, *e->now);
        }
        if (!u) { e->fatal(0xc8f); std::abort(); }
        if (rd8(u, 0x70) && m == rd32(u, 0x74)) return;
        const int32_t sel = e->sel224f0(u);
        if (!sel) e->remove23070(u);
        else if (!e->has225d0(u, sel)) e->play38a70(u, 0, 0, 0);
        const bool alt = e->class22490(u) == 0x3b;
        if (rd8(u, 0x70) || alt) {
            if (e->sel22570(u, rd32(u, 0x74))) e->play38a70(u, 0, 0, 0);
        }
        u[0x70] = 1;
        wr32(u, 0x74, m);
        return;
    }
    if (kind != 1) return;
    int32_t id = e->ord1700(u);
    if (id < 0 || id >= e->monstatsCount) id = -1;
    const bool a = m == 1 || m == 2 || m == 0xf;
    if (!((id == 0x6e && m == 8) || (id == 0x88 && m == 0xb) || a)) return;
    const uint8_t* snd = e->monSound2a70(u);
    if (!snd) return;
    const uint8_t* snd2 = e->monSound2a70(u);
    const int32_t cat = snd2 ? int32_t(rd32(snd2, 0x68)) : 0;
    const uint32_t cool = rd32(snd, 0x6c), now = *e->now;
    if (!cat || e->has225d0(u, cat)) return;
    if (uint32_t(now - *e->lastPlay) < *e->nextDelay) return;
    if (uint32_t(now - e->get220a0(u)) < cool) return;
    if (uint32_t(now - e->get22040(u)) < cool) return;
    const int32_t o = e->ord10331(u);
    if (o && e->ord10057(o)) return;
    if (!e->near881b0() || e->rand39640(0xf)) return;
    e->play38a70(u, 0, 0, 0);
    *e->lastPlay = now;
    e->set22010(u, now);
    *e->nextDelay = e->rand39640(0x3d) + 0x1e;
}

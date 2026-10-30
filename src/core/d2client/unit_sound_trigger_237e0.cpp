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

// ---- The play path: 0x237E0 itself, its seams backed by the session. ----
// A handle stands for the client unit: its per-unit timers (+0x7C dwSfxTicks, +0x80 dwSfxAsyncTicks) live in g_idle by unit id.
// DAT_6fbc97f8 is the frame counter the shell passes; DAT_6fbcc4c0/c8 are g_idleLast/g_idleDelay. All start at 0 per session.
#include <unordered_map>
#include <cstddef>
#include <D2DataTbls.h>
extern "C" uint32_t d2_session_client_rand(uint32_t n);
extern "C" int d2_session_monster_idle_facts(uint32_t id, int32_t* town, int32_t* near);
static_assert(offsetof(D2MonSoundsTxt, dwNeutral) == 0x68 && offsetof(D2MonSoundsTxt, dwNeuTime) == 0x6c, "0x237E0 reads MonSounds +0x68/+0x6C");
namespace {
struct IdleUnit { uint32_t u7c = 0, u80 = 0; };
std::unordered_map<uint32_t, IdleUnit> g_idle;
uint32_t g_idleNow = 0, g_idleLast = 0, g_idleDelay = 0;
struct IdleCall { int32_t cls; uint32_t mode; int32_t town, near; IdleUnit* st; const D2MonSoundsTxt* snd; uint32_t row; };
IdleCall* C(void* u) { return static_cast<IdleCall*>(u); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_monster_idle_reset() { g_idle.clear(); g_idleNow = g_idleLast = g_idleDelay = 0; }
// 0x22070 / the inline stores beside DAT_6fbcc4c4 (0x23630 hit/death, 0x242B0 attack): a monster sound stamps the unit's +0x7C.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_monster_sfx_stamp(uint32_t id, uint32_t now) { g_idle[id].u7c = now; }
// One 0x4F4A0 -> 0x237E0 call for a client monster (class, 0x223D0 mode) at frame `now`; returns the Sounds row 0x88A70 submits, or 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_monster_idle(uint32_t id, int32_t cls, uint32_t mode, uint32_t now) {
    IdleCall c{cls, mode, 0, 0, &g_idle[id], DATATBLS_GetMonSoundsTxtRecordFromMonsterId(cls), 0};
    if (!d2_session_monster_idle_facts(id, &c.town, &c.near)) return 0;
    g_idleNow = now;
    D2c237Env e{};
    e.mode223d0 = [](void* u) { return C(u)->mode; };
    e.type22430 = [](void*) { return int32_t(1); };
    e.ord1700 = [](void* u) { const D2MonStatsTxt* r = DATATBLS_GetMonStatsTxtRecord(C(u)->cls); return r ? int32_t(r->nBaseId) : C(u)->cls; };
    e.monstatsCount = 0x7fffffff;
    e.monSound2a70 = [](void* u) { return reinterpret_cast<uint8_t*>(const_cast<D2MonSoundsTxt*>(C(u)->snd)); };
    e.has225d0 = [](void*, int32_t) { return int32_t(0); };   // ponytail: the unit's playing-sound list (+0x78) is not mirrored; NeuTime outlasts the row
    e.get220a0 = [](void* u) { return C(u)->st->u7c; };
    e.get22040 = [](void* u) { return C(u)->st->u80; };
    e.set22010 = [](void* u, uint32_t v) { C(u)->st->u80 = v; };
    e.ord10331 = [](void*) { return int32_t(1); };
    e.rand39640 = [](uint32_t n) { return d2_session_client_rand(n); };
    e.play38a70 = [](void* u, int, int, int) { C(u)->row = C(u)->snd->dwNeutral; };
    e.now = &g_idleNow; e.lastPlay = &g_idleLast; e.nextDelay = &g_idleDelay;
    e.fatal = [](uint32_t) {};
    // the room/town and range seams read the call's facts; the env seams take no unit, so they go through a file-scope pointer
    static IdleCall* cur; cur = &c;
    e.ord10057 = [](int32_t) { return cur->town; };
    e.near881b0 = [] { return cur->near; };
    d2_client_237e0(reinterpret_cast<uint8_t*>(&c), &e);
    return c.row;
}

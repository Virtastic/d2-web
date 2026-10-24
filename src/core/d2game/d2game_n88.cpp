// D2Game batch n88: faithful ports of register-ABI game/quest helpers. Callees outside this file are
// function-pointer seams (set by the host; null seams are skipped). Register args become parameters.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <Windows.h>

namespace n88 {
typedef uint8_t* P;
template <class T> static inline T& F(void* p, size_t o) { return *(T*)((P)p + o); }
using Fn = void* (*)(...);
// seams (name = callee RVA / ordinal)
struct Seams {
    void (*ord10015)();                       // Ordinal_10015
    uint32_t* (*getGameLocked_2AAE0)();       // 0x2AAE0
    void (*unlockGame_29BA0)();               // 0x29BA0
    void (*ord10050)();                       // game table lock
    void (*pump_FFB30)(uint32_t);             // 0xDFB30
    uint32_t (*ord10265)(uint32_t);
    void (*ord10024)(const char*, uint32_t);
    void* (*alloc10045)(const char*, int, int);
    uint32_t (*ord10042)(uint32_t, uint32_t);
    // 2C2D0 etc.
    void (*exit_)(int);
};
Seams g;
static void die(uint32_t code) { g.ord10265(code); g.ord10024("", code); (g.exit_ ? g.exit_ : (void(*)(int))exit)(-1); }
}  // namespace n88
using namespace n88;

extern uint8_t* g_gameTableSlots_6FD30C08[];  // not used directly; histogram takes a seam below
extern CRITICAL_SECTION* g_gameTableLock_6FD30BE8;

// 1.13c D2GAME+0x2AB70
// ESI = client id: returns locked game if its client list (game+0x88) holds that id, else unlocks.
uint32_t* D2Game_2AB70(int esiClientId)
{
    g.ord10015();
    uint32_t* game = g.getGameLocked_2AAE0();
    if (!game) return nullptr;
    for (int* c = (int*)(uintptr_t)game[0x22]; c; c = (int*)(uintptr_t)c[0x12a])
        if (*c == esiClientId) return game;
    g.unlockGame_29BA0();
    return nullptr;
}

// 1.13c D2GAME+0x2B350
// Walks the 128 per-bucket unit chains (game+0x1120, next at +0xE4) calling the pump up to `count` times.
int D2Game_2B350(uint32_t, uint32_t, int count)
{
    int n = 0;
    g.ord10050();
    LeaveCriticalSection(g_gameTableLock_6FD30BE8);
    uint32_t* game = g.getGameLocked_2AAE0();
    if (!game) return 0;
    uint32_t* slot = game + 0x448;
    for (uint32_t b = 0; n < count; ) {
        uint32_t u = *slot;
        while (u && n < count) {
            g.pump_FFB30(u); ++n;
            if (!u) die(0xdf);
            u = F<uint32_t>((void*)(uintptr_t)u, 0xe4);
        }
        ++b; ++slot;
        if (b > 0x7f) break;
    }
    if (!(CRITICAL_SECTION*)(uintptr_t)game[6]) die(0x100f);
    LeaveCriticalSection((CRITICAL_SECTION*)(uintptr_t)game[6]);
    return n;
}

// 1.13c D2GAME+0x2B840
// Histogram of game frame/7500 over the 1024-slot game table; slots[] = table at 0x6FD30C08.
void D2Game_2B840(uint16_t* out, uint32_t n, const int32_t* slots /*1024*/)
{
    memset(out, 0, (size_t)n * 2);
    for (int i = 0; i < 0x400; ++i) {
        if (slots[i] == 0 || slots[i] == -1) continue;
        uint32_t* game = g.getGameLocked_2AAE0();
        if (!game) continue;
        uint32_t frame = game[0x2a];
        if (!(CRITICAL_SECTION*)(uintptr_t)game[6]) die(0x100f);
        LeaveCriticalSection((CRITICAL_SECTION*)(uintptr_t)game[6]);
        int bin = (int)frame / 0x1d4c;
        if ((int)n <= bin) bin = (int)n - 1;
        out[bin]++;
    }
}

// 1.13c D2GAME+0x2C2D0
// Game creation body: needs the whole game-table/alloc stack; ported as an ordered seam script over
// the offsets it writes. ponytail: callee bodies live behind Seams2; null seam = skipped.
struct CreateSeams {
    void* (*allocGame)(uint32_t flags);       // 0x22060/0x22090 under table lock
    void* (*alloc10045)(const char*, int);
    uint16_t (*allocGameId_2A220)();
    void (*f4bc90)(); void (*f4a900)(); void (*fae630)(); void (*fd2560)();
    uint32_t (*cb34)();                       // DAT_6fd31c3c+0x34
    void* (*f6a9b0)(void*); void (*f69580)(); void (*fbbe40)(); void (*feacc0)();
    uint32_t (*f2b9a0)(void*); void (*fafbe0)(void*); void (*f548a0)(void*);
    void (*f4a2d0)(int, uint16_t); void (*f4ac60)();
    uint32_t (*ord10055)(); void (*f72390)(void*, int, int); void (*faa3e0)(void*, int);
    uint32_t cbPresent, cbHas8; uint8_t gameType;
};
CreateSeams g_cs;
void D2Game_2C2D0(uint32_t flags, uint8_t a6a, uint8_t a6b, uint8_t a6d)
{
    CreateSeams& s = g_cs;
    void* gm = s.allocGame ? s.allocGame(flags) : nullptr;
    if (!gm) return;
    uint8_t* b = (uint8_t*)gm;
    CRITICAL_SECTION* cs = (CRITICAL_SECTION*)s.alloc10045("..\\Source\\D2Game\\GAME\\Game.cpp", 0x4a6);
    F<CRITICAL_SECTION*>(b, 0x18) = cs; InitializeCriticalSection(cs);
    F<uint32_t>(b, 0x1c) = 0;
    memset(b + 0x1120, 0, 0x200); memset(b + 0x1320, 0, 0x200); memset(b + 0x1520, 0, 0x200);
    memset(b + 0x1720, 0, 0x200); memset(b + 0x1920, 0, 0x200); memset(b + 0x1b24, 0, 0x200);
    F<uint32_t>(b, 0x24) = 1;
    F<uint16_t>(b, 0x28) = s.allocGameId_2A220();
    s.f4bc90();
    b[0x6a] = a6a;
    for (int o = 0x90; o <= 0xa4; o += 4) F<uint32_t>(b, o) = 0;
    b[0x6d] = a6d; b[0x6b] = a6b;
    F<uint32_t>(b, 0x70) = (flags & 0x100000) ? 1 : 0;
    F<uint16_t>(b, 0x78) = (uint16_t)(((F<uint32_t>(b, 0x70) != 0) ? 99 : 0) + 2);
    F<uint32_t>(b, 0x74) = (flags >> 0x15) & 1;
    s.f4a900(); s.fae630(); s.fd2560();
    F<uint32_t>(b, 0x8c) = 0; F<uint32_t>(b, 0x88) = 0;
    if (s.cbPresent && s.cb34) F<uint32_t>(b, 0x20) = s.cb34();
    uint8_t* p = (uint8_t*)s.f6a9b0(b);
    if (b[0x6a] == 1) s.f69580();
    s.fbbe40(); s.feacc0();
    F<uint32_t>(b, 0x80) = s.f2b9a0(b);
    s.fafbe0(b); s.f548a0(b);
    s.f4a2d0(0, F<uint16_t>(b, 0x28)); s.f4ac60();
    if (!p) die(0x7d6);
    if (s.gameType >= 5) die(0x7d7);
    p[0x1ac] = s.gameType;
    uint32_t v = s.ord10055();
    F<uint32_t>(b, 0x1dc4) = v < 2 ? 2 : v;
    void* h = (void*)(uintptr_t)g.ord10042(0x524, 0);
    s.f72390(h, 0, 0);
    if (!s.cbPresent || !s.cbHas8) { s.faa3e0(nullptr, 1); return; }
    die(0x52b);
}

// 1.13c D2GAME+0x2CBE0
// Per-tick unit sweep over game+0xBC[5] room lists. Seams for the unit iterator/update calls.
struct SweepSeams {
    void* (*firstUnit_10717)(void* room);
    uint32_t (*ord10826)(void*); uint8_t* (*ord10014)(uint32_t); void (*f67820)(void*, uint32_t);
    void (*fd01430)(void*); void (*f400e0)(void*, void*); void (*f4f0d0)(void*, void*); void (*f67a20)(void*, void*);
    void (*ord10774)(void*, int); int (*ord10752)(void*);
};
SweepSeams g_ss;
void D2Game_2CBE0(void* game)
{
    if (!game) die(0xc0e);
    int* pr = (int*)((P)game + 0xbc);
    for (int k = 5; k; --k, ++pr) {
        uint8_t* room = (uint8_t*)(uintptr_t)*pr;
        if (!room || !F<uint32_t>(room, 0x54)) continue;
        for (void* u = g_ss.firstUnit_10717(room); u; u = (void*)(uintptr_t)F<uint32_t>(u, 0x7c)) {
            uint64_t v = (uint64_t)F<uint32_t>(u, 0x6c) * 0x6ac690c5ull + F<uint32_t>(u, 0x70);
            F<uint64_t>(u, 0x6c) = v;
            if ((v & 0x7fff) == 0 && F<uint32_t>(u, 0x78) == 0) {
                uint32_t t = g_ss.ord10826(u);
                uint8_t* rec = g_ss.ord10014(t);
                if (rec[0x2e] && (intptr_t)rec != -0x1c) g_ss.f67820(u, t);
            }
            if (!(F<uint8_t>(u, 0x34) & 1)) {
                g_ss.fd01430(game); g_ss.f400e0(game, u); g_ss.f4f0d0(game, u); g_ss.f67a20(game, u);
                F<uint32_t>(u, 0x34) |= 1;
                g_ss.ord10774(u, 1);
            } else if (g_ss.ord10752(u) == 0) {
                g_ss.f400e0(game, u);
                g_ss.ord10774(u, 1);
            }
        }
        F<uint32_t>(room, 0x54) = 0;
    }
}

// 1.13c D2Game+0x2D090
// ESI = client; stack (game, flag) ret 8. Builds the 0x28-byte 0x5A leave packet {5A, flag, 04, 0, ..., +8 char name,
// +0x18 account}, sends it to the other clients (CBDB0: eax = packet, game), removes the client (4B7F0: eax = id, game, 1),
// then kicks the connection: D2Net #10002 sends the 1-byte 0xB0, #10000 frees it. Null client: nothing.
struct ClientSeams {
    void (*sstrcopy)(void* dst, const void* src, int n);          // Storm #501 (bounded string copy)
    void (*sendLeave)(void* pkt, void* game);                     // 0xCBDB0
    void (*removeClient)(uint32_t id, void* game, int save);      // 0x4B7F0
    void (*netSend)(int, uint32_t, void*, int);                   // D2Net #10002
    void (*netFree)(uint32_t);                                    // D2Net #10000
};
ClientSeams g_cl;
void D2Game_2D090(void* client, void* game, uint32_t flag)
{
    if (!client) return;
    uint8_t pkt[0x28] = {0};
    uint32_t id = *(uint32_t*)client;
    uint8_t b0 = 0xb0;
    pkt[0] = 0x5a; pkt[1] = (uint8_t)flag; pkt[2] = 4;
    g_cl.sstrcopy(pkt + 0x18, (P)client + 0x1d, 0x32);
    g_cl.sstrcopy(pkt + 8, (P)client + 0xd, 0x10);
    pkt[0x27] = 0;
    g_cl.sendLeave(pkt, game);
    g_cl.removeClient(id, game, 1);
    g_cl.netSend(0, id, &b0, 1);
    g_cl.netFree(id);
}

// 1.13c D2GAME+0x2E050
// Per-frame game tick. Seams for the sub-steps; EAX = game.
struct TickSeams {
    void (*f4ca20)(void*); void (*f49be0)(); void (*f4cbe0)(void*); void (*faf070)(); void (*f4d9b0)(void*);
    void (*fbd850)(); void (*fbd5b0)(); void (*f53b70)(void*); void (*f4cf30)(void*);
    void (*ord10007)(uint32_t); void (*f4c0f0)(); void (*ord10042)(uint32_t, uint32_t);
};
TickSeams g_ts;
void D2Game_2E050(void* game)
{
    int mode = F<int>(game, 0x1dc8);
    int& fr = F<int>(game, 0xa8);
    ++fr;
    if (mode == 1) *(volatile int*)0 = 0;               // deliberate crash path, as retail
    else if (mode == 2) *(volatile int*)0xc0000001 = 0; // deliberate crash path, as retail
    else {
        if (mode == 3) die(0xdc7);
        if (mode == 4) g_ts.ord10042(0xdcb, 0);
    }
    g_ts.f4ca20(game); g_ts.f49be0(); g_ts.f4cbe0(game); g_ts.faf070(); g_ts.f4d9b0(game);
    g_ts.fbd850(); g_ts.fbd5b0();
    if (fr % 0x14 == 0) g_ts.f53b70(game);
    if (fr % 0xc == 0) g_ts.f4cf30(game);
    if (fr % 0xb == 0)
        for (int i = 0; i < 5; ++i) { uint32_t r = F<uint32_t>(game, 0xbc + 4 * i); if (r) g_ts.ord10007(r); }
    g_ts.f4c0f0();
}

// 1.13c D2GAME+0x33880
// EAX = player-guid list (u32[32] then u16 count at +0x80), ESI = guid: append if absent and room.
void QUESTS_AddPlayerGUID_33880(void* list, int guid)
{
    uint16_t cnt = F<uint16_t>(list, 0x80);
    if (cnt >= 0x20) return;
    for (int i = 0; i < cnt; ++i) if (F<int>(list, i * 4) == guid) return;
    F<int>(list, cnt * 4) = guid;
    F<uint16_t>(list, 0x80) = cnt + 1;
}

// 1.13c D2GAME+0x33F80
// EDI = quest data, ESI = guid: push onto singly linked list at +0x74 unless present. Returns 1 if added.
uint32_t QUESTS_AddToGuidList_33F80(void* q, int guid)
{
    uint32_t* node = (uint32_t*)(uintptr_t)F<uint32_t>(q, 0x74);
    if (!guid) die(0x5d);
    for (; node && node[0]; node = (uint32_t*)(uintptr_t)node[1])
        if ((int)node[0] == guid) return 0;
    node = (uint32_t*)g.alloc10045("..\\Source\\D2Game\\QUESTS\\QUESTS.CPP", 0x68, 0);
    node[0] = guid;
    node[1] = F<uint32_t>(q, 0x74);
    F<uint32_t>(q, 0x74) = (uint32_t)(uintptr_t)node;
    return 1;
}

// 1.13c D2GAME+0x34760
// EAX = quest data (+0x60 inventory), param1 = game. Walks inventory items, runs per-quest drop callback.
struct DropSeams {
    void* (*ord10460)(uint32_t); void* (*ord10464)(void*); uint32_t* (*ord11147)(void*);
    uint8_t* (*ord10695)(uint32_t); void (*f53ce0)(int, int); int (*isBadCode)(void*);
};
DropSeams g_ds;
void QUESTS_PlayerDroppedWithQuestItem_34760(void* q, void* game)
{
    void* it = g_ds.ord10460(F<uint32_t>(q, 0x60));
    for (;;) {
        uint8_t* rec;
        do {
            if (!it) { g_ds.f53ce0(1, 0); return; }
            void* next = g_ds.ord10464(it);
            uint32_t* x = g_ds.ord11147(it);
            rec = g_ds.ord10695(x ? x[1] : 0xffffffffu);
            it = next;
        } while (!rec || !rec[0x12a]);
        int idx = rec[0x12a] - 1;
        if (idx < 0) die(0x77);
        uint32_t* tbl = (uint32_t*)(uintptr_t)F<uint32_t>(game, 0x10f4);
        int* e = nullptr;
        if (tbl) for (int* p = (int*)(uintptr_t)tbl[0]; p; p = (int*)(uintptr_t)p[0x3d]) if (*p == idx) { e = p; break; }
        if (e && e[0x31]) {
            if (g_ds.isBadCode((void*)(uintptr_t)e[0x31])) die(0x2de);
            ((void (*)())(uintptr_t)e[0x31])();
        }
    }
}

// 1.13c D2GAME+0x366A0
// EAX = unit, ESI = this, param1 = arg: if 0x2E660(unit) then 0x365E0(this, arg).
void (*g_f365E0)(void*, int);
int (*g_f2E660)(int);
void AITHINK_Fn001_100_Idle_Buffy_366A0(int unit, void* self, int arg)
{
    if (g_f2E660(unit)) g_f365E0(self, arg);
}

// 1.13c D2GAME+0x38470
struct SpawnSeams {
    int (*ord10450)(uint32_t, int*); void* (*f58090)(int, int); void (*ord10977)(void*, int, int);
    void (*ord10058)(void*, uint8_t); int (*ord10792)(void*); void (*f583f0)();
    int (*ord10744)(void*, int); uint32_t (*ord11116)(void*, int); void (*ord10887)(void*, int, uint32_t, uint32_t);
    bool (*f57f20)(); bool (*f57ec0)(void*, int, int);
};
SpawnSeams g_sp;
void D2Game_38470(int game, int p2, uint32_t, uint32_t* p4)
{
    int tbl;
    if (!g_sp.ord10450(*p4, &tbl) || ((P)p4)[5] == 0) return;
    int i = 0;
    do {
        void* u = g_sp.f58090(game, game);
        g_sp.ord10977(u, 0x20000, 1);
        uint8_t* e = (uint8_t*)(uintptr_t)tbl;
        g_sp.ord10058(u, e[4]);
        if (g_sp.ord10792(u) == 0) {
            if (e[4] == 0) {
                g_sp.f583f0();
                if (g_sp.ord10744(u, 5) == 0) g_sp.ord10887(u, 0x48, g_sp.ord11116(u, 0), 0);
                else g_sp.ord10887(u, 0x46, 0xfa, 0);
                if (g_sp.ord10744(u, 5) != 0) g_sp.ord10887(u, 0x46, 100, 0);
            } else {
                if (!g_sp.f57f20()) g_sp.f583f0();
                if (g_sp.ord10744(u, 5) == 0) g_sp.ord10887(u, 0x48, g_sp.ord11116(u, 0), 0);
                else g_sp.ord10887(u, 0x46, 0xfa, 0);
            }
        } else if (!g_sp.f57ec0(u, game, 1)) g_sp.f583f0();
        ++i;
    } while (i < (int)((P)(uintptr_t)p2)[5]);
}

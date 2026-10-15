#include <D2Game/GAME/Game.h>

// 1.13c D2Game+0x2A210 (#10017): one store to the DLL's init-seed global (DAT 0x6FD2E354).
int32_t gnGameInitSeed;

void __stdcall GAME_SetInitSeed(int32_t nInitSeed)
{
    gnGameInitSeed = nInitSeed;
}

// Replay hook for s3208: set, then read the global back.
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_game_set_init_seed(int v) { GAME_SetInitSeed(v); return gnGameInitSeed; }

// 1.13c D2Game+0x2A900 fixed-seed branch (nInitSeed != -1), s3237 replay probe: field 0..4 = +7C, +84, +D0, +D4.
#include <D2Seed.h>
#include <cstring>
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_seedinit(uint32_t seed, int field)
{
    static D2GameStrc g;
    memset(&g, 0, sizeof g);
    g.InitSeed = 1;
    g.dwInitSeed = seed;
    SEED_InitSeed(&g.pGameSeed);
    SEED_InitLowSeed(&g.pGameSeed, seed);
    return field == 0 ? (uint32_t)g.dwInitSeed : field == 1 ? (uint32_t)g.InitSeed : field == 2 ? g.pGameSeed.nLowSeed : g.pGameSeed.nHighSeed;
}

// s3238 replay probe for 1.13c D2Game+0x8E630 EVENT_AllocEventQueue (kind 3252): B8==0 call, then B8-set call.
// field 0..6: b8 set after call 1, slab1, b8 set after call 2, b8 changed, slab2, queue2==queue1, nonzero bytes of queue 1 outside the slab head.
#include <D2Game/GAME/Event.h>
#include <cstring>
#include <vector>
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_eventqueue(int field)
{
    static D2GameStrc g;
    memset(&g, 0, sizeof g);
    EVENT_AllocEventQueue(&g);
    D2EventTimerQueueStrc* q1 = g.pTimerQueue;
    const bool slab1 = q1->pSlabListHead != nullptr;
    q1->pSlabListHead = nullptr;
    std::vector<unsigned char> c1((unsigned char*)q1, (unsigned char*)q1 + sizeof *q1);
    uint32_t nz = 0;
    for (unsigned char b : c1) nz += b != 0;
    q1->pSlabListHead = nullptr; // slab leaks (never freed); the probe only reads
    EVENT_AllocEventQueue(&g);
    D2EventTimerQueueStrc* q2 = g.pTimerQueue;
    const bool slab2 = q2->pSlabListHead != nullptr;
    q2->pSlabListHead = nullptr;
    const bool eq = memcmp(q2, c1.data(), c1.size()) == 0;
    const uint32_t r[] = { q1 != nullptr, slab1, q2 != nullptr, q2 != q1, slab2, eq, nz };
    return r[field];
}
// s3242 replay probe for 1.13c D2Game+0x8E420 EVENT_AllocTimerSlab (kind 3254): field 0..5 = nonNull, nonZeroWords, fnv, headOff, lastNextOff, secondNextOff.
// wasm64 pointers are 8 bytes, so the retail 32-bit image (12-dword timers, free-list link at dword 7, head at 0x1C20) is rebuilt from the struct;
// any byte of a timer other than its free-list link must be zero (else the image is poisoned and fnv differs).
#include <D2Game/GAME/Event.h>
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_timerslab(int field)
{
    static D2GameStrc g;
    memset(&g, 0, sizeof g);
    D2EventTimerSlabListStrc* slab = EVENT_AllocTimerSlab(&g);
    if (!slab) return field == 0 ? 0 : 0xFFFFFFFFu;
    static uint32_t w[0x1C22];
    memset(w, 0, sizeof w);
    for (int i = 0; i < 600; i++)
    {
        D2EventTimerStrc t = slab->tEventTimersStorage[i];
        D2EventTimerStrc* next = t.pNextFreeEventTimer;
        t.pNextFreeEventTimer = nullptr;
        static const D2EventTimerStrc zero = {};
        if (memcmp(&t, &zero, sizeof t)) w[i * 12] = 0xBAD00000u;
        if (next) w[i * 12 + 7] = (uint32_t)((next - slab->tEventTimersStorage) * 0x30);
    }
    w[0x1C20] = (uint32_t)((slab->pFreeEventTimerListHead - slab->tEventTimersStorage) * 0x30);
    if (slab->pNextSlab) w[0x1C21] = 0xBAD00000u;
    uint32_t nz = 0, fnv = 2166136261u;
    for (int i = 0; i < 0x1C22; i++) { nz += w[i] != 0; fnv = (fnv ^ w[i]) * 16777619u; }
    const uint32_t r[] = { 1, nz, fnv, w[0x1C20], w[599 * 12 + 7], w[7] };
    return r[field];
}

// s3244 replay probe for 1.13c D2Game+0x8E4D0 EVENT_FreeEventQueue (kind 3255): kase 0/1/2 = 1 slab / 3 slabs / queue already null;
// field 0..2 = chainBefore, queueAfter, poolSame.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_freeq(int kase, int field)
{
    static D2GameStrc g;
    memset(&g, 0, sizeof g);
    uint32_t chain = 0;
    if (kase < 2)
    {
        EVENT_AllocEventQueue(&g);
        D2EventTimerSlabListStrc* last = g.pTimerQueue->pSlabListHead;
        for (int i = 0; kase == 1 && i < 2; i++) { last->pNextSlab = EVENT_AllocTimerSlab(&g); last = last->pNextSlab; }
        for (D2EventTimerSlabListStrc* s = g.pTimerQueue->pSlabListHead; s; s = s->pNextSlab) chain++;
    }
    void* pool = g.pMemoryPool;
    EVENT_FreeEventQueue(&g);
    const uint32_t r[] = { chain, g.pTimerQueue != nullptr, g.pMemoryPool == pool };
    return r[field];
}

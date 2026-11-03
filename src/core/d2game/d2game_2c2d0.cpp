// 1.13c D2Game+0x2C2D0 GAME_CreateNewEmptyGame: take a free game record from the server's game pool and set it up.
// Convention: EAX = flags, 11 stack words a[0..10]; ret 0x2C; no result. a[0] the client-side id, a[1] the game name,
// a[2] / a[6] / a[7] bytes stored at +0x6A / +0x6B / +0x6D, a[3] a[4] a[8] go to the first client, a[5] a[9] a[10] unused.
// Game (retail layout, 32-bit words): +0x18 lock, +0x1C, +0x20 server cookie, +0x24, +0x28 game id word, +0x6A..+0x6D bytes,
// +0x70 / +0x74 flag bits, +0x78 word, +0x80, +0x88 / +0x8C client list and count, +0x90..+0xA4, six 0x200-byte tables
// (+0x1120, +0x1320, +0x1520, +0x1720, +0x1920, +0x1B24), +0x1DC4. Client: +0x1AC byte.
// The module globals (the server record 0x111C24, the callback table 0x111C3C, the byte 0x111C30) come through the env.
#include <cstdint>
#include <cstring>

struct D2Game2C2D0Env {
    const uint32_t* server;    // 0x111C24 (its +0x50 is the pool lock: Enter/LeaveCriticalSection)
    const uint32_t* callbacks; // 0x111C3C: +0x08 must be clear, +0x34 the cookie function
    const uint8_t* diff;       // 0x111C30
    uint32_t (*nextKey)(uint32_t server);                                      // 0x2060 (EDI server)
    uint32_t (*takeGame)(uint32_t server, uint32_t key);                       // 0x2090 (EDI server, stack key, 0, 0)
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);            // 0xA2E6 Fog alloc (ECX pool, EDX size, file, line, 0)
    uint32_t (*newGameId)();                                                   // 0x2A220
    void     (*setName)(uint32_t game, uint32_t name);                         // 0x2BC90 (EAX game, EDI name)
    void     (*initA)(uint32_t game);                                          // 0x2A900 (EDI game)
    void     (*initB)(uint32_t game);                                          // 0x8E630 (EBX game)
    void     (*initC)(uint32_t flags, uint32_t game, uint32_t a6);             // 0xB2560 (EAX flags, EDI game, stack a6)
    uint32_t (*cookie)(uint32_t fn);                                           // [table+0x34] (ECX 0)
    uint32_t (*addClient)(uint32_t a0, uint32_t game, uint32_t a3, uint32_t a4, uint32_t a8);   // 0x4A9B0 (EAX a0, 6 stack words)
    void     (*bind)(uint32_t a0, uint32_t key);                               // 0xA38E (stdcall 2)
    void     (*markClient)(uint32_t client);                                   // 0x49580 (EAX client)
    void     (*initD)(uint32_t game);                                          // 0x9BE40 (ESI game)
    void     (*initE)(uint32_t game);                                          // 0xCACC0 (ESI game)
    uint32_t (*initF)(uint32_t game);                                          // 0xB9A0 (stdcall 1)
    void     (*initG)(uint32_t game);                                          // 0x8FBE0 (stdcall 1)
    void     (*initH)(uint32_t game);                                          // 0x348A0 (stdcall 1)
    void     (*mapKey)(uint32_t key, uint32_t gameId);                         // 0x2A2D0 (stdcall 2)
    void     (*joinClient)(uint32_t game, uint32_t client);                    // 0x2AC60 (EAX game, ESI client)
    uint32_t (*players)();                                                     // 0xA2F2
    uint32_t (*alloc2)(uint32_t size, uint32_t line);                          // 0xA34C Fog alloc (ECX size, EDX file, line, 0)
    void     (*initI)(uint32_t p, uint32_t key);                               // 0x52390 (ECX p, EDX 0, stack key)
    void     (*send)(uint32_t client, const uint32_t* pkt, uint32_t n);        // 0x8A3E0 (EAX client, stack pkt, n): pkt is a[2]'s own slot
    void     (*fatal)(uint32_t line);
};

static uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
static void wr8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
static void wr16(uint32_t a, uint16_t v) { memcpy((void*)(uintptr_t)a, &v, 2); }

void D2Game_2C2D0(const D2Game2C2D0Env* e, uint32_t flags, const uint32_t* a) {
    const uint32_t srv = *e->server;
    if (!srv) return;
    const uint32_t key = e->nextKey(srv);
    const uint32_t g = e->takeGame(srv, key);
    if (!g) return;
    wr(g + 0x18, e->alloc(0, 0x18, 0x4A6));
    wr(g + 0x1C, 0);
    static const uint32_t kTables[6] = { 0x1120, 0x1320, 0x1520, 0x1720, 0x1920, 0x1B24 };
    for (uint32_t o : kTables) memset((void*)(uintptr_t)(g + o), 0, 0x200);
    wr(g + 0x24, 1);
    wr16(g + 0x28, (uint16_t)e->newGameId());
    e->setName(g, a[1]);
    wr8(g + 0x6A, (uint8_t)a[2]);
    for (uint32_t o = 0x90; o <= 0xA4; o += 4) wr(g + o, 0);
    wr8(g + 0x6D, (uint8_t)a[7]);
    wr8(g + 0x6B, (uint8_t)a[6]);
    wr(g + 0x70, (flags & 0x100000) ? 1 : 0);
    wr16(g + 0x78, rd(g + 0x70) ? 0x65 : 2);
    wr(g + 0x74, flags >> 21 & 1);
    e->initA(g);
    e->initB(g);
    e->initC(flags, g, a[6]);
    wr(g + 0x8C, 0);
    wr(g + 0x88, 0);
    if (const uint32_t t = *e->callbacks) if (const uint32_t fn = rd(t + 0x34)) wr(g + 0x20, e->cookie(fn));
    const uint32_t c = e->addClient(a[0], g, a[3], a[4], a[8]);
    e->bind(a[0], key);
    if (*(const uint8_t*)(uintptr_t)(g + 0x6A) == 1) e->markClient(c);
    e->initD(g);
    e->initE(g);
    wr(g + 0x80, e->initF(g));
    e->initG(g);
    e->initH(g);
    e->mapKey(key, rd(g + 0x28) & 0xFFFF);
    e->joinClient(g, c);
    if (!c) { e->fatal(0x7D6); return; }
    const uint8_t d = *e->diff;
    if (d >= 5) { e->fatal(0x7D7); return; }
    wr8(c + 0x1AC, d);
    const uint32_t n = e->players();
    wr(g + 0x1DC4, n > 1 ? n : 2);
    e->initI(e->alloc2(0x14, 0x524), key);
    if (const uint32_t t = *e->callbacks) if (rd(t + 8)) { e->fatal(0x52B); return; }
    // retail reuses a[2]'s stack slot as the one-byte packet: low byte 2, the upper bytes stay a[2]'s
    const uint32_t slot = (a[2] & 0xFFFFFF00u) | 2u;
    e->send(c, &slot, 1);
}

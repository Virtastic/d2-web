// n89 batch: STALLED d2game rows implemented from the decompile.
// Retail globals / Win32 / ordinals are reached through the N89Host function-pointer seam.
#include <cstdint>
typedef uint8_t* P;
static inline int32_t& I89(P p, int off) { return *(int32_t*)(p + off); }
static inline uint32_t& U89(P p, int off) { return *(uint32_t*)(p + off); }

struct N89Host {
    void (*fatal)(int msgId);                                  // Ordinal_10265 + Ordinal_10024 + _exit(-1)
    void (*mem_free)(void* p, const char* file, int line, int z); // Ordinal_10046
    P (*get_game)();                                           // 0x4AAE0 (locks, returns game or null)
    void (*leave_cs)(void* cs);                                // LeaveCriticalSection
    void (*enter_cs)(void* cs);                                // EnterCriticalSection
    uint32_t (*tick)();                                        // GetTickCount
    uint32_t (*frame)();                                       // Ordinal_10055
    void (*net_send)(int ch, uint32_t id, const void* b, int n); // Ordinal_10002
    void (*net_lock)();                                        // Ordinal_10050
    void (*proc_clients)(P game);                              // 0x4E050
    void (*free_client)(P game, int dummy);                    // 0x4D500
    void (*sweep)();                                           // 0x4CE20
    void (*game_tick)(P game, P msg);                          // 0x52160
    void (*remove_client)(P game, uint32_t clientId, int arg); // 0x4B7F0
    void (*list_unlink)(void* head);                           // 0x21E50
    void (*list_free)(void* head);                             // 0x21E10
    void (*free_ord)(int a, int b);                            // Ordinal_10043
    void (*send_pkt)(const void* b, int n);                    // 0x8A3E0
    void (*dispatch16b)(void* a);                              // 0x4E960
    void (*dispatch16c)();                                     // 0x4E900
    void (*obj_a)(void* z, int p1, P a, int b);                // 0x4E2A0
    void (*obj_b)(int p1, int esi, P a, int p2, int b);        // 0x4F970
    void (*obj_c)();                                           // 0x4E180
    void (*alias_4_3B20)();                                    // 0x54320
    P (*unit_by_guid)(int room, uint32_t guid);                // 0xE03A0
    // 0x4B7F0 collaborators
    P (*find_client)(uint32_t id);                             // 0x49FE0
    void (*cl_a)(P cl);                                        // 0x4AE90
    void (*cl_b)(P cl, P game);                                // 0x4C910
    void (*cl_c)(P game);                                      // 0x4BED0
    P (*get_player)(P cl, int ignoreDeath);                    // 0x4AD10
    int (*unit_stat)(P unit, int stat, int z);                 // Ordinal_10587
    void (*u_a)(); void (*u_b)(P game, P unit); void (*u_c)(P unit); void (*u_d)(P game);
    void (*u_e)();                                             // 0xBC840,0xB7640,0xB0560,0x54760,0x93AA0
    P (*unit_inv)(P unit);                                     // Ordinal_10331
    void (*inv_remove)(P inv, int type, int guid);             // Ordinal_10885
    void (*unit_free)(P pool, P unit);                         // 0xE20A0
    void (*unit_a)(P inv, int z);                              // Ordinal_10622
    void (*cb_free)();                                         // 0xBD550
    void (*name_hash)(const char* n, uint16_t* out);       // Ordinal_10137
    int (*name_cmp)(const char* a, const char* b, int n);      // Ordinal_509
    void (*callback)(void* cb, void** args);                   // DAT_6fd31c3c+4 (null-checked by caller)
    void* leaveCallback;                                       // DAT_6fd31c3c
    P* sessionTab;  // DAT_6fd307b8[256]
    P* nameTab;     // DAT_6fd303b8[256]
    void* csSess; void* csName; void* csQueue; void* csTab; // DAT_6fd30bb8, 30bd0, 31f58, 31c58 (stride 0x18)
    P sessionLock; int sessionOn;                              // DAT_6fd31c18
    P* gameTab;     // DAT_6fd30318[32]
    P queueHead;    // DAT_6fd30398
    int* queueCount;// DAT_6fd303ac
    P* clientTabSz; // DAT_6fd30c04 (current game)
};

// 1.13c D2GAME+0x38C70
int N89_38C70(N89Host* h, P* curGame, void* csGlobal, void* csTab)
{
    h->net_lock();
    h->leave_cs(csGlobal);
    P g = h->get_game();
    *curGame = g;
    if (!g) h->fatal(0x7b);
    int n = 0;
    for (int i = 0; i < 0x80; i++)
        for (uint32_t u = U89(g, 0x4c8 * 4 + i * 4); u; u = U89((P)(uintptr_t)u, 0xe4)) n++;
    void* cs = (void*)(uintptr_t)U89(g, 6 * 4);
    if (!cs) h->fatal(0x100f);
    h->leave_cs(cs);
    return n;
}

// 1.13c D2GAME+0x483B0
void N89_483B0(N89Host* h, int** pInteract)
{
    int it = **pInteract;
    while (it) { it = *(int*)(uintptr_t)(it + 8); h->mem_free(nullptr, "..\\Source\\D2Game\\MONSTER\\MonsterAI.cpp", 0x188, 0); }
    h->mem_free(nullptr, "..\\Source\\D2Game\\MONSTER\\MonsterAI.cpp", 0x18b, 0);
}

// 1.13c D2GAME+0x4EB50
void N89_4EB50(N89Host* h, P unit, void* arg)
{
    int t = unit ? I89(unit, 4) : -1;
    if (t == 0x16b) h->dispatch16b(arg);
    else if (t == 0x16c) h->dispatch16c();
}

// 1.13c D2GAME+0x50070
void N89_50070(N89Host* h, int p1, int p2, P eax, int ebx, int esi)
{
    h->obj_a(nullptr, p1, eax, ebx);
    h->obj_b(p1, esi, eax, p2, ebx);
    h->obj_c();
}

// 1.13c D2GAME+0x54400
void N89_54400(N89Host* h) { h->alias_4_3B20(); }

// 1.13c D2GAME+0x529F0
void N89_529F0(N89Host* h, uint32_t id, int* msg, void (*f526F0)(uint32_t, int*))
{
    if (msg[1] != 0) h->fatal(0x45f);
    f526F0(id, msg);
}

// 1.13c D2GAME+0x526F0
void N89_526F0(N89Host* h, uint32_t id, int* msg)
{
    P g = h->get_game();
    if (!g) return;
    h->proc_clients(g);
    for (uint32_t c = U89(g, 0x22 * 4); c;) { uint32_t nx = U89((P)(uintptr_t)c, 0x4a8); (void)nx; c = nx; h->free_client(g, 1); }
    uint32_t now = h->frame();
    bool direct = true;
    if (U89(g, 0x771 * 4) != 1 && now - U89(g, 0x771 * 4) < 0x709) {
        uint32_t tk = h->tick();
        if (U89(g, 0x23 * 4) == 0) {
            if (U89(g, 0x770 * 4) == 0) U89(g, 0x770 * 4) = tk;
            if (tk - U89(g, 0x770 * 4) > 300000 || (I89(g, 0x2a * 4) > 0x5dc && I89(g, 0x2a * 4) < 0x1d4c)) { direct = true; goto drain; }
        } else U89(g, 0x770 * 4) = 0;
        void* cs = (void*)(uintptr_t)U89(g, 6 * 4);
        if (!cs) h->fatal(0x100f);
        h->leave_cs(cs);
        h->enter_cs((P)h->csTab + (id & 0x1f) * 0x18);
        g = h->gameTab[id & 0x1f];
        if (!g) { h->leave_cs((P)h->csTab + (id & 0x1f) * 0x18); return; }
        h->game_tick(g, (P)msg);
        if (U89(g, 0) >= 0x20) h->fatal(0x96);
        h->leave_cs((P)h->csTab + U89(g, 0) * 0x18);
        return;
    }
    for (uint32_t c = U89(g, 0x22 * 4); c;) {
        uint32_t nx = U89((P)(uintptr_t)c, 0x12a * 4);
        uint8_t b[2] = { 0xb0, 0 };
        h->net_send(0, U89((P)(uintptr_t)c, 0), b, 1);
        h->net_send(1, U89((P)(uintptr_t)c, 0), b + 1, 1);
        h->remove_client(g, U89((P)(uintptr_t)c, 0), 0);
        c = nx;
    }
    { void* cs = (void*)(uintptr_t)U89(g, 6 * 4); if (!cs) h->fatal(0x100f); h->leave_cs(cs); }
drain:
    if (direct) {
        void* cs = (void*)(uintptr_t)U89(g, 6 * 4); (void)cs;
    }
    h->sweep();
    h->enter_cs(h->csQueue);
    P q = h->queueHead;
    P ent = h->gameTab[id & 0x1f];
    if (ent) {
        if (!q) h->fatal(0x2e9);
        P e = q - 0x10;
        if (I89(q, -0xc) == I89(ent, 4) + 1) {
            h->enter_cs((P)h->csTab + U89(e, 0) * 0x18);
            if (!q) h->fatal(0x2f2);
            h->list_unlink(q);
            h->leave_cs((P)h->csTab + U89(e, 0) * 0x18);
            h->enter_cs((P)h->csTab + U89(ent, 0) * 0x18);
            h->game_tick(ent, nullptr);
            h->leave_cs((P)h->csTab + U89(ent, 0) * 0x18);
            I89(q, -0xc) -= 1;
            h->list_unlink(q);
        } else { I89(ent, 4) -= 1; h->list_unlink(q); }
        h->list_free(q);
        h->free_ord(0x306, 0);
    }
    *h->queueCount -= 1;
    h->leave_cs(h->csQueue);
}

// 1.13c D2GAME+0x4B7F0
void N89_4B7F0(N89Host* h, uint32_t clientId, P game, int param2)
{
    if (!game) h->fatal(0x489);
    if (P c = h->find_client(clientId)) h->cl_a(c);
    if (param2 == 1) h->cl_b(h->find_client(clientId), game);
    char name[16] = {};
    if (h->sessionOn) {
        h->net_lock();
        P n = h->sessionTab[clientId & 0xff];
        if (n) {
            if (U89(n, 0) == clientId) {
                for (int i = 0; i < 16; i++) { name[i] = n[0xd + i]; if (!n[0xd + i]) break; }
                h->sessionTab[clientId & 0xff] = (P)(uintptr_t)U89(n, 299 * 4);
            } else {
                P prev = n;
                while (U89((P)(uintptr_t)U89(prev, 299 * 4), 0) != clientId) prev = (P)(uintptr_t)U89(prev, 299 * 4);
                P cur = (P)(uintptr_t)U89(prev, 299 * 4);
                if (!cur) h->fatal(0x4db);
                for (int i = 0; i < 16; i++) { name[i] = cur[0xd + i]; if (!cur[0xd + i]) break; }
                U89(prev, 299 * 4) = U89(cur, 0x4ac);
            }
        }
        h->leave_cs(h->csSess);
    }
    // unlink from game client list (+0x88, next +0x4A8)
    P found = nullptr;
    P head = (P)(uintptr_t)U89(game, 0x88);
    if (head) {
        if (U89(head, 0) == clientId) { U89(game, 0x88) = U89(head, 0x4a8); found = head; }
        else {
            P p = head;
            while (U89(p, 0x4a8) && U89((P)(uintptr_t)U89(p, 0x4a8), 0) != clientId) p = (P)(uintptr_t)U89(p, 0x4a8);
            P cur = (P)(uintptr_t)U89(p, 0x4a8);
            if (cur) { U89(p, 0x4a8) = U89(cur, 0x4a8); found = cur; }
        }
    }
    if (h->sessionOn) {
        uint16_t hs; h->name_hash(name, &hs);
        h->net_lock();
        uint32_t k = hs & 0xff;
        P b = h->nameTab[k];
        if (b) {
            if (h->name_cmp((char*)b + 0xd, name, 0x10) == 0) h->nameTab[k] = (P)(uintptr_t)U89(b, 0x4b0);
            else {
                if (!U89(b, 0x4b0)) h->fatal(0x512);
                while (h->name_cmp((char*)(uintptr_t)U89(b, 0x4b0) + 0xd, name, 0x10) != 0) {
                    b = (P)(uintptr_t)U89(b, 0x4b0);
                    if (!U89(b, 0x4b0)) break;
                }
                if (U89(b, 0x4b0)) U89(b, 0x4b0) = U89((P)(uintptr_t)U89(b, 0x4b0), 0x4b0);
            }
        }
        h->leave_cs(h->csName);
    }
    if (!found) return;
    h->cl_c(game);
    P pl = h->get_player(nullptr, 0);
    if (!pl) {
        if (h->leaveCallback) { found[0x1b * 4] = 0; h->callback(h->leaveCallback, nullptr); }
    } else {
        h->u_a(); h->u_b(game, pl); h->u_c(pl); h->u_d(game); h->u_e();
        P inv = h->unit_inv(pl);
        if (inv) h->inv_remove(inv, I89(pl, 0), I89(pl, 0xc));
        h->unit_free((P)(uintptr_t)U89(found, 0x6a * 4), pl);
        h->unit_a(inv, 0);
        h->free_ord((int)(uintptr_t)inv, 0);
        if (h->leaveCallback) h->callback(h->leaveCallback, nullptr);
    }
    if (U89(found, 0x6d * 4)) h->cb_free();
    if (U89(found, 0x5f * 4)) { h->mem_free((void*)(uintptr_t)U89(found, 0x5f * 4), "..\\Source\\D2Game\\GAME\\Clients.cpp", 0x5e1, 0); U89(found, 0x5f * 4) = 0; }
    while (U89(found, 0x70 * 4)) {
        void* x = (void*)(uintptr_t)U89(found, 0x70 * 4);
        U89(found, 0x70 * 4) = U89((P)x, 0x204);
        h->mem_free(x, "..\\Source\\D2Game\\GAME\\Clients.cpp", 0x5f1, 0);
    }
    h->mem_free(found, "..\\Source\\D2Game\\GAME\\Clients.cpp", 0x5f4, 0);
    I89(game, 0x8c) -= 1;
}

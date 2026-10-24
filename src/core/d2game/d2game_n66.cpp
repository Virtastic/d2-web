// n66 batch: STALLED d2game rows implemented from the decompile.
// Retail globals / Win32 / ordinals become a host seam struct (function pointers).
// Struct accesses use the retail 32-bit byte offsets on a flat buffer view.
#include <cstdint>
typedef uint8_t* P;
static inline int32_t& I(P p, int off) { return *(int32_t*)(p + off); }

struct N66Host {
    void (*mem_free)(void* p, const char* file, int line, int zero);  // Ordinal_10046
    void (*mem_free_named)(void* p, const char* name, int a, int b);  // Ordinal_403
    void (*unwind)(void* frame, void* target, void* rec, void* ret);  // RtlUnwind
    void (*unlock_fhandle)(int fh);                                   // __unlock_fhandle
    void (*fatal)(const char* msg, int code);                         // Ordinal_10024+_exit
    int32_t dataTablesRoot;                                           // *sgptDataTables_exref
    // unit/item seams
    P (*first_room)(P ptr);                                           // Ordinal_10717
    P (*unit_get_inv)(P unit);                                        // Ordinal_10331
    void (*inv_remove)(P inv, int type, int guid);                    // Ordinal_10885
    void (*unit_a)(P unit);                                           // Ordinal_10408
    void (*unit_b)(P unit);                                           // Ordinal_11016
    void (*unit_unlink)(P unit);                                      // 0xDFC10
    void (*unit_free)(P game, P unit);                                // 0xE20A0
    void (*set_string)(int a, int b);                                 // Ordinal_10786
    P (*lookup)();                                                    // 0xA00B0
    void (*apply)(int a, P b, int idx, int ebx);                      // 0x448E0
    // VirtualQuery family
    int (*reset_stack)();
};

// 1.13c D2GAME+0x01000
void sub_1000(N66Host* h, void* p) { if (p) h->mem_free_named(p, "delete", -1, 0); }

// see 1.13c D2GAME+0x077F0
void __global_unwind2(N66Host* h, void* frame) { h->unwind(frame, (void*)0x6fc27808, 0, 0); }

// 1.13c D2GAME+0x0870C
void ARENA_FreeArena(N66Host* h, int locked, int fh) { if (locked) h->unlock_fhandle(fh); }

// 1.13c D2GAME+0x0B8A0
void OBJRGN_FreeObjectControl(N66Host* h, P game)
{
    static const char* F = "..\\Source\\D2Game\\OBJECTS\\objrgn.cpp";
    if (!game) { h->fatal("objrgn", -1); return; }
    P c = (P)(intptr_t)I(game, 0x10f0);
    if (!c) return;
    for (P n = (P)(intptr_t)I(c, 0x1110); n; n = (P)(intptr_t)I(n, 0xc))
        h->mem_free(n, F, 0xc1, 0);
    I(c, 0x1110) = 0;
    P dt = (P)(intptr_t)h->dataTablesRoot;
    for (int i = 1; i < I(dt, 0xc5c); i++) h->mem_free(0, F, 0xe2, 0);
    for (int i = 0; i < 8; i++) if (I(c, 0x28 + i * 4)) h->mem_free((void*)(intptr_t)I(c, 0x28 + i * 4), F, 0xe8, 0);
    h->mem_free(c, F, 0xec, 0);
}

// 1.13c D2GAME+0x0C6B0
void sub_C6B0(N66Host* h, int a, int idx, int ebx)
{
    P dt = (P)(intptr_t)h->dataTablesRoot;
    if (idx < 0 || idx >= I(dt, 0xba0)) return;
    P row = (P)(intptr_t)I(dt, 0xb98) + idx * 0x23c;
    if (!row) return;
    h->set_string(a, I(row, 0x60));
    P r = h->lookup();
    if (r) h->apply(a, r, idx, ebx);
}

// 1.13c D2GAME+0x0FB70
void ITEMS_DeleteInactiveItems(N66Host* h, P game, int slot, int frame)
{
    P head = (P)(intptr_t)I(game, 0xbc + slot * 4);
    if (!head) return;
    for (P rm = h->first_room(head); rm; rm = (P)(intptr_t)I(rm, 0x7c))
        for (P u = (P)(intptr_t)I(rm, 0x74); u;) {
            P next = (P)(intptr_t)I(u, 0xe8);
            P d = (P)(intptr_t)I(u, 0x14);
            if (I(u, 0) == 4 && I(d, 0x24) && I(d, 0x24) <= frame) {
                P inv = h->unit_get_inv(u);
                if (inv) { h->inv_remove(inv, I(u, 0), I(u, 0xc)); h->unit_a(u); h->unit_b(u); }
                I(u, 0xc4) &= ~2;
                h->unit_unlink(u);
                h->unit_free(game, u);
            }
            u = next;
        }
}

// 1.13c D2GAME+0x1EB20
void SUNITINACTIVE_DeleteExpiredItemNodes(N66Host* h, P game, int slot)
{
    for (P n = (P)(intptr_t)I(game, 0xd8 + slot * 4); n; n = (P)(intptr_t)I(n, 0x14)) {
        int32_t* prev = 0;
        int32_t* cur = (int32_t*)(intptr_t)I(n, 8);
        while (cur) {
            int32_t* nx = (int32_t*)(intptr_t)cur[0];
            if (cur[1] && cur[1] < I(game, 0xa8)) {
                if (prev) prev[0] = (int32_t)(intptr_t)nx; else I(n, 8) = (int32_t)(intptr_t)nx;
                h->mem_free(cur, "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp", 0x724, 0);
            } else prev = cur;
            cur = nx;
        }
    }
}

// see 1.13c D2GAME+0x09A45
int __resetstkoflw(N66Host* h) { return h->reset_stack(); }

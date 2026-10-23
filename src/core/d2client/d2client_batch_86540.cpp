// src/core/d2client/d2client_batch_86540.cpp -- 1.13c D2Client+0x86540 (599 B), alpha batch lane: implementation + marker only.
// Unrecovered callees (ordinals, other RVAs, unaff_ register args) are host seams (d2client_batch_869b0 pattern).
#include <cstdint>
#include <emscripten.h>

struct D2c86540Env {
    void     (*ord11105)(const uint8_t* p, int32_t len, int32_t z, uint8_t* out8);  // out8 = local_18 block
    uint32_t dbg_active;                                                            // DAT_6fbcbbfc
    int32_t* (*fn54e20)(uint32_t id, uint32_t tblEntry, uint32_t idx);              // returns 32-bit-layout record, [0x18] = handle
    int32_t* (*bucket)(uint32_t slot);                                              // DAT_6fbbae08[slot] head; next at [0x39]
    void     (*fatal)(void);                                                        // Ordinal_10265+10024+_exit(-1)
    int      (*ord10197)(uint32_t v, uint32_t mask);
    void     (*fn32980)(int a, uint32_t b, int c);
    void     (*fn32490)(void);
    int32_t* (*fn333f0)(const uint8_t* pkt, uint32_t* o28);
    void     (*ord10058)(int32_t* u, uint32_t v);
    int      (*ord10405)(int32_t h, int32_t* u, uint32_t mode);
    int      (*fn31b40)(void);
    void     (*fn193c0)(int32_t* rec);
    void     (*ord10138)(const uint8_t* p, int32_t esi);
    void     (*fn18cb0)(int a);
    void     (*fn19a90)(int32_t* rec);
    void     (*fn1a220)(void);
    void     (*fn31b70)(void);
    void     (*ord10466)(const uint8_t* p, int a);
    void     (*ord11017)(const uint8_t* p);
    void     (*fn62d0)(void);
    void     (*fn31ff0)(void);
    void     (*fn32fe0)(int32_t* rec);
    void     (*fn6e2b0)(void);
    void     (*fn31aa0)(void);
    void     (*fn7ca0)(int32_t v);
};

// 1.13c D2Client+0x86540
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_86540(const uint8_t* pkt, uint32_t ebp, int32_t esi, uint32_t stk1c, uint8_t b20, uint8_t b23, const D2c86540Env* e) {
    uint8_t loc[8] = {0};                       // local_18: [3]=cStack_17 kind, [4..]=uStack_16/14
    uint32_t u28 = 0;
    const uint8_t* body = pkt + 0xd;
    e->ord11105(body, (int32_t)pkt[2] - 0xd, 0, loc);
    if (!e->dbg_active) return;
    uint32_t idx = pkt[8], id4 = *(const uint32_t*)(pkt + 4);
    int32_t* rec = e->fn54e20(*(const uint32_t*)(pkt + 9), idx * 0x80, idx);
    if (!rec) return;
    int32_t* u = nullptr;
    for (int32_t* p = e->bucket(id4 & 0x7f); p; p = (int32_t*)(uintptr_t)(uint32_t)p[0x39])
        if ((uint32_t)p[3] == id4) { if (*p != 4) e->fatal(); u = p; break; }
    int32_t* u2;
    if (u) {
        if (!e->ord10197(stk1c, 1)) { e->fn32980(0, b20, 0); u2 = u; goto L; }
        e->fn32490();
    }
    u = e->fn333f0(pkt, &u28); u2 = u;
L:
    if (u) e->ord10058(u, *(const uint32_t*)(loc + 1));
    if (rec[0x18]) {
        uint32_t mode = (loc[1] == 0xb || loc[1] == 0xc) ? 4 : 3;
        if (e->ord10405(rec[0x18], u, mode) && e->fn31b40()) {
            e->fn193c0(rec);
            if (ebp == 3) e->ord10138(body, esi);
            e->fn18cb0(0);
            if (rec[0] >= 0 && rec[0] < 2) {
                uint8_t c = *(const uint8_t*)(uintptr_t)(*(const uint32_t*)(loc + 1) + 3);
                if (c == 0 || c == 1) e->fn19a90(rec);
                e->fn1a220();
                if (rec == u2) {
                    e->fn31b70();
                    if (!e->ord10197(u28, 8)) { e->ord10466(body, 0); e->ord11017(body); e->fn62d0(); }
                }
                if (b23 < 0xb) e->fn31ff0();
                e->fn32fe0(rec); e->fn32fe0(rec);
                e->fn6e2b0(); e->fn31aa0();
                e->fn7ca0(esi ? *(int32_t*)(uintptr_t)(esi + 0xc) : -1);
            }
        }
    }
}

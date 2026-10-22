// src/core/realm/bnclient_batch_6677.cpp -- 1.13c BNClient.dll row 0x6677 (alpha lane, no spec/vector), from
// build-wasm/ghidra-bnclient/decomp/0x6677.c. MSVC CRT __setmbcp: resolves the -2/-3/-4 pseudo code pages, and
// if the page changed, runs sub_64E3 (bn_setmbcp) and snapshots the old state into a 0x220-byte block.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct BnSetMbcpState;
struct BnSetMbcpEnv;
extern "C" int32_t bn_setmbcp(uint32_t cp, uint8_t* ctype, BnSetMbcpState* st, const BnSetMbcpEnv* env);

struct BnSetMbcpState {  // must match bnclient_batch_64e3.cpp
    uint32_t cp, mb_flag, mb_aux, extra[3];
};

struct BnMbcpWorld {
    BnSetMbcpState* st;       // DAT_3FA84 / 3F960 / 3F958
    uint8_t* ctype;           // 257 B at 0x3F980
    uint16_t* aux5;           // 5 u16 at 0x3FA90
    uint8_t* tbl256;          // 256 B at 0x3FAA0
    uint8_t** saved;          // DAT_3F95C (block of 0x220 B or null)
    uint32_t* crt_has_default;// DAT_3E65C
    uint32_t default_cp;      // DAT_3E6A8
    const BnSetMbcpEnv* env;  // forwarded to bn_setmbcp
    uint32_t (*oemcp)();      // GetOEMCP
    uint32_t (*acp)();        // GetACP
    void* (*alloc)(uint32_t); // _malloc
    void (*release)(void*);   // _free
    void (*lock)(int);        // __lock(0xd)
    void (*unlock)();         // FUN_67be
};

// 1.13c BNClient+0x06677 (327 B). Returns 0, or -1 on failure.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_setmbcp_wrap(int32_t cp, const BnMbcpWorld* w) {
    int32_t rc = -1;
    w->lock(0xd);
    *w->crt_has_default = 0;
    if (cp == -2) { *w->crt_has_default = 1; cp = (int32_t)w->oemcp(); }
    else if (cp == -3) { *w->crt_has_default = 1; cp = (int32_t)w->acp(); }
    else if (cp == -4) { *w->crt_has_default = 1; cp = (int32_t)w->default_cp; }
    if ((uint32_t)cp == w->st->cp) {
        rc = 0;
    } else {
        uint8_t* blk = *w->saved;
        if (!blk || *(int32_t*)blk != 0) blk = (uint8_t*)w->alloc(0x220);
        if (blk && (rc = bn_setmbcp((uint32_t)cp, w->ctype, w->st, w->env)) == 0) {
            memset(blk, 0, 4);
            memcpy(blk + 4, &w->st->cp, 4);
            memcpy(blk + 8, &w->st->mb_flag, 4);
            memcpy(blk + 12, &w->st->mb_aux, 4);
            memcpy(blk + 0x10, w->aux5, 10);
            memcpy(blk + 0x1c, w->ctype, 0x101);
            *w->saved = blk;
            memcpy(blk + 0x11d, w->tbl256, 0x100);
        }
        if (rc == -1 && blk != *w->saved) w->release(blk);
    }
    w->unlock();
    return rc;
}

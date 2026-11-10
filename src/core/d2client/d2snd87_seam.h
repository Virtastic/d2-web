// src/core/d2client/d2snd87_seam.h -- the D2Client 0x87DE0..0x888B0 sound list state and callee seam (shared with its probes).
#pragma once
#include <cstdint>

struct D2cSnd87 {
    void**   list;     // +0x10BF30 DAT_6fbcbf30 head
    uint8_t* recs;     // +0x10C24C
    int32_t  count;    // +0x10C250
    int32_t  now;      // +0x10C268
    int32_t  lo, hi;   // +0x10C25C / +0x10C260
    uint32_t player;   // +0x10BBFC
};
struct D2cSnd87Env {
    uint32_t (*ord10032)(void);
    void     (*ord10006)(void);
    void     (*ord10062)(void);   // per-node call in 88150 (args implicit in retail)
    void     (*ord10063)(void);
    void     (*fn87ed0)(int32_t now, uint32_t key, int32_t mode);
    void     (*fatal)(uint32_t code); // Ordinal_10265+10024+_exit(-1); no return
    void     (*fn88550)(uint8_t* entry);                                                    // 0x88550 (ECX = entry); null: run the port's own
    void     (*fn88230)(uint32_t eax, int32_t key, int32_t p1, int32_t p2, int32_t p3);     // 0x88230 (EAX = rec[0x40], ECX = key, stack key p2 p3); null: run the port's own
};

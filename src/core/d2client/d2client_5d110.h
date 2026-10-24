#pragma once
#include <cstdint>
struct D2cRes5d {
    uint8_t*  recs;      // +0x10C24C DAT_6fbcc24c record array, stride 0x8E
    int32_t   count;     // +0x10C250
    int32_t   now;       // +0x10C268 tick stamp
    void**    list;      // +0x10C264 list node: [0]=record index, [1]=next node
    int32_t   used;      // +0x10C27C
    int32_t   live;      // +0x10C274
    int32_t   inited;    // +0x10C280
    uint32_t  player;    // +0x10BBFC
    struct Slot {        // +0x1175C0, 16 x 0x20; own layout, retail offsets noted
        uint32_t h;      // +0x00
        uint32_t mode;   // +0x04
        uint8_t* rec;    // +0x08
        uint8_t  pad[0x10];
    } slot[16];
};
static_assert(sizeof(D2cRes5d::Slot) >= 0x18, "slot");

struct D2cRes5dEnv {
    int      (*ord10041)(void);
    void     (*ord10061)(void);
    uint32_t (*ord10032)(void);
    void     (*ord10006)(void);
    void     (*ord10049)(void);
    uint32_t (*ord10065)(void);          // slot handle alloc, 0 = fail
    int      (*ord10056)(void);
    int      (*ord10046)(void);
    int      (*ord10059)(void);
    void     (*ord10066)(void);
    uint32_t (*ord10094)(void);
    uint32_t (*ord10095)(void);
    void     (*ord10062)(void);
    void     (*ord10018)(void);
    void     (*ord10097)(void);
    void     (*ord10043)(uint32_t a, uint32_t b);
    uint32_t (*ord10331)(uint32_t);
    uint32_t (*ord10826)(uint32_t);
    uint32_t (*ord10014)(uint32_t);
    void     (*fn5cf90)(void);
    void     (*fn5cd10)(void);
    int      (*ord10044)(void);          // Ordinal_10044 gate used by 0x5CC10
    void     (*fatal)(uint32_t code);    // Ordinal 10265+10024+_exit(-1); no return
    const char* sufA;                    // DAT_6fb853e4 (6 bytes incl NUL)
    const char* sufB;                    // DAT_6fb853ec (8 bytes incl NUL)
    uint8_t  (*ord14byte)(uint32_t unit);// reads +0x21C of Ordinal_10014 result
};

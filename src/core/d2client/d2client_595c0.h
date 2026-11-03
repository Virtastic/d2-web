#pragma once
#include <cstdint>
struct D2cExitState {
    uint32_t  state;       // +0x10C2CC DAT_6fbcc2cc: 0 idle,1/2 box up,3..6 sent,7 waiting
    uint32_t* boxA;        // +0x0C77F0 DAT_6fbc77f0 (box shown by 599E0)
    uint32_t* boxB;        // +0x0C77E8 DAT_6fbc77e8 (box shown by 59AC0)
    uint32_t  tick;        // +0x0C77F4 DAT_6fbc77f4 (GetTickCount stamp, -1 none)
    uint32_t  flag77f8;    // +0x0C77F8
    uint16_t  val77ec;     // +0x0C77EC low half of _DAT_6fbc77ec
    uint16_t  val77ee;     // +0x0C77EE high half
    uint32_t  inGame;      // +0x0FADDC DAT_6fbaaddc
    uint32_t  aad94;       // +0x0FAD94 DAT_6fbaad94
    uint32_t  aad60;       // +0x0FAD60 DAT_6fbaad60
    uint32_t  player;      // +0x10BBFC DAT_6fbcbbfc (unit ptr, 32-bit)
    uint32_t  be74;        // +0x10BE74
    uint32_t  c2d0;        // +0x10C2D0
    uint32_t  a8c9c;       // +0x0F8C9C DAT_6fba8c9c
    uint32_t  c404;        // +0x10C404
    uint32_t  c3a4;        // +0x10C3A4
    uint32_t  c418, c414, c434; // +0x10C418/414/434
    uint32_t  d349c, d3498;     // +0x1134 9C/98
    uint32_t  mouseX, mouseY;   // +0x10B828 / +0x10B824
    uint32_t  b9a0, b9a4;       // +0x10B9A0 / +0x10B9A4 panel origin
    uint32_t  d5d0;             // +0x0DD5D0 DAT_6fb8d5d0
    uint32_t  a9e14;            // +0x0F9E14 DAT_6fba9e14
};

struct D2cExitEnv {
    void     (*send)(const uint8_t* pkt);                       // 0x143E0 send packet
    void     (*send8)(uint32_t id, uint16_t hi, uint16_t lo);   // 0x149A0
    uint32_t (*tick)(void);                                     // GetTickCount
    void     (*closebox)(void);                                 // 0xA3D20
    void     (*uiMsg)(uint32_t a, uint32_t b, uint32_t c);      // 0xC2790
    void     (*fn8bb90)(void);                                  // 0x8BB90
    void     (*fn88a70)(uint32_t, uint32_t, uint32_t, uint32_t);// 0x88A70
    void     (*fn8ca20)(void);                                  // 0x8CA20
    void     (*fn14660)(void);                                  // 0x14660
    uint32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer); // Ordinal 10973
    uint32_t*(*newBox)(uint32_t w, uint32_t h, uint32_t cbA, uint32_t cbB, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); // 0xA3E40
    void     (*addButton)(uint32_t* box, uint32_t str, uint32_t col, uint32_t, uint32_t, uint32_t cb, uint32_t idx); // 0xA36F0
    uint32_t (*getStr)(void);                                   // Ordinal 10003 (string id source)
    void     (*fatal)(uint32_t code);                           // Ordinal 10265+10024+_exit(-1); no return
    void     (*click)(uint32_t arg);                            // Ordinal 10003(arg,0,0)+10085
    void     (*boxUpdate)(uint32_t* box);                       // 0xA3840
    uint32_t (*ord10331)(uint32_t);                             // Ordinal 10331
    uint32_t (*ord10826)(uint32_t);                             // Ordinal 10826
    uint32_t (*ord10416)(uint32_t);                             // Ordinal 10416
    void     (*ord10227)(void);                                 // Ordinal 10227
    void     (*ord10006)(void);                                 // Ordinal 10006
    void     (*fn3a4d0)(uint32_t, uint32_t, uint32_t);          // 0x3A4D0
    void     (*fn6f720)(void);                                  // 0xBF720
    uint32_t (*ord10048)(void);                                 // Ordinal 10048
    void     (*log1)(uint32_t a, uint32_t strRva);              // 0x3CC60 (faecc60)
    void     (*log2)(uint32_t a, uint32_t strRva);              // 0x3CCC0 (faeccc0)
    void     (*fn6e2e0)(void);                                  // 0xBE2E0
};


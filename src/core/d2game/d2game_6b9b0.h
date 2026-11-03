// Seam of D2Game+0x6B9B0 (drop an object beside the unit). pCtx = {+0 game, +8 unit}; every callee is a hooked function in the capture.
#pragma once
#include <cstdint>
struct D2GameDropObjEnv {
    uint32_t (*room)(uint32_t pUnit);                                          // 0xA478 (1 stack word)
    void (*pos)(uint32_t pUnit, uint32_t* pXY);                                // 0xA544 (2 stack words, writes x y)
    uint32_t (*kindOf)(uint32_t pRoom);                                        // 0xA556 (1)
    uint32_t (*a56e)(uint32_t w);                                              // 0xA56E (1)
    uint32_t (*aade)(uint32_t b);                                              // 0xAADE (1; the word's high bytes are stack residue)
    uint32_t (*a694)(uint32_t pRoom, uint32_t* pXY, uint32_t a, uint32_t b, uint32_t c);   // 0xA694 (5 stack words; may move the position)
    uint32_t (*a808)(uint32_t pRoom);                                          // 0xA808 (1)
    void (*dffd0)(uint32_t pUnit, uint32_t nMsg);                              // 0xDFFD0 (ESI = unit; stack 0x18, unit)
    uint32_t (*ae98)(uint32_t pRoom, uint32_t* pXY, uint32_t* pXY2, uint32_t a, uint32_t b, uint32_t c, uint32_t d);   // 0xAE98 (7 stack words)
    uint32_t (*f1280)(uint32_t pRoom, uint32_t y);                             // 0x1280 (EAX = room, EDI = y)
    uint32_t (*create)(uint32_t a, uint32_t b, uint32_t x, uint32_t y, uint32_t game, uint32_t r, uint32_t c, uint32_t d, uint32_t e);   // 0xE1D90 (ECX, EDX, 7 stack words)
    void (*a4a2)(uint32_t pObj, uint32_t nMode);                               // 0xA4A2 (2)
    uint32_t (*a22e0)(uint32_t game, uint32_t pObj, uint32_t nAade, uint32_t nKind);   // 0xA22E0 (4)
    uint32_t (*aee0)(uint32_t pObj);                                           // 0xAEE0 (1)
    void (*aec8)(uint32_t pObj, uint32_t nFlags);                              // 0xAEC8 (2; the flags word's high bytes are stack residue)
    void (*a51a)(uint32_t pRoom, uint32_t n);                                  // 0xA51A (2)
    void (*fatal)(uint32_t nLine);
};
void D2Game_6B9B0(uint32_t pCtx, const D2GameDropObjEnv* pEnv);

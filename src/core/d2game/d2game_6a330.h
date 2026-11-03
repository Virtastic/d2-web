// Seam shared by D2Game+0x6A330 (skill spawn check, EAX = out flag, EBX = game, EDX = unit id, EDI = caster, stack word 0 = unit type) and
// D2Game+0x6A720 (its wrapper, EAX = unit type, ECX = caster, stack game, id). Every callee is a hooked function in the capture; the probe logs
// the words retail passes, as the descriptor's hook lines do. Pointers are the retail 32-bit words.
#pragma once
#include <cstdint>
struct D2GameSkillEnv {
    uint32_t (*findUnit)(uint32_t nType, uint32_t pGame, uint32_t nId);        // 0xE03A0 (EAX = type word, ECX = game, EDX = id; no stack args)
    uint32_t (*unitRec)(uint32_t pUnit);                                       // 0xB156 (1 stack word)
    uint32_t (*playerData)(uint32_t pUnit);                                    // 0xA4C0 (1 stack word)
    uint32_t (*a6a6)(uint32_t w);                                              // 0xA6A6 (1 stack word: caster+0x60)
    void (*a478)(uint32_t pUnit);                                              // 0xA478 (1 stack word, result unused here)
    uint32_t (*callHandler)(uint32_t fn, const uint32_t* pBlock);              // table word, ECX = the 5-word block {game, unit, caster, game+0x10F0, class}
    uint32_t (*classRec)(uint32_t nClass);                                     // 0xA520 (1 stack word)
    uint32_t (*aaa8)(uint32_t pCaster, uint32_t pUnit);                        // 0xAAA8 (2 stack words)
    uint32_t (*skill330)(uint32_t* pFlag, uint32_t pGame, uint32_t nId, uint32_t pCaster, uint32_t nType);   // 0x6A330 (EAX, EBX, EDX, EDI, stack word 0)
    void (*fatal)(uint32_t nLine);                                             // Fog 10265/10024
};
// pTable = the handler table (module +0x107BB8, 0x65 module-address words).
uint32_t D2Game_6A330(uint32_t* pFlag, uint32_t pGame, uint32_t nId, uint32_t pCaster, uint32_t nType, const uint32_t* pTable, const D2GameSkillEnv* pEnv);
uint32_t D2Game_6A720(uint32_t nType, uint32_t pCaster, uint32_t pGame, uint32_t nId, const D2GameSkillEnv* pEnv);

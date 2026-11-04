// Callee seam table of the D2Game 0x7E640-0x89C30 AI think ports (d2game_batch_77d90d.cpp); the FN probes bind it to HookEnv recorders.
#pragma once
#include <cstdint>
struct D2Game77dEnv {
    // Ordinal_11103(unit, 0) (stdcall).
    void     (*ordinal11103)(uint32_t unit, uint32_t zero);
    // 0x1320(eax = unit class id): monstats row or 0; byte +0x104 is masked with the 0xF8BE0 global.
    uint32_t (*sub1320)(uint32_t classId);
    // The global pointer at D2Game+0xF8BE0; dwords +0x10 and +0x14 are the masks.
    uint32_t (*globalF8BE0)(void);
    // 0x75E10(game, cmd, 1) (stdcall): post a command record.
    void     (*sub75E10)(uint32_t game, const uint8_t* cmd, uint32_t one);
    // 0x20EC0(eax = unit, stack game): free the current command.
    void     (*sub20EC0)(uint32_t unit, uint32_t game);
    // 0xE03A0(eax = type, edx = id, ecx = game): the unit found or 0 (SUNIT_GetServerUnit).
    uint32_t (*subE03A0)(uint32_t type, uint32_t id, uint32_t game);
    // 0xF1B10(al, game, unit) (stdcall): walk close to the unit.
    void     (*subF1B10)(uint32_t al, uint32_t game, uint32_t unit);
    // 0xF2220(eax = mode, edi = unit, game, pTarget): change mode and target.
    void     (*subF2220)(uint32_t mode, uint32_t unit, uint32_t game, uint32_t pTarget);
    // 0xF22C0(eax = game, stack value).
    void     (*subF22C0)(uint32_t game, uint32_t value);
    // 0xF2380(al, ecx = unit, ebx = game, skill, pTarget, 0, 0): use a skill.
    void     (*subF2380)(uint32_t al, uint32_t unit, uint32_t game, uint32_t skill, uint32_t pTarget, uint32_t z0, uint32_t z1);
    // 0xF2540(ecx = game, edx = unit, pTarget, 2, 0, 0, 1, 7).
    uint32_t (*subF2540)(uint32_t game, uint32_t unit, uint32_t pTarget, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    // 0xF2D80(eax = pTarget, game, unit, byte, 1): non-zero when it acted.
    uint32_t (*subF2D80)(uint32_t pTarget, uint32_t game, uint32_t unit, uint32_t byteArg, uint32_t one);
    // Ordinal_10507(unit); 0x9F9A0(eax = skill, value, 1); 0xA2100(ecx = unit, edx = 0, skill, -1).
    uint32_t (*ordinal10507)(uint32_t unit);
    void     (*sub9F9A0)(uint32_t skill, uint32_t value, uint32_t one);
    void     (*subA2100)(uint32_t unit, uint32_t zero, uint32_t skill, uint32_t minusOne);
    // Import thunks, named by their D2Game address (all stdcall): 0x2A718(unit) = 0..99 style roll R,
    // 0x2A478(unit) -> a, 0x2A556(a) -> b, 0x2A56E(b) -> byte, 0x2A4C0(player) -> table (dword +0x10+diff*4),
    // 0x2A568(v, 0x15, 0) -> flag, 0x2A3B8(sub)/0x2A454(sub) -> dword, 0x2AFE8(a, b, c, 0x40) -> word.
    uint32_t (*th2A718)(uint32_t unit);
    uint32_t (*th2A478)(uint32_t unit);
    uint32_t (*th2A556)(uint32_t a);
    uint32_t (*th2A56E)(uint32_t a);
    uint32_t (*th2A4C0)(uint32_t player);
    uint32_t (*th2A568)(uint32_t v, uint32_t k, uint32_t z);
    uint32_t (*th2A3B8)(uint32_t sub);
    uint32_t (*th2A454)(uint32_t sub);
    uint32_t (*th2AFE8)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    // 0xF2C10(eax = pTarget, ebx = unit, game, 8, 1): non-zero when it acted.
    uint32_t (*subF2C10)(uint32_t pTarget, uint32_t unit, uint32_t game, uint32_t eight, uint32_t one);
    // 0xF26B0(eax = 0, ecx = unit, game, pTarget, 4).
    void     (*subF26B0)(uint32_t zero, uint32_t unit, uint32_t game, uint32_t pTarget, uint32_t four);
    // 0x1900(eax = unit): distance-like integer to the unit's current target.
    int32_t  (*sub1900)(uint32_t unit);
    // 0xB17B0(eax = 1, ecx = buf, edx = game, unit, callback): the per-unit search; the callback rva
    // (0x7BB00 or 0x7CF10) selects the filter. The buffer layout is the callback's.
    void     (*subB17B0)(uint32_t buf, uint32_t game, uint32_t unit, uint32_t callbackRva);
    // Import thunks (stdcall): 0x2A3D0(ecx = unit, edx = skill, -1) -> skill record or 0;
    // 0x2A400(unit, rec, 1) -> int; 0x2A68E(unit, state) -> flag; 0x2A3CA(unit, state, 0).
    uint32_t (*th2A3D0)(uint32_t unit, uint32_t skill, uint32_t minusOne);
    int32_t  (*th2A400)(uint32_t unit, uint32_t rec, uint32_t one);
    uint32_t (*th2A68E)(uint32_t unit, uint32_t state);
    void     (*th2A3CA)(uint32_t unit, uint32_t state, uint32_t zero);
    // 0x412F0(eax = unit, game, cmd): set the unit's AI command; cmd has kind at +8, id +0xC, type +0x10.
    void     (*sub412F0)(uint32_t unit, uint32_t game, uint32_t cmd);
    // 0xF3E30(eax = y, ecx = target unit or 0, game, skill, x): non-zero when the skill may be used.
    uint32_t (*subF3E30)(uint32_t y, uint32_t target, uint32_t game, uint32_t skill, uint32_t x);
    // 0xF2870(al, ecx = target, game, unit); 0xF2120(edi = skill, game, target, 0, 0).
    void     (*subF2870)(uint32_t al, uint32_t target, uint32_t game, uint32_t unit);
    void     (*subF2120)(uint32_t skill, uint32_t game, uint32_t target, uint32_t z0, uint32_t z1);
    // 0x1100(ecx = unit) / 0x1130(ecx = unit): the unit's x / y; 0x2AD78(unit, pTarget, 4) (stdcall).
    int32_t  (*sub1100)(uint32_t unit);
    int32_t  (*sub1130)(uint32_t unit);
    uint32_t (*th2AD78)(uint32_t unit, uint32_t pTarget, uint32_t four);
    // 0xB17B0 with its eax mode argument (1 or 9) in front; 0xB1BC0(eax = unit, ecx = game, &outDist)
    // returns a target unit and writes its distance.
    void     (*subB17B0m)(uint32_t mode, uint32_t buf, uint32_t game, uint32_t unit, uint32_t callbackRva);
    uint32_t (*subB1BC0)(uint32_t unit, uint32_t game, uint32_t outDist);
    // 0x89730(eax = unit, stack game); 0x7BE10(eax = game, esi = unit).
    uint32_t (*sub89730)(uint32_t unit, uint32_t game);
    void     (*sub7BE10)(uint32_t game, uint32_t unit);
    // 0x1450(esi = seed, stack n) and 0x11D0(ecx = seed, esi = n): seeded rolls below n.
    uint32_t (*sub1450)(uint32_t seed, uint32_t n);
    uint32_t (*sub11D0)(uint32_t seed, uint32_t n);
    // 0xF1D90(eax = mode, ecx, game, unit, a, b); 0xF20C0(eax = game, ebx = unit, value);
    // 0xF1FF0(eax = 8, ecx = unit, game, x, y); 0xB07B0(eax = unit, x, y) -> distance.
    void     (*subF1D90)(uint32_t mode, uint32_t ecx, uint32_t game, uint32_t unit, uint32_t a, uint32_t b);
    void     (*subF20C0)(uint32_t game, uint32_t unit, uint32_t value);
    void     (*subF1FF0)(uint32_t mode, uint32_t unit, uint32_t game, uint32_t x, uint32_t y);
    uint32_t (*subB07B0)(uint32_t unit, uint32_t x, uint32_t y);
    // 0x2A71E(unit, 0xFFFF, 1) -> mask; 0x2A694(room, ptr, mask) -> non-zero when blocked.
    uint32_t (*th2A71E)(uint32_t unit, uint32_t mask, uint32_t one);
    uint32_t (*th2A694)(uint32_t room, uint32_t ptr, uint32_t mask);
    // 0xE03A0 with the registers exactly as 0x89C30 loads them: eax = ai+0x30, ecx = ai+0x28, edx = ai+0x2C.
    uint32_t (*subE03A0raw)(uint32_t eax, uint32_t ecx, uint32_t edx);
    // The structure behind the pointer-to-pointer at D2Game+0x1829C: +0xB98 skill record table (stride 0x23C),
    // +0xBA0 record count.
    uint32_t (*globalSkillData)(void);
};

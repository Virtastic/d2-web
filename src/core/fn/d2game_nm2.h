// Seam environment for the D2Game quest / NPC callback rows reached through pointer tables: GE plus the idioms those callbacks share.
// Every callee is a recorder (hookenv.h); a code pointer handed to a callee is logged 0xB2000000|rva, as the capture logs module addresses.
#pragma once
#include "d2game_gp.h"
struct NE : GE {
    NE(const uint32_t* rr, int nblobs) : GE(rr, nblobs) {}
    static uint32_t rd16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
    static void wr16(uint32_t a, uint32_t v) { uint16_t x = (uint16_t)v; memcpy((void*)(uintptr_t)a, &x, 2); }
    static uint32_t lo(uint32_t eax, uint32_t al) { return (eax & ~0xFFu) | (al & 0xFFu); }   // mov al, imm8 over a live EAX
    // an import or stack-only callee: no register is part of the record
    uint32_t imp(uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0, uint32_t s7 = 0) { return h(rva, 0, 0, 0, 0, 0, s0, s1, s2, s3, s4, s5, s6, s7); }
    // the player's quest-flag buffer for the game's difficulty: player data (D2Common 10920) +0x10 + 4 * game +0x6D
    uint32_t qbuf(uint32_t game, uint32_t unit) { return rd32(imp(0xA4C0, unit) + 0x10 + 4 * rd8(game + 0x6d)); }
    uint32_t fl(uint32_t buf, uint32_t q, uint32_t f) { return imp(0xA568, buf, q, f); }      // D2Common 10174: read a quest flag
    uint32_t setf(uint32_t buf, uint32_t q, uint32_t f) { return imp(0xA57A, buf, q, f); }    // D2Common 10003: set
    uint32_t clrf(uint32_t buf, uint32_t q, uint32_t f) { return imp(0xA586, buf, q, f); }    // D2Common 10593: clear
    // 0xDFCC0: run a callback over the game's players (EAX, EBX = callback, EDI = game, one stack word)
    uint32_t cb(uint32_t game, uint32_t fnRva, uint32_t eax = 0, uint32_t s0 = 0) { const uint32_t v = h(0xDFCC0, eax, 0, 0, 0xB2000000u | fnRva, 0, s0); edi(0xDFCC0, game); return v; }
    // the state-change trace: the import wsprintfA(stack buffer, "changing state to %d\n", n) through its slot 0xF8D58, then, when the
    // module's trace switch (0x111C20) is on, the text goes to 0xCBE70 (EDX = buffer, game, 4). The switch is off in the shipped module.
    uint32_t trace = 0;
    void say(uint32_t game, uint32_t n) {
        uint32_t buf[4] = {};
        imp(0xF8D58, stk(buf), 0xB2000000u | 0xFCC2C, n);   // wsprintfA(buf, "changing state to %d\n", n): the capture records the call
        if (trace) h(0xCBE70, 0, 0, stk(buf), 0, 0, game, 4);
    }
    // the game's quest list: holder at game +0x10F4, node id +0, next +0xF4; 0 = absent
    uint32_t quest(uint32_t game, uint32_t id) const {
        const uint32_t holder = rd32(game + 0x10f4);
        uint32_t n = holder ? rd32(holder) : 0;
        while (n && rd32(n) != id) n = rd32(n + 0xf4);
        return n;
    }
};

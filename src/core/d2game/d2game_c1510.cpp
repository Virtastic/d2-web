// 1.13c D2Game+0xC1510
// Claim of a reward slot by a player (caller 0xC1890). stdcall (self = player record, unit = player unit, pkt = the request
// record, slotWord = the requested slot in its low 16 bits), ret 0x10, no register inputs. Every callee is reached through
// the env with the registers and stack words retail hands it; pointers are 32-bit addresses.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2GameC1510Env {
    int32_t  (*o10973)(uint32_t unit, uint32_t stat, uint32_t layer);                       // 0xA42A stat get
    uint32_t (*oA4C0)(uint32_t unit);                                                        // 0xA4C0 player data
    int32_t  (*oA568)(uint32_t v, uint32_t a, uint32_t b);                                    // 0xA568
    uint32_t (*oABF8)(uint32_t ecx, uint32_t edx, uint32_t s0);                               // 0xABF8 fastcall, 1 stack word
    int32_t  (*oABE0)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);   // 0xABE0 fastcall, 4 stack words
    int32_t  (*subBCB20)(uint32_t eax, uint32_t ecx);
    uint32_t (*sub8F5C0)(uint32_t eax, uint32_t ecx);
    void     (*subBC7D0)(uint32_t eax, uint32_t ebx, uint32_t esi, uint32_t edi, uint32_t s0); // ebx/esi/edi: out-pointers
    int32_t  (*subBCC60)(uint32_t esi, uint32_t s0);
    uint32_t (*subEDDB0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
    void     (*sub48FC0)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    uint32_t (*sub1370)(uint32_t eax);
    uint32_t (*sub48390)(uint32_t ecx);
    void     (*subBD330)(uint32_t eax, uint32_t s0, uint32_t s1);
    void     (*sub8AC90)(uint8_t al, uint8_t cl, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
    void     (*subBD160)(uint32_t edi, uint32_t s0);
    void     (*sub8A3E0)(uint32_t eax, uint32_t buf, uint32_t len);
};

namespace {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, P(a) + o, 4); return v; }
inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, P(a) + o, 2); return v; }
inline uint8_t rd8(uint32_t a, uint32_t o) { return P(a)[o]; }
inline uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }

// the reply target: a player unit's client (player data +0x9C), else 0
uint32_t replyTag(const D2GameC1510Env* e, uint32_t unit) { return unit && rd32(unit, 0) == 0 ? rd32(e->oA4C0(unit), 0x9C) : 0; }

// 0x2A reply through 0x8AC90: code in CL, the gold word (stat 0xE) in EDX, stack (client, item id, 0)
void reply(const D2GameC1510Env* e, uint32_t unit, uint8_t code, uint32_t iid) {
    uint32_t tag = replyTag(e, unit);
    uint32_t gold = (uint32_t)e->o10973(unit, 0xE, 0);
    e->sub8AC90(0x2A, code, gold, tag, iid, 0);
}

// the 15-byte 0x2A packet built on the stack and sent raw through 0x8A3E0; bytes 3..6 are never written by retail
void sendRaw(const D2GameC1510Env* e, uint32_t unit, uint8_t code) {
    uint32_t tag = replyTag(e, unit);
    uint8_t buf[15] = {};
    buf[0] = 0x2A; buf[2] = code;
    uint32_t gold = (uint32_t)e->o10973(unit, 0xE, 0);
    std::memcpy(buf + 11, &gold, 4);
    buf[1] = 0;
    std::memset(buf + 7, 0xFF, 4);
    e->sub8A3E0(tag, A(buf), 0xF);
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C1510(uint32_t self, uint32_t unit, uint32_t pkt, uint32_t slotWord, const D2GameC1510Env* e)
{
    e->o10973(unit, 0xC, 0);
    int32_t kind = e->subBCB20(pkt, self);
    e->oA4C0(unit);
    if (pkt) {
        bool deny = false;
        if (rd32(pkt, 4) == 0x203 && e->oA568(rd32(e->oA4C0(unit), 0x10 + rd8(self, 0x6D) * 4), 0x24, 0) == 0) deny = true;
        else if (rd32(pkt, 4) == 0x96 && kind < 8 && e->oA568(rd32(e->oA4C0(unit), 0x10 + rd8(self, 0x6D) * 4), 2, 0) == 0) deny = true;
        if (deny) { sendRaw(e, unit, 0x0B); return; }
    }
    uint32_t argSlot = unit;                       // retail hands 0x8F5C0 the address of its unit argument slot, later reused for the kind
    uint32_t blk = e->sub8F5C0(A(&argSlot), self);
    if (!blk) { sendRaw(e, unit, 0x09); return; }
    argSlot = pkt ? rd32(pkt, 4) : 0xFFFFFFFFu;
    uint32_t inv = 0, lo = 0, hi = 0;
    e->subBC7D0(argSlot, A(&inv), A(&hi), A(&lo), self);
    int32_t slot = (uint16_t)slotWord;
    if (slot < (int32_t)lo || slot > (int32_t)hi) { sendRaw(e, unit, 0x09); return; }
    uint32_t ps = rd32(blk, 0x10) + (uint32_t)(slot - (int32_t)lo) * 16;
    if (rd16(ps, 0) != (uint16_t)slot || rd32(ps, 8) != 0) { reply(e, unit, 9, 0xFFFFFFFFu); return; }
    uint32_t key = e->oABF8(rd32(self, 0x70), 0, (ps & 0xFFFF0000u) | rd16(ps, 0));   // retail loads the slot with a 16-bit move into the pointer register
    uint8_t out[0x18] = {};
    if (!e->oABE0(rd32(self, 0x70), unit, rd32(ps, 4), key, rd8(self, 0x6D), A(out))) return;
    uint32_t v14; std::memcpy(&v14, out + 0x14, 4);
    if (!e->subBCC60(unit, v14)) { reply(e, unit, 0xC, 0xFFFFFFFFu); return; }
    uint32_t it = e->subEDDB0(inv, 1, pkt, self, 4, 0);
    if (!it) it = e->subEDDB0(inv, 1, unit, self, 4, 0);
    if (!it) { reply(e, unit, 0xF, 0xFFFFFFFFu); return; }
    uint32_t one = 1; std::memcpy(P(ps) + 8, &one, 4);
    e->sub48FC0(self, unit, it, 0xFFFF, ps, 0);
    e->sub48390(e->sub1370(pkt));
    e->subBD330(pkt, self, replyTag(e, unit));
    reply(e, unit, 5, rd32(it, 0xC));
    e->subBD160(blk, argSlot);
}

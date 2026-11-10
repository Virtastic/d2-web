// 1.13c D2Game+0x54D90: ECX = pBuf, stack = a0, a1, nSize, bCorpse, pnRead (ret 0x14). Reads a 'JM' item
// section. The rule over an environment E (HookEnv in the FN probe src/core/fn/d2game_54d90.cpp); every callee is one E::h call in retail's
// order (disassembled).
// - buf word 0 != 'JM': 0xE. n = word buf +2; read = 4; pos = buf +4.
// - per item (n of them, unsigned): the 18-dword item record is zeroed; word at pos != 'JM' gives 0xE; pos += 2;
//   len = 0x52BD0 (ESI = &record; pos); pos += len; read += len + 2 (nSize is decremented alike, never read).
//   kids = 0 then bCorpse: r = 0x54A20 (a0, a1, &record, &unused, &kids), nonzero r is returned; else r = 0x54BC0 (EAX = a1, ECX = a0;
//   &record, &unused, &kids), nonzero r moves to the next item.
// - kids (signed) socketed children follow, each read as above and handed to the same callee with a scratch in place of &kids; a nonzero
//   result from either callee is returned.
// - *pnRead = read; 0. (The 20-byte duplicate table is never filled: its count starts at 0 and only a match adds to it, so its loop and the
//   socket-count branch behind a match are dead in retail.)
#pragma once
#include <cstdint>

namespace g54d90 {
template <class E> uint32_t ReadItems(E& e, uint32_t buf, uint32_t a0, uint32_t a1, uint32_t corpse, uint32_t pnRead)
{
    if (e.rd8(buf) != 0x4A || e.rd8(buf + 1) != 0x4D) return 0xE;
    const uint32_t n = e.rd8(buf + 2) | e.rd8(buf + 3) << 8;
    uint32_t read = 4, pos = buf + 4;
    uint32_t rec[18];
    uint32_t unused = 0, kids = 0, scratch = 0;
    auto next = [&]() {
        for (auto& w : rec) w = 0;
        if (e.rd8(pos) != 0x4A || e.rd8(pos + 1) != 0x4D) return false;
        pos += 2;
        const uint32_t len = e.h(0x52BD0, 0, 0, 0, 0, e.stk(rec), pos);
        pos += len;
        read += len + 2;
        return true;
    };
    auto give = [&](uint32_t out) {
        if (corpse) {
            const uint32_t r = e.h(0x54A20, 0, 0, 0, 0, 0, a0, a1, e.stk(rec), e.stk(&unused), out);
            e.blk(0x54A20, (uint32_t)(uintptr_t)rec, 18);
            return r;
        }
        const uint32_t r = e.h(0x54BC0, a1, a0, 0, 0, 0, e.stk(rec), e.stk(&unused), out);
        e.blk(0x54BC0, (uint32_t)(uintptr_t)rec, 18);
        return r;
    };
    for (uint32_t i = 0; i < n; i++) {
        if (!next()) return 0xE;
        kids = 0;
        const uint32_t r = give(e.stk(&kids));
        if (r) { if (corpse) return r; continue; }
        for (int32_t j = 0; j < (int32_t)kids; j++) {
            if (!next()) return 0xE;
            const uint32_t q = give(e.stk(&scratch));
            if (q) return q;
        }
    }
    e.wr32(pnRead, read);
    return 0;
}
}

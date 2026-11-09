// 1.13c D2Game+0x524C0 TASK_InsertIntoQueues, transcribed from the 1.13c disassembly. Convention: stdcall, one stack argument (the new task), ret 4.
// Retail layout over 32-bit addresses. S is the address of the queue sentinel (0x6FD30398): S+0 head word, S+4 tail word, S+0x10 tasks created,
// S+0x14 games count. A task is {+0 slot index (selects a lock, 0x18 stride at 0x6FD31C58), +4 type count, +8 balance link {prev, next}, +0x10 queue
// link {word0, word1}}; a balance-list node carries its sort key at node-0xC. The loop walks the queue from the tail word along word 1 of each queue
// link until it reaches S. For every queued task it takes the task's balance list out (all of it when the step count is not 0; the step count is the
// type count less one when games/created plus the remainder test is zero) and re-inserts the nodes into the new task's balance list ordered by key.
// The new task's queue link then becomes the tail. The locks are Enter/LeaveCriticalSection on the slot (no memory effect the vector sees).
// Not reproduced: the null-link failure exit (task + 0x10 == 0 logs and _exit(-1)); created == 0 divides by zero as in retail.
#pragma once
#include <cstdint>

namespace r524c0 {
template <class E> void Do(E& e, uint32_t task, uint32_t S)
{
    const uint32_t created = e.rd32(S + 0x10), games = e.rd32(S + 0x14);
    const uint32_t per = games / created;
    uint32_t lim = created - 1;
    if (lim > 0x1f) lim = 0x20;
    int32_t lc = (int32_t)(lim - games % created);
    const uint32_t H = task + 8;
    uint32_t p8 = e.rd32(S + 4);
    while (p8 != S) {
        --lc;
        int32_t n = (int32_t)(e.rd32(p8 - 0xC) - (uint32_t)((((lc > -1) ? 1u : 0u) + per) == 0));
        const uint32_t p7 = p8 - 8;
        const uint32_t p2 = e.rd32(p7);
        uint32_t p11 = p2;
        for (; n != 0 && (n--, p11 != p7); p11 = e.rd32(p11 + 4)) {}
        uint32_t p6 = e.rd32(p11 + 4);
        e.wr32(p11 + 4, p7);
        e.wr32(p7, p11);
        if (p6 != p7) {
            e.wr32(p2 + 4, 0);
            uint32_t q7 = e.rd32(H);
            if (q7 != H) {
                do {
                    if ((int32_t)(e.rd32(p6 - 0xC) - e.rd32(q7 - 0xC)) >= 0) break;
                    q7 = e.rd32(q7);
                } while (q7 != H);
            }
            uint32_t p10 = e.rd32(q7 + 4);
            bool tail = true;
            while (p10 != H) {
                const uint32_t p3 = e.rd32(p6 + 4);
                e.wr32(p6, q7);
                p10 = e.rd32(q7 + 4);
                e.wr32(p6 + 4, p10);
                e.wr32(p10, p6);
                e.wr32(q7 + 4, p6);
                if (p3 == 0) { tail = false; break; }
                q7 = p6;
                if (e.rd32(p6 + 4) != H) {
                    p10 = e.rd32(p6 + 4);
                    do {
                        q7 = p6;
                        const uint32_t a = e.rd32(p3 - 0xC), b = e.rd32(p10 - 0xC);
                        if (a == b || (int32_t)(a - b) < 0) break;
                        p6 = p10;
                        q7 = p10;
                        p10 = e.rd32(p10 + 4);
                    } while (p10 != H);
                }
                p6 = p3;
                p10 = e.rd32(q7 + 4);
            }
            if (tail) {
                e.wr32(p6, q7);
                e.wr32(p2 + 4, H);
                e.wr32(q7 + 4, p6);
                e.wr32(H, p2);
            }
        }
        p8 = e.rd32(p8 + 4);
    }
    const uint32_t ql = task + 0x10;
    e.wr32(ql, S);
    const uint32_t tl = e.rd32(S + 4);
    e.wr32(ql + 4, tl);
    e.wr32(tl, ql);
    e.wr32(S + 4, ql);
}
}

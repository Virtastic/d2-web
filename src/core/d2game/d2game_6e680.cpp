// 1.13c D2Game+0x6E680 -- answer to a trade/party-style request (packet 0x58). ECX = idA, EAX = idB, stack = game, id, param3 (short); returns 1.
// Looks up three units in the game's id hash rows (id & 0x7F, chain link at +0xE4): other = row 0x1520 by id, subject = row 0x1120 by idA, and
// a third by idB (row 0x1720). Needs the subject, and the other to act on; sends packet {0x58, id, mode[, flag]} to the subject's client and
// moves the other's state: mode 1 on param3 == 2 (state 0), 4 refusing, 5 accepting (flag = approval; state 1, then 2 plus 0xC9CB0 for class 0x98,
// otherwise the 0x8EA80 notice). Any other param3 does nothing.
#include <cstring>
#include "d2game_6e680.h"
namespace {
uint32_t rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t find(uint32_t head, uint32_t id) {
    for (uint32_t p = rd(head); p; p = rd(p + 0xe4)) if (rd(p + 0xc) == id) return p;
    return 0;
}
}  // namespace
// 1.13c D2Game+0x6E680
uint32_t d2game_port_6e680(uint32_t idA, uint32_t idB, uint32_t game, uint32_t id, uint32_t param3, const D2Game6E680Env* env)
{
    if (!game) env->fatal(0x2d3);
    const uint32_t other = find(game + 0x1520 + 4 * (id & 0x7f), id);
    const uint32_t subj = find(game + 0x1120 + 4 * (idA & 0x7f), idA);
    uint32_t third = find(game + 0x1720 + 4 * (idB & 0x7f), idB);
    if (!subj) { if (!other) env->fatal(0x1109); return 1; }
    if (!other) return 1;
    uint32_t client = 0;
    if (rd(subj) == 0) client = rd(env->playerData(subj) + 0x9c);
    const uint32_t ecx = rd(other + 0x10);
    uint8_t pkt[8] = {0x58, (uint8_t)id, (uint8_t)(id >> 8), (uint8_t)(id >> 16), (uint8_t)(id >> 24), 0, 0, 0};
    const uint32_t mode = param3 & 0xffff;
    auto clear = [&] { wr(subj + 0x64, 0xffffffffu); wr(subj + 0x68, 6); *(uint8_t*)(uintptr_t)(subj + 0x6c) = 0; };
    if (mode == 2) {
        pkt[5] = 1;
        env->send(pkt, 6, client);
        env->setState(other, 0);
        clear();
        return 1;
    }
    if (mode != 3) return 1;
    const uint32_t row = env->rowOf(third ? rd(third + 4) : 0xffffffffu);
    bool accept;
    if (rd(other + 4) == 0x98) accept = rd(row + 0x80) == 0x20747368u;
    else accept = env->allowed(subj, game) != 0;
    if (!accept) { pkt[5] = 4; env->send(pkt, 6, client); return 1; }
    clear();
    pkt[5] = 5;
    uint32_t ok;
    if (rd(other + 4) == 0x98) ok = 1;
    else ok = env->roleOk((ecx & 0xffffff00u) | *(const uint8_t*)(uintptr_t)(row + 0x122), game) & 0xff;
    pkt[6] = (uint8_t)ok;
    env->send(pkt, 7, client);
    if (ok != 1) { env->setState(other, 0); return 1; }
    env->setState(other, 1);
    const uint32_t txt = env->textRow(rd(other + 4));
    if (rd(other + 4) == 0x98) {
        env->setState(other, 2);
        env->finish(subj, game, other);
        return 1;
    }
    env->notify(other, 1, (rd(txt + 0xdc) >> 8) + rd(game + 0xa8) + 1);
    return 1;
}

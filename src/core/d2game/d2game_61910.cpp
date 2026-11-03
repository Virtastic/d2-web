// 0x61910 (ECX = game, EDX = client; ret 4): quest 4 intro. Unless #10174 reads state 0 or state 1 of quest 4 nonzero: #10003 state 0xE, then adds the client's unit id
// (-1 if none) to the list at the first type-4 node's data +0xB4 (0x33880: EAX = list, ESI = id). Returns 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x61910
uint32_t d2game_61910_q32(const Call& X, uint32_t game, uint32_t client) {
    uint32_t sk = questRec(X, game, client);
    if (qGet(X, sk, 4, 0) != 0) return 0;
    if (qGet(X, sk, 4, 1) != 0) return 0;
    qSet(X, sk, 4, 0xe);
    uint32_t n = findQ(game, 4);
    uint32_t id = client ? rd32(client, 0xc) : 0xffffffffu;
    X(0x33880, rd32(n, 0x18) + 0xb4, 0, 0, 0, id);
    return 0;
}

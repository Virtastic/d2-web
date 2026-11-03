// 0x62CD0 (ECX = game, EDX = client; ret 4): quest 1. Unless #10174 reads state 0 or state 1 as exactly 1: set state 0xE (#10003), call 0x34AA0 (stack 1, 0; ret 8)
// and 0x34C00 (EAX = client, stack game; ret 4). Returns 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x62CD0
uint32_t d2game_62CD0_q32(const Call& X, uint32_t game, uint32_t client) {
    uint32_t sk = questRec(X, game, client);
    if (qGet(X, sk, 1, 0) == 1) return 0;
    if (qGet(X, sk, 1, 1) == 1) return 0;
    qSet(X, sk, 1, 0xe);
    X(0x34AA0, 0, 0, 0, 0, 0, 1, 0);
    X(0x34C00, client, 0, 0, 0, 0, game);
    return 0;
}

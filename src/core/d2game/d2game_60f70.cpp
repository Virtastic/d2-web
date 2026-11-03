// 0x60F70 (ECX = game, EDX = client; ret 4): quest 5 area entry. Skips when state 0 or 1 of quest 5 is nonzero, or when the client's room (#10331) is null or its
// level (#10826) is not 0x19. Otherwise adds the client's unit id to the list at the first type-5 node's data +0x8C (0x33800 test, 0x33880 add) and sets states 0 and 0xD.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x60F70
uint32_t d2game_60F70_q32(const Call& X, uint32_t game, uint32_t client) {
    uint32_t sk = questRec(X, game, client);
    if (qGet(X, sk, 5, 0) != 0) return 0;
    if (qGet(X, sk, 5, 1) != 0) return 0;
    uint32_t room = X(0xA478, 0, 0, 0, 0, 0, client);
    if (!room) return 0;
    if ((X(0xA556, 0, 0, 0, 0, 0, room) & 0xffffffffu) != 0x19) return 0;
    uint32_t n = findQ(game, 5);
    if (!n) return 0;
    uint32_t id = client ? rd32(client, 0xc) : 0xffffffffu;
    uint32_t d = rd32(n, 0x18);
    if (!X(0x33800, 0, 0, d + 0x8c, 0, id)) X(0x33880, d + 0x8c, 0, 0, 0, id);
    qSet(X, sk, 5, 0);
    qSet(X, sk, 5, 0xd);
    return 0;
}

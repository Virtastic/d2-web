// 1.13c D2Game+0x561A0: read a joining client's save header into a player
// unit. eax = ppUnit (out), stack game, client, p7, a4, a5 (ret 0x14). Empty header -> 0xE; otherwise the
// save-file dispatcher 0x56140 fills *ppUnit; on its error the out pointer is cleared, on success with a
// player the header is freed (game types 1/2) and the quest sequences cycle. Callees and the two client
// reads are seams; the client layout pins pSaveHeader +0x17C / nSaveHeaderSize +0x180 (D2MOO Clients.h).
#include <cstddef>
#include <cstdint>

typedef uint32_t W;

#pragma pack(push, 1)
struct D2G561A0Client { uint8_t pad0x00[0x17C]; W pSaveHeader; W nSaveHeaderSize; };
struct D2G561A0Game { uint8_t pad0x00[0x6A]; uint8_t nGameType; };
#pragma pack(pop)
static_assert(offsetof(D2G561A0Client, pSaveHeader) == 0x17C && offsetof(D2G561A0Client, nSaveHeaderSize) == 0x180,
              "D2ClientStrc fields");
static_assert(offsetof(D2G561A0Game, nGameType) == 0x6A, "D2GameStrc.nGameType");

struct D2G561A0Seam {
    W (*dispatch)(void* ctx, W game, W client, W saveHeader, W size, W ppUnit, W p7, W a4, W a5); // 0x56140: ebx=ppUnit, edi=p7, esi=a4, edx=a5
    void (*freeSaveHeader)(void* ctx, W client);                                                   // 0x49630: esi=client, no stack words
    void (*sequenceCycler)(void* ctx, W game, W player, W bGameEnter);                             // 0x35090, 3 stack words
    W (*rd)(W addr);
    void (*wr)(W addr, W value);
    void* ctx;
};

W GAME_561A0(const D2G561A0Seam& s, W ppUnit, W game, W client, W p7, W a4, W a5)
{
    s.wr(ppUnit, 0);
    if (!client) return 0;   // retail: Ordinal 10265(0xB41) -> 10024 -> _exit(-1); precondition, not modelled
    const W saveHeader = s.rd(client + offsetof(D2G561A0Client, pSaveHeader));
    const W size = s.rd(client + offsetof(D2G561A0Client, nSaveHeaderSize));
    if (!size) return 0xE;
    const W r = s.dispatch(s.ctx, game, client, saveHeader, size, ppUnit, p7, a4, a5);
    if (r) { s.wr(ppUnit, 0); return r; }
    const W player = s.rd(ppUnit);
    if (player) {
        const W type = (s.rd(game + 0x68) >> 16) & 0xFF;   // byte at game+0x6A, nGameType
        if (type == 1 || type == 2) s.freeSaveHeader(s.ctx, client);
        s.sequenceCycler(s.ctx, game, player, 0);
    }
    return 0;
}

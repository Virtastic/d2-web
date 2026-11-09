// 1.13c D2Game+0xBF990: an NPC transaction on one item (sell / buy-back / repair style check): find the NPC by id and the
// item by id, check the player is talking to that NPC and may trade the item, price it, try to put a copy into the NPC's
// store, then run the mode's action and reply. EAX = NPC id; stdcall (game, player, itemId, mode), ret 0x10; returns 0
// (done), 1 (nothing to do / refused quietly) or 3 (refused with a reply). Written from the 1.13c disassembly
//. Callees go through Env so the FN probe records them in retail's order.
#ifndef D2GAME_BF990_CPP
#define D2GAME_BF990_CPP
#include "d2g_mem.h"
namespace d2game_bf990 {
using namespace d2g;
struct Env {
    P (*interactUnit)(P game, U type, U id);         // 0xE03A0: ECX game, EAX type ([player+0x68]), EDX id ([player+0x64])
    I (*canAccess)(P player, P item);                 // 0xE150: ECX player, EDI item, EAX 0
    P (*pcData)(P player);                            // thunk 0xA4C0 (1)
    I (*stat)(P unit, I stat, I layer);               // thunk 0xA42A (3)
    void (*reply)(I cmd, I code, I value, U client, I a, I b);   // 0x8AC90: AL cmd, CL code, EDX value, stack 3
    void (*send)(U client, uint8_t* pkt, I len);      // 0x8A3E0: EAX client, stack 2
    I (*isItem)(P item);                              // thunk 0xAC46 (1)
    U (*npcRecord)(P player, P game);                 // 0x1020: EAX player, stack 1 (two more words ride to the next call)
    I (*price)(P player, P item, I diff, U rec, U npcClass, I one);   // thunk 0xA406 (6)
    I (*quality)(P item);                             // thunk 0xA898 (1)
    U (*itemFile)(P item);                            // thunk 0xA874 (1)
    U (*fileName)(U v);                               // thunk 0xAC2E (1)
    I (*strcmpi)(U a, U b);                           // 0x2813 (cdecl 2)
    I (*flagged)(P item, U flag, I line, U file);     // thunk 0xA8A4 (4)
    I (*itemType)(P item);                            // thunk 0xAA66 (1)
    I (*socketed)(P item);                            // 0xD1BC0: EAX item
    I (*questItem)(P item);                           // 0x17E0: ESI item
    U (*storeOf)(P game, P npc, U* out);              // 0x8F820: ECX game, EAX npc, stack 1 (out)
    P (*storeState)(P player, U store, P game);       // 0x8F6C0: EAX player, EDI store, stack 1
    I (*inStore)(P npc, P game, P item);              // 0xBCAA0: EAX npc, EBX game, stack 1 (AL)
    P (*copyItem)(P game, P item, I one);             // 0x107E0: EAX = EDI = game, stack 2
    void (*setFlag)(P item, I v);                     // thunk 0xA4A2 (2)
    U (*page)(P item);                                // thunk 0xAC34 (1): AL
    void (*freeItem)(P game, P item);                 // 0xEE70: EAX game, ESI item
    void (*setPage)(P item, U v);                     // thunk 0xA92E (2)
    U (*posX)(P item);                                // 0x1100: ECX item
    U (*posY)(P item);                                // 0x1130: ECX item
    I (*place)(U file, I line, P game, P npc, U id, U x, U y, I a, I b, I c);   // 0xD76A0: ECX file, EDX line, stack 8
    void (*addStore)(P npc, P item, I one);           // 0xBCE60: EDI npc, ESI item, stack 1
    U (*durMax)(P item);                              // thunk 0xAA00 (1)
    U (*qtyMax)(P item);                              // thunk 0xAA06 (1)
    void (*setStat)(P unit, I stat, U v, I layer);    // thunk 0xA472 (4)
    I (*hasType)(P item, I type);                     // thunk 0xA430 (2)
    U (*itemCode)(P item, I z);                       // thunk 0xA90A (2): AX
    P (*itemsTxt)(U code);                            // thunk 0xA9D6 (1)
    void (*charge)(P player, U v, I n);               // 0xBF8F0: EAX player, stack 2
    void (*mkFlags)(U* out, I bits, I one);           // thunk 0xAA5A (3)
    U (*invPage)(P item);                             // thunk 0xA6C4 (1)
    void (*setInvPage)(P item, U v);                  // thunk 0xA958 (2)
    void (*sendItem)(P item, P player, U client, U flags);   // 0x8C810: EAX item, EDI player, EBX client, stack 1
    void (*resync)(P player, P game, I z);            // 0xD2D90: EAX player, stack 2
    I (*modeFour)(P player, P item, P game);          // 0xD2C70: EAX player, ESI item, stack 1
    U (*bodyLoc)(P item);                             // thunk 0xA97C (1)
    I (*modeOther)(P game, P player, U loc, I one, U* out);  // 0xD8970: ECX game, EDX player, stack 3
    void (*gold)(P player, P game, I amount);         // 0x12DF0: ESI player, stack 2
};
constexpr U kFile = 0x6FD1B3A4, kCracked = 0x6FD1B39C;   // module strings retail hands on (the source file, "Cracked")

inline U client(const Env& e, P player) { return rd(player, 0) == 0 ? rd(e.pcData(player), 0x9C) : 0; }
inline void nack(const Env& e, P player, I code) { const U c = client(e, player); e.reply(0x2A, code, e.stat(player, 0xE, 0), c, -1, 0); }

// 1.13c D2Game+0xBF990
inline I Transfer_113c(const Env& e, U npcId, P game, P player, U itemId, U mode) {
    const P me = rd8(player, 0x6C) ? e.interactUnit(game, rd(player, 0x68), rd(player, 0x64)) : nullptr;
    P npc = pf(game, 0x1320 + (npcId & 0x7F) * 4);
    while (npc && rd(npc, 0xC) != npcId) npc = pf(npc, 0xE4);
    P item = pf(game, 0x1720 + (itemId & 0x7F) * 4);
    while (item && rd(item, 0xC) != itemId) item = pf(item, 0xE4);
    if (!item) return 1;
    if (!npc || me != npc) {
        const U c = client(e, player);
        uint8_t pkt[16] = { 0x2A, 0, 9 };
        const U s = (U)e.stat(player, 0xE, 0);
        std::memcpy(pkt + 3, &s, 4);
        std::memset(pkt + 7, 0xFF, 4);
        e.send(c, pkt, 0xF);
        return 1;
    }
    if (!e.canAccess(player, item)) { nack(e, player, 0xB); return 3; }
    const uint16_t m = (uint16_t)mode;
    if (rd(item, 0x10) != m || !e.isItem(item)) { nack(e, player, 9); return 3; }
    I lo = e.price(player, item, rd8(game, 0x6D), e.npcRecord(player, game), rd(npc, 4), 1);
    bool ok = true;
    if (e.quality(item) == 1 && e.strcmpi(e.fileName(e.itemFile(item)), kCracked) == 0) ok = false;
    if (e.flagged(item, 0x100, 0x500, kFile)) ok = false;
    if (e.itemType(item) == 7) ok = false;
    if (e.flagged(item, 0x1000000, 0x508, kFile)) ok = false;
    if (e.flagged(item, 0x400000, 0x50C, kFile)) ok = false;
    if (e.socketed(item)) ok = false;
    if (e.questItem(item)) ok = false;
    U cell = 0;                                        // retail's frame slot: storeOf fills it, the low byte later holds the page
    const U store = e.storeOf(game, npc, &cell);
    if (rd8(e.storeState(player, store, game), 4)) ok = false;
    P copy = nullptr;
    if (ok && !e.inStore(npc, game, item)) {
        copy = e.copyItem(game, item, 1);
        if (!copy) { nack(e, player, 9); return 3; }
        e.setFlag(copy, 4);
        const uint8_t pg = (uint8_t)e.page(copy);
        cell = (cell & ~0xFFu) | pg;
        if (pg == 0xFF) { e.freeItem(game, copy); copy = nullptr; }
        else {
            e.setPage(copy, cell);
            U y = e.posY(copy), x = e.posX(copy);     // retail: 0x1130 before 0x1100
            I placed = e.place(kFile, 0x553, game, npc, rd(copy, 0xC), x, y, 1, 1, 0);
            if (!placed && pg == 1) {
                e.setPage(copy, 2);
                y = e.posY(copy); x = e.posX(copy);
                placed = e.place(kFile, 0x559, game, npc, rd(copy, 0xC), x, y, 1, 1, 0);
            }
            if (!placed) e.freeItem(game, copy);       // copy stays set: the other-mode refusal frees it again
            else {
                e.addStore(npc, copy, 1);
                e.setStat(copy, 0x48, e.durMax(copy), 0);
                e.setStat(copy, 0x46, e.qtyMax(copy), 0);
                const I v = e.price(player, copy, rd8(game, 0x6D), e.npcRecord(player, game), rd(npc, 4), 1);
                if (lo > v) lo = v;
            }
        }
    }
    if (m == 0) {
        if (e.hasType(item, 0x16)) e.charge(player, rd(e.itemsTxt((uint16_t)e.itemCode(item, 0)), 8), 1);
        else if (e.hasType(item, 0x12)) {
            const I s = e.stat(item, 0x46, 0);
            if (s >= 0) e.charge(player, rd(e.itemsTxt((uint16_t)e.itemCode(item, 0)), 0xC), s);
        }
        const U c = client(e, player);
        U fl = 0;
        e.mkFlags(&fl, 0x20, 1);
        e.setInvPage(item, e.invPage(item));
        e.sendItem(item, player, c, fl);
        e.resync(player, game, 0);
    } else if (m == 4) {
        if (!e.modeFour(player, item, game)) { nack(e, player, 9); return 1; }
    } else {
        U slot = mode;                                 // retail hands the mode argument's own slot as the out cell
        if (!e.modeOther(game, player, e.bodyLoc(item), 1, &slot)) {
            if (copy) e.freeItem(game, copy);
            return 1;
        }
    }
    e.gold(player, game, lo);
    const U c = client(e, player);
    e.reply(0x2A, 1, e.stat(player, 0xE, 0), c, (I)itemId, 3);
    return 0;
}
}
#endif

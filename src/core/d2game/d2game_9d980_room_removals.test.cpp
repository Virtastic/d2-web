// Host test: g++ -std=c++17 -Isrc/core/d2moo/{compat,include,include/D2Game,squall} (+ an emscripten.h defining EMSCRIPTEN_KEEPALIVE empty)
// Leaving a room tells the client to drop each unit the room holds, in the room's order, but not missiles.
#include "d2game_9d980_room_removals.h"
#include <cassert>
#include <vector>

static std::vector<std::pair<int, uint32_t>> g_sent;
void __fastcall D2GAME_PACKETS_SendPacket0x0A_RemoveObject_6FCC5F80(D2UnitStrc* u, D2ClientStrc*) { g_sent.push_back({ (int)u->dwUnitType, u->dwUnitId }); }

int main() {
    D2UnitStrc mon{}, item{}, missile{}, obj{};
    mon.dwUnitType = UNIT_MONSTER; mon.dwUnitId = 11;
    missile.dwUnitType = UNIT_MISSILE; missile.dwUnitId = 12;
    item.dwUnitType = UNIT_ITEM; item.dwUnitId = 13;
    obj.dwUnitType = UNIT_OBJECT; obj.dwUnitId = 14;
    mon.pRoomNext = &missile; missile.pRoomNext = &item; item.pRoomNext = &obj;
    D2ActiveRoomStrc room{}; room.pUnitFirst = &mon;
    D2ClientStrc client{};
    assert(D2GAME_SendRoomUnitRemovals(&room, &client) == 3);
    assert(g_sent.size() == 3 && g_sent[0].second == 11 && g_sent[1].second == 13 && g_sent[2].second == 14);
    g_sent.clear();
    room.pUnitFirst = nullptr;
    assert(D2GAME_SendRoomUnitRemovals(&room, &client) == 0 && g_sent.empty());
    assert(D2GAME_SendRoomUnitRemovals(nullptr, &client) == 0);
    return 0;
}

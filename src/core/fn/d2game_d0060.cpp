// d2game+0xd0060 sub_D0060: probe for the packet 0x36 (hire merc) handler. Only the nSize != 9 guard is vectored (nSize 9 needs a live game).
#include <emscripten.h>
#include <cstdint>
struct D2GameStrc; struct D2UnitStrc;
int32_t __fastcall D2GAME_PACKETCALLBACK_Rcv0x36_HireMerc_6FC86BD0(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pPacket, int32_t nSize);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_d0060(uint32_t* r) {
    r[4] = (uint32_t)D2GAME_PACKETCALLBACK_Rcv0x36_HireMerc_6FC86BD0((D2GameStrc*)(uintptr_t)r[0], (D2UnitStrc*)(uintptr_t)r[1], (void*)(uintptr_t)r[2], (int32_t)r[3]);
}

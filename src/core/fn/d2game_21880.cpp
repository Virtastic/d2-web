#include "../d2game/d2game_21880.h"
#include "MONSTER/MonsterUnique.h"
#include <emscripten.h>
#include <cstddef>
#include <cstring>

// Native game path: the register-only missile argument is a pointer, never an int.
// 1.13c D2Game+0x21880
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_21880(
    D2GameStrc* game, D2UnitStrc* unit, D2UnitStrc* missile)
{
    d2game_21880(game, unit, missile,
        [](D2GameStrc* g, D2UnitStrc* u, D2UnitStrc* m, int column) {
            sub_6FC6E780(g, u, m, column);
        });
}

namespace {
#pragma pack(push, 1)
struct RetailUnitHeader { int32_t type; uint8_t rest[16]; uint32_t data; };
#pragma pack(pop)
static_assert(offsetof(RetailUnitHeader, type) == 0);
static_assert(offsetof(RetailUnitHeader, data) == 0x14);
static_assert(sizeof(RetailUnitHeader) == 0x18);
uint32_t read32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
}

// FUNCTION protocol: missile, serialized unit, game, hooklog, table (r[5]).
// Decode the x86 header before entering the same typed owner as the native game.
// Only callback resolution differs: the capture pins module tokens instead of host pointers.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_21880(uint32_t* r)
{
    const auto raw = reinterpret_cast<const uint8_t*>(uintptr_t(r[1]));
    D2UnitStrc unit{};
    if (raw) unit.dwUnitType = read32(raw);
    d2game_21880(reinterpret_cast<D2GameStrc*>(uintptr_t(r[2])), raw ? &unit : nullptr,
        reinterpret_cast<D2UnitStrc*>(uintptr_t(r[0])),
        [r, raw](D2GameStrc* game, D2UnitStrc* u, D2UnitStrc* missile, int column) {
            const uint32_t address = read32(raw + offsetof(RetailUnitHeader, data));
            if (!address) return;
            const auto data = reinterpret_cast<const uint8_t*>(uintptr_t(address));
            const auto table = reinterpret_cast<const uint8_t*>(uintptr_t(r[5]));
            auto log = reinterpret_cast<uint32_t*>(uintptr_t(r[3]));
            d2game_268e0_walk(game, u, missile, column, data + 0x1c,
                int((data[0x16] >> 3) & 1),
                [table, log](uint8_t id, int col, D2GameStrc* g, D2UnitStrc* target, int unique) {
                    const uint32_t callback = table ? read32(table + uint32_t(id) * 24 + col * 4) : 0;
                    if (!callback) return;
                    // Normalized module token, RVA 0x79100. The descriptor
                    // hooks this callback with ECX/EDX and two stack arguments.
                    if (callback != 0xb2079100u || !log) return;
                    const uint32_t n = log[0]++;
                    if (n >= 9) return;
                    auto entry = log + 1 + n * 10;
                    entry[0] = 0x79100; entry[1] = 0;
                    entry[2] = uint32_t(reinterpret_cast<uintptr_t>(g));
                    entry[3] = uint32_t(reinterpret_cast<uintptr_t>(target));
                    entry[4] = 0; entry[5] = 0; entry[6] = id;
                    entry[7] = uint32_t(unique); entry[8] = 0; entry[9] = 0;
                });
        });
    r[4] = 0;
}

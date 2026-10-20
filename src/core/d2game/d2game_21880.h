#pragma once
#include <cstdint>
struct D2GameStrc;
struct D2UnitStrc;

// Shared guard for native units and FUNCTION-mode serialized x86 unit headers.
// The callback receives (game, unit, missile, column), preserving pointer widths.
template<class Game, class Unit, class Missile, class Type, class Dispatch>
void d2game_21880_guard(Game game, Unit unit, Missile missile, Type type, Dispatch dispatch)
{
    if (unit && type(unit) == 1 && missile) dispatch(game, unit, missile, 5);
}
extern "C" void d2game_21880(D2GameStrc* game, D2UnitStrc* unit, D2UnitStrc* missile);

// The table is supplied by native module state or by the pinned FUNCTION fixture.
// IDs remain unsigned bytes; a zero first ID gates all nine slots, but later zeroes do not.
template<class Game, class Unit, class Missile, class Callback>
void d2game_268e0_walk(Game game, Unit unit, Missile missile, int column,
                      const uint8_t* ids, int unique, Callback callback)
{
    if (!ids || !ids[0]) return;
    for (int i = 0; i < 9; ++i)
        callback(ids[i], column, game, column == 5 ? missile : unit, unique);
}

template<class Unit, class Dispatch>
void d2game_21880(D2GameStrc* game, Unit* unit, D2UnitStrc* missile, Dispatch dispatch)
{
    d2game_21880_guard(game, unit, missile,
        [](const Unit* u) { return u->dwUnitType; }, dispatch);
}

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

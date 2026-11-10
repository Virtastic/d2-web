// The Skills row's client start function (its cltstfunc column) for a unit the joined-game client
// keeps as an engine unit. One file per function; the table is realm_skill_start_table.cpp.
#pragma once
#include <cstdint>
struct D2UnitStrc;

// 1 when the hand weapon has something to shoot or throw (non-players always do).
int d2client_realm_a0a30(D2UnitStrc* unit);
int d2client_realm_a0ea0(D2UnitStrc* unit, int skillId, int level);   // start function 1
int d2client_realm_a0e70(D2UnitStrc* unit, int skillId, int level);   // start function 2
int d2client_realm_14f30(D2UnitStrc* unit, int skillId, int level);   // start function 11
int d2client_realm_14f10(D2UnitStrc* unit, int skillId, int level);   // start function 12
int d2client_realm_57000(D2UnitStrc* unit, int skillId, int level);   // start function 39

int d2client_realm_a0730(D2UnitStrc* unit, int skillId, int level);   // start function 4
int d2client_realm_406f0(D2UnitStrc* unit, int skillId, int level);   // start function 6
int d2client_realm_401a0(D2UnitStrc* unit, int skillId, int level);   // start function 7
int d2client_realm_40610(D2UnitStrc* unit, int skillId, int level);   // start function 10
int d2client_realm_569c0(D2UnitStrc* unit, int skillId, int level);   // start function 37
int d2client_realm_569a0(D2UnitStrc* unit, int skillId, int level);   // start function 38
int d2client_realm_56930(D2UnitStrc* unit, int skillId, int level);   // start function 40
int d2client_realm_566e0(D2UnitStrc* unit, int skillId, int level);   // start function 41

// Runs start function `fn` (0, out of the table or not ported: nothing runs, 1). 0 = the start refused.
int d2client_realm_skill_start(int fn, D2UnitStrc* unit, int skillId, int level);
// Whether the table's entry `fn` is ported here.
int d2client_realm_skill_start_ported(int fn);

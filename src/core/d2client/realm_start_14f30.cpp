// 1.13c D2Client+0x14F30
// Client start function 11 (the bow and crossbow skills): starts when there is ammunition.
#include "realm_skill_start.h"
int d2client_realm_14f30(D2UnitStrc* unit, int, int) { return d2client_realm_a0a30(unit) != 0; }

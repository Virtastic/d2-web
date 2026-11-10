// 1.13c D2Client+0x15500
// Client start function 13 (Strafe): the start refuses when the hand weapon has nothing to shoot.
// The retail function also stores the arrow count and first target on the used skill; the do step
// that reads them is not ported here yet.
#include "realm_skill_start.h"
int d2client_realm_15500(D2UnitStrc* unit, int, int) { return d2client_realm_a0a30(unit) != 0; }

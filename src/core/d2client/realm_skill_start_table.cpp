// 1.13c D2Client+0xA1CF0
// The start-function step of the client skill start: the Skills row's cltstfunc indexes a table
// of 53 functions; an index outside it or an empty entry runs nothing and the start goes on.
// Ported entries: 1, 2, 11, 12, 39. The others report "not ported" and let the start go on, which
// is what the client did before this table existed here.
#include "realm_skill_start.h"
int d2client_realm_skill_start_ported(int fn) {
    return fn == 1 || fn == 2 || fn == 11 || fn == 12 || fn == 39;
}
int d2client_realm_skill_start(int fn, D2UnitStrc* unit, int skillId, int level) {
    switch (fn) {
    case 1: return d2client_realm_a0ea0(unit, skillId, level);
    case 2: return d2client_realm_a0e70(unit, skillId, level);
    case 11: return d2client_realm_14f30(unit, skillId, level);
    case 12: return d2client_realm_14f10(unit, skillId, level);
    case 39: return d2client_realm_57000(unit, skillId, level);
    default: return 1;
    }
}

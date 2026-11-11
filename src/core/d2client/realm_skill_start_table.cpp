// 1.13c D2Client+0xA1CF0
// The start-function step of the client skill start: the Skills row's cltstfunc indexes a table
// of 53 functions; an index outside it or an empty entry runs nothing and the start goes on.
// Ported entries: 1, 2, 4, 6, 7, 10, 11, 12, 13, 14, 37, 38, 39, 40, 41; entry 0 is empty and runs nothing. The others report "not ported" and let the start go on, which
// is what the client did before this table existed here.
#include "realm_skill_start.h"
#include "realm_skill_do.h"
// The client callbacks the start functions that search for targets use (13 and 14).
static const RealmSkillDoHost* g_startHost = nullptr;
void d2client_realm_set_start_host(const RealmSkillDoHost* host) { g_startHost = host; }
const RealmSkillDoHost* d2client_realm_start_host() { return g_startHost; }
int d2client_realm_skill_start_ported(int fn) {
    switch (fn) {
    case 0: case 1: case 2: case 4: case 6: case 7: case 10: case 11: case 12: case 13: case 14: case 37: case 38: case 39: case 40: case 41: return 1;
    default: return 0;
    }
}
int d2client_realm_skill_start(int fn, D2UnitStrc* unit, int skillId, int level) {
    switch (fn) {
    case 1: return d2client_realm_a0ea0(unit, skillId, level);
    case 2: return d2client_realm_a0e70(unit, skillId, level);
    case 11: return d2client_realm_14f30(unit, skillId, level);
    case 12: return d2client_realm_14f10(unit, skillId, level);
    case 13: return d2client_realm_15500(unit, skillId, level);
    case 14: return d2client_realm_15350(unit, skillId, level);
    case 39: return d2client_realm_57000(unit, skillId, level);
    case 4: return d2client_realm_a0730(unit, skillId, level);
    case 6: return d2client_realm_406f0(unit, skillId, level);
    case 7: return d2client_realm_401a0(unit, skillId, level);
    case 10: return d2client_realm_40610(unit, skillId, level);
    case 37: return d2client_realm_569c0(unit, skillId, level);
    case 38: return d2client_realm_569a0(unit, skillId, level);
    case 40: return d2client_realm_56930(unit, skillId, level);
    case 41: return d2client_realm_566e0(unit, skillId, level);
    default: return 1;
    }
}

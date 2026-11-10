// 1.13c D2Client+0xA2C90
// The do-function step of the generic do: the Skills row's cltdofunc indexes the table at
// D2Client+0xDEA48. Ported entries: 16 (Jab), 17 (Multiple Shot), 18 (Guided Arrow).
#include "realm_skill_do.h"
int d2client_realm_skill_do_ported(int fn) {
    switch (fn) {
    case 16: case 17: case 18: return 1;
    default: return 0;
    }
}
int d2client_realm_skill_do(int fn, D2UnitStrc* unit, int skillId, int level, const RealmSkillDoHost& host) {
    switch (fn) {
    case 16: return d2client_realm_14f40(unit, skillId, level, host);
    case 17: return d2client_realm_15da0(unit, skillId, level, host);
    case 18: return d2client_realm_15c40(unit, skillId, level, host);
    default: return 0;
    }
}

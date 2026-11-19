// 1.13c D2Client+0x2A0B0 as called by start functions 6 and 10: the engine unit keeps its path target current, so nothing runs here.
// The call is a seam point (D2_CALL) so a probe can bind it to the recorder.
#include "realm_skill_start.h"
void d2client_realm_2a0b0(D2UnitStrc*) {}

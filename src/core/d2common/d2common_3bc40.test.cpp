// Host test: g++ -o /tmp/t src/core/d2common/d2common_3bc40.test.cpp && /tmp/t
// Mirrors D2Common+0x3BC40 (DUNGEON_GetPortalFlagFromLevelId): 1<<index of level in the portal table, else 0.
#include <cassert>
static int tbl[] = {1, 40, 75, 103, 109};
static int flag(int id) { for (int i = 0; i < 5; ++i) if (id == tbl[i]) return 1 << i; return 0; }
int main() { assert(flag(1) == 1 && flag(75) == 4 && flag(109) == 16 && flag(2) == 0); return 0; }

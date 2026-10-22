// Prior art used: D2MOO DRLGMAZE_BuildBasicMaze (1.10f); 1.13c 0x10160 is the same room-growth loop
// (target count = per-difficulty table, x3 staff tomb, x2 boss tomb; random room, 2-bit direction).
#include <cstdint>
#include "D2DataTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgMaze.h"

// 1.13c D2Common+0x10160
extern "C" void d2_fn_d2common_10160(D2DrlgLevelStrc* level)
{
    DRLGMAZE_BuildBasicMaze(level);
}

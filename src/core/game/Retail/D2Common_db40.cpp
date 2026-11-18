// Path recache around the centred fixed-point coordinates, moved from the stand-alone d2common batch into the game tree.
#include <cstdint>
#include "Path/Step.h"

// 1.13c D2Common+0x0DB40
void __stdcall d2common_recache_center_db40(D2DynamicPathStrc* path)
{
    PATH_RecacheRoomAtCoordIfNeeded(path, nullptr,
        PATH_FP16FitToCenter(path->tGameCoords.dwPrecisionX),
        PATH_FP16FitToCenter(path->tGameCoords.dwPrecisionY));
}

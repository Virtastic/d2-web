// Callees of 1.13c D2Common+0x7C270 (room post-init). The port in d2common_batch_b62.cpp calls these
// two seams; they were weak stubs under the wrong names (sub_B8400, sub_5BF70) and never linked, so the
// room's post-init skipped both calls. The seams now reach the d2moo implementations of the same RVAs:
//   d2common_seam_68400 -> 0x68400  (DRLGROOMTILE_LoadDT1FilesForRoom)
//   d2common_seam_bf70  -> 0x0BF70  (DRLGPRESET_SpawnHardcodedPresetUnits)
#include <cstdint>

#include "Drlg/D2DrlgPreset.h"
#include "Drlg/D2DrlgRoomTile.h"

extern "C" void d2common_seam_68400(void* pRoom)
{
	DRLGROOMTILE_LoadDT1FilesForRoom(static_cast<D2DrlgRoomStrc*>(pRoom));
}

extern "C" void d2common_seam_bf70(void* pRoom)
{
	DRLGPRESET_SpawnHardcodedPresetUnits(static_cast<D2DrlgRoomStrc*>(pRoom));
}

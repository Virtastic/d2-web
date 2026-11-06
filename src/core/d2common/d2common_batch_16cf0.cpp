// Prior art used: D2MOO DrlgOutRoom / DrlgDrlgGrid / DrlgDrlgVer.
// Retail's ESI points directly at the outdoor data, not its owning room.
#include "Drlg/D2DrlgOutRoom.h"
#include "Drlg/D2DrlgDrlgVer.h"
#include "Fog.h"
#include "d2common_4a4d0.h"

extern "C" {
#define W __attribute__((weak))
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
}

// D2Common 0x4A4D0 lives in d2common_4a4d0.cpp; this wrapper hands it the Fog free and the D2MOO vertex release.
namespace {
void F4a4d0(uint32_t pool, uint32_t p, uint32_t line) { FOG_FreePool(reinterpret_cast<void*>(uintptr_t(pool)), reinterpret_cast<void*>(uintptr_t(p)), "..\\Source\\D2Common\\DRLG\\DrlgGrid.cpp", int(line), 0); }
void V4a4d0(uint32_t pool, uint32_t list) { DRLGVER_FreeVertices(reinterpret_cast<void*>(uintptr_t(pool)), reinterpret_cast<D2DrlgVertexStrc**>(uintptr_t(list))); }
}
extern "C" void D2Common_4A4D0_Outdoor(D2DrlgOutdoorRoomStrc* outdoor, void* mempool)
{
    static const D2Common4a4d0Env env = { F4a4d0, V4a4d0 };
    D2Common_4A4D0_With(uint32_t(uintptr_t(outdoor)), uint32_t(uintptr_t(mempool)), &env);
}


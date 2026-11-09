#pragma once
// The light sources a client that holds its own units (the online client) hands to d2_client_lightmap_compose
// (monspawn_check.cpp). One definition, so the defining file and the realm caller agree on the record and on
// the return type. Include after the D2 type headers (D2UnitStrc, D2DrlgActStrc, D2ActiveRoomStrc).
#include <cstdint>

struct D2SessionLightmap;

// A monster entry (unit set) takes its position and radius from the unit; an object entry carries its
// position, its Objects.txt Lit radius and the room it stands in.
struct D2ClientLightIn { D2UnitStrc* unit; int32_t type, classId, x, y, radius; uint32_t guid; D2ActiveRoomStrc* room; };

extern "C" const D2SessionLightmap* d2_client_lightmap_compose(D2UnitStrc* player, D2DrlgActStrc* act,
    const D2ClientLightIn* in, int n, int ambientOverride, int occludeUnits);

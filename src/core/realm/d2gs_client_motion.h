#pragma once

#include <cstdint>
#include <Units/Units.h>

extern "C" {
bool d2_realm_client_motion_attach(D2UnitStrc* unit, int x, int y);
int d2_realm_client_motion_set_mode(D2UnitStrc* unit, uint32_t mode);
int d2_realm_client_motion_set_target(D2UnitStrc* unit, int x, int y);
int d2_realm_client_motion_advance_unit(D2UnitStrc* unit);
void d2_realm_client_motion_detach(D2UnitStrc* unit);
int d2_realm_client_motion_apply_player_move(D2UnitStrc* unit, uint8_t action, uint8_t opaque,
        uint16_t targetX, uint16_t targetY);
}

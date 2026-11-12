#pragma once
#include <cstdint>
struct D2UnitStrc;
struct D2SndUnit { uint32_t type, id, cls; const D2UnitStrc* unit; };
void d2client_sound_unit_sync(const D2SndUnit* u, uint32_t n);   // sound_unit_refs.cpp (1.13c D2Client+0x22B20)
void d2client_sound_unit_reset();
int32_t d2client_object_slot_row(uint32_t cls, uint32_t slot);   // 0xF6C58[cls][slot]
void d2client_sound_unit_submit(int32_t row);                     // sound_pump.cpp: a unit's 0x88A70 submit (replay only)
// session_unit_sound.cpp: the client's unit sound calls on the session units
void d2client_session_unit_mode(const D2UnitStrc* u, uint32_t mode, int32_t roll, bool hero);   // 0x24490 (bUseGiven = 1)
void d2client_session_objects(const D2SndUnit* u, uint32_t n);                                  // 0x237E0 object branch, each held object
void d2client_session_player_packets();                                                         // the frame's player 0x0D packets -> 0x24490
void d2client_session_unit_sound_reset();

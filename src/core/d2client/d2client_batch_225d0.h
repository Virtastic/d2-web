#pragma once
#include <cstdint>
namespace d2client_batch_225d0 {
using U=uint32_t; using I=int32_t; using B=unsigned char;
// Bind these references to the client's live globals. Serialized pointers are
// resolved by the caller's address space, never cast to wasm/native pointers.
// resolve(0) must return nullptr. Unit is the actual serialized UnitAny bytes:
// +0x78 is its serialized sound-list head, not a separately maintained list.
// entries binds DAT_6fbcbf30: list node +0 data address, +4 next address;
// sound data +0x28 is the channel address. The caller supplies the 22A70,
// 22490, 221C0 and 223D0 adapters with the original unit operand intact.
// This is the Stage A native interface; browser/game-loop wiring is still owed.
using Unit=B*;
struct State {
 B*& table; I& count; I& enabled; U& entries; Unit& player;
 U& tick; U& lastRandom; U& lastSpecial; U& nextSpecial; U& clock; I& previousLevel;
 U* visited; const I* const* groups;
 B* (*resolve)(U);
 I (*mode)(); B* (*config)(Unit); U (*index)(Unit); B* (*indexedConfig)(U);
 void (*flush)(U); void (*play)(Unit,U,I,I,I); void (*remove)(Unit,U,I);
 void (*setTime)(Unit,U); U (*getTime)(Unit); U (*setFlags)(Unit,U);
 I (*busy)(Unit); I (*blocked)(Unit); I (*action)(Unit); U (*now)();
 I (*active)(B*,U);
 void* (*room)(Unit); I (*level)(void*); I levelCount;
 I (*levelBlocked)(); I (*playing)(); void (*levelSound)(Unit,U);
};
U sub_225D0(State&,Unit,I);
void sub_22B20(State&,Unit,I);
void sub_22C10(State&,Unit,U);
void sub_22CA0(State&,Unit,I);
void sub_22D60(State&,Unit);
void sub_22DA0(State&,Unit);
void sub_22EB0(State&,Unit);
void sub_23070(State&,Unit);
void sub_23120(State&,Unit,I);
void sub_23170(State&,Unit,I);
void sub_23A30(State&,Unit,I,B*);
void sub_23E20(State&);
}

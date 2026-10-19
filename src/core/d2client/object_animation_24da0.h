#pragma once
#include <cstdint>

// Stage A interface for serialized 1.13c images, NOT native D2UnitStrc.
// Every unit, object-data, objects.txt and path buffer uses the retail x86 offsets.
// resolve(0) must return nullptr; resolve maps embedded guest addresses only.
// A native D2UnitStrc has wider pointer fields and must never be passed here or
// to these callbacks without an explicit field-by-field adapter. No narrowing
// cast from a native pointer to a guest address is part of this interface.
//
// All callbacks are required on the branch that calls them. They implement the
// callee named below, including its writes; this is not a fake/no-op fallback.
// The owner argument to the missile functions is native and is passed unchanged
// to stat/spawn. spawn's 23 scalar words omit the two pointer slots: word 1 is
// owner and word 2 is source, supplied as separate native pointers. An adapter
// must construct the callee's native missile representation, not memcpy it.
// D2MOO supplies the native UNITS and STATLIST operations, but they cannot
// consume serialized blobs directly. Globals are pinned for one retail call. No module
// singleton is created here: the client frame dispatcher and deep client
// callees remain separate function, so this interface is not live wiring.
namespace object24 {
// Serialized retail objects retain 32-bit addresses. resolve translates them to native
// pointers; callbacks take native pointers without narrowing them to retail integers.
struct Services {
 void* (*resolve)(uint32_t);
 uint32_t (*tick)();
 uint32_t (*random)(void* seed,int32_t upper,int32_t lower); // +2DC0
 void (*mode)(void*,uint32_t); // #11090 UNITS_ChangeAnimMode; serialized adapter required
 uint32_t (*refresh)(void*,uint32_t); // +6A220
 void (*update)(void*); // ordinal 10595
 void (*activate)(void*); // #10408 UNITS_FreeCollisionPath; serialized adapter required
 void (*flags)(void*,uint32_t,uint32_t); // +13D0
 void (*animate)(void*,uint16_t,uint32_t,uint32_t,uint32_t); // +24920
 int32_t (*periodOfDay)(void* act,int32_t* minute); // D2Common #11044
 uint32_t act; // global +11C3B8: ACT, not a player unit; guest address
 uint32_t localPlayer; // global +11BBFC: guest player-unit address
 int32_t (*distance)(void*,void*); // #10522 UNITS_GetDistanceToOtherUnit; image operands
 uint32_t (*message)(uint32_t tableRva,uint32_t bucket,uint32_t id,uint32_t type); // +A6EF0
 uint32_t (*advance)(void*); // +237E0
 uint32_t (*dispatch)(void*); // +24680, ESI unit
 void (*sound)(uint32_t,uint32_t); // +16EB0
 void (*effect)(void*,uint32_t); // +69660, EDI unit
 void (*setMode)(void*,uint32_t,uint32_t,uint32_t,uint32_t); // +6B300
 int32_t (*pathX)(void*); int32_t (*pathY)(void*); // #10867/#10750, serialized dynamic paths
 int32_t (*stat)(void* owner,uint32_t stat,uint32_t layer); // D2Common #10973, STAT_LEVEL=12
 void (*spawn)(const uint32_t*,void*,void*); // +B30F0; pointer fields supplied natively
 void (*fatal)(uint32_t); // 10265 ->10024 ->exit(-1)
};
}

extern "C" {
uint8_t d2_client_24da0(void*, object24::Services*);
uint8_t d2_client_24e30(void*, object24::Services*);
uint8_t d2_client_24fa0(void*, object24::Services*);
void d2_client_251e0(void*, object24::Services*);
uint32_t d2_client_25280(void*, uint32_t suppress, object24::Services*);
void d2_client_25400(void*, object24::Services*);
void d2_client_25660(void*, object24::Services*);
uint8_t d2_client_258f0(void*, object24::Services*);
void d2_client_25b40(void*, object24::Services*);
void d2_client_25b70(void*, object24::Services*);
void d2_client_25dc0(void* sourceImage, void* nativeOwner, object24::Services*);
void d2_client_25f10(void* sourceImage, void* nativeOwner, object24::Services*);
}

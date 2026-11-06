// D2Common 0x43580: outdoor level init (EAX level). Callees arrive through Env; `act` takes the retail RVA of the
// act-specific callee it stands for (0x7F3A0, 0x85E20, 0x176F0, 0x854C0, 0x856C0, 0x84DA0, 0x861A0, 0x41CA0).
#pragma once
#include <cstdint>
struct D2Common43580Env {
	void (*gridInit)(uint32_t pool, uint32_t grid, int32_t w, int32_t h);     // 0x36950: ECX pool, EBX grid+4, stack w h
	const int32_t* actStart;                                                   // the six-entry first-level-of-act table
	void (*placeRooms)(uint32_t origin, uint32_t pool, uint32_t list, uint32_t n);   // 0x1B6E0: ECX level+0x1C, EAX 0, stack
	void (*clipRooms)(uint32_t pool, uint32_t list);                           // 0x42070: EBX pool, EDI outdoor+0x64
	void (*act)(uint32_t rva, uint32_t level);
	void (*build)(uint32_t level);                                             // 0x43190: stack level
};
extern "C" void D2Common_43580_With(uint32_t level, const D2Common43580Env* e);

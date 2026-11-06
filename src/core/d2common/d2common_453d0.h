// D2Common 0x453D0: place a room's linked-level preset (stdcall: p1, room, idx; ret 0xC). Callees arrive through Env.
#pragma once
#include <cstdint>
struct D2Common453d0Env {
	uint32_t (*linkTable)(uint32_t drlg, uint32_t levelId);   // 0x44CE0: EAX drlg, EDX id; 8 linked level ids
	uint32_t (*newLevel)(uint32_t drlg, uint32_t levelId);    // 0x2D9B0: ECX drlg, EDX id; the level node
	uint32_t (*fileTable)(uint32_t drlg, uint32_t levelId);   // 0x19950: EAX drlg, EDX id; 8 preset file ids
	void (*initLevel)(uint32_t level);                         // 0x2E360: stack level
	uint32_t (*place)(uint32_t ecx, uint32_t eax, uint32_t room, uint32_t p1, uint32_t idx, uint32_t v);   // 0x452C0
	void (*fatal)(uint32_t line);                              // 10265/10024 then exit(-1)
};
extern "C" void D2Common_453D0_With(uint32_t p1, uint32_t room, uint32_t idx, const D2Common453d0Env* e);

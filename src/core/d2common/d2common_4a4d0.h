// D2Common 0x4A4D0: outdoor grid release. Callees arrive through the Env seams.
#pragma once
#include <cstdint>
struct D2Common4a4d0Env {
	void (*free)(uint32_t pool, uint32_t block, uint32_t line);   // 0x9228 (Fog 10046): ECX pool, EDX block, stack file, line, 0
	void (*freeVertices)(uint32_t pool, uint32_t list);            // 0x1B3D0: EBX pool, EDI &vertex list
};
extern "C" void D2Common_4A4D0_With(uint32_t outdoor, uint32_t pool, const D2Common4a4d0Env* e);

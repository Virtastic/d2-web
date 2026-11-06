// D2Common 0x43190: walk the outdoor cell grids of a level and add a preset map or an 8x8 room per cell. Callees via Env.
#pragma once
#include <cstdint>
struct D2Common43190Env {
	uint32_t (*allocMap)(uint32_t seed, uint32_t level, uint32_t preset, uint32_t* coords);   // 0xBC60: EBX seed, stack level preset &coords
	void (*buildArea)(uint32_t level, uint32_t map, uint32_t flags, uint32_t zero);          // 0xC690: stack, ret 0x10
	void (*addRoom)(uint32_t flags, uint32_t level, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
		uint32_t packed, uint32_t ex, uint32_t mask);                                          // 0x4A5C0: EAX flags, ECX level, 7 stack words
};
extern "C" void D2Common_43190_With(uint32_t level, const D2Common43190Env* e);

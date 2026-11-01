// Fog allocator seam for D2Common ports that allocate/free through Fog ordinals 10042/10045/10046 (retail: import thunks at D2Common
// 0x9222 alloc, 0x9228 free, 0x9264 string alloc). A seam, not a wrapper: retail's pool/size/file/line arguments are the callee's,
// the port passes what the 1.13c call site passes. The FN probes supply recorders that log the call as the capture's hook does.
#pragma once
#include <cstdint>
struct D2CommonFogEnv {
	uint32_t (*alloc)(uint32_t pool, uint32_t size);   // 0x9222 (ecx = pool, edx = size, stack = file, line, 0)
	void (*free)(uint32_t pool, uint32_t ptr);         // 0x9228 (ecx = pool, edx = ptr, stack = file, line, 0)
	uint32_t (*strAlloc)(uint32_t size);               // 0x9264 (ecx = size, edx = file, stack = line, 0)
};

// Retail-layout (32-bit) field access for FN ports: struct addresses arrive as 32-bit values, fields are read at retail offsets.
namespace d2raw {
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

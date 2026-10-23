// 1.13c D2Common implementation batch: +0x9DE0.
// Deep copy of a preset record list: an 8-byte header {count, copy} plus count*3 dwords of records,
// both allocations taken from the caller's memory pool. The pool and the size are the two register
// arguments of retail ordinal 10045, so the allocation seam carries them explicitly.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t A(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// Ordinal 10045 allocation seam: (pool, size). Zero until the environment installs it.
struct D2Common9DE0Env
{
	uint32_t (*alloc)(uint32_t nPool, uint32_t nSize);
};

// 1.13c D2Common+0x09DE0 (#10496): copy a preset list.
// The header count is copied first, then count*3 dwords from the source list pointer. The record
// count is masked with 0x3FFFFFFF, which only limits a hostile count; the byte copy is exact.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t DRLGPRESET_CopyMapList(uint32_t nPool, uint32_t pSrc, const D2Common9DE0Env* pEnv)
{
	if (!pSrc || !pEnv || !pEnv->alloc)
		return 0;

	uint8_t* src = P(pSrc);
	uint8_t* dst = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(pEnv->alloc(nPool, 8)));
	if (!dst)
		return 0;

	const uint32_t nRecords = W(src, 0) & 0x3FFFFFFF;
	const uint32_t nDwords = nRecords * 3;
	W(dst, 0) = W(src, 0); // header keeps the unmasked count
	W(dst, 4) = 0;

	uint8_t* copy = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(pEnv->alloc(nPool, nDwords * 4)));
	W(dst, 4) = A(copy);
	if (copy && nDwords)
		std::memcpy(copy, P(W(src, 4)), static_cast<size_t>(nDwords) * 4);

	return A(dst);
}

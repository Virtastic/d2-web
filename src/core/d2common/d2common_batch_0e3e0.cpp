// 1.13c D2Common+0x0E3E0 (73 bytes): reads table[ *(*(p+0x1b4)+0x450) ] of the block at p+0x14, == 1.
// Fatal paths (null p -> Fog msg 0x1c; null p+0x14 -> msg 0x1e; then _exit(-1)) are modelled as returns of 0.
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_0e3e0(uint32_t p)
{
	if (!p) return 0;
	const uint32_t blk = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(p + 0x14));
	if (!blk) return 0;
	const uint32_t q = *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(p + 0x1b4));
	const uint8_t idx = *reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(q + 0x450));
	return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(blk + 4 + idx * 4)) == 1;
}

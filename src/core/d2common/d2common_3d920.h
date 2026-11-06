// D2Common 0x3D920: ESI = level, stack arg = path index, ret 4. The node allocator arrives through the seam.
#pragma once
#include <cstdint>
// 0x9222 (pool alloc thunk): ECX = pool, EDX = size, stack (file, line, 0); returns the block.
typedef uint32_t (*D2Common3D920Alloc)(uint32_t pool, uint32_t size, uint32_t line);
extern "C" void D2Common_3D920_With(uint32_t level, int32_t idx, D2Common3D920Alloc alloc);

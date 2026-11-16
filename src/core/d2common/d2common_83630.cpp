// 1.13c D2Common+0x83630: preset-node sweep. EAX = rect {x,y,w,h} in tiles, ECX = row offset, EDX = column offset; stack (ret 0xC):
// pOwner (its pointee goes to the callee in ESI), nArg1 (handed to the callee in ECX), pCtx. Every node of the list at
// [[pCtx+0x50]+0x54] (next at +0xC) whose position (x +8, y +0x18) lies strictly inside the rect scaled by 5 is offered to
// 0x44480 (ESI = *pOwner, five stack words: node+0x14, node+4, node+0, x - rx*5 + col*5, y - ry*5 + row*5). The callee is a seam.
#include <cstdint>
#include <cstring>
namespace { inline uint32_t rd(uintptr_t a, uint32_t o) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(a + o), 4); return v; } }
extern "C" void D2Common_83630_Sweep(uintptr_t rect, int32_t row, int32_t col, uintptr_t pOwner, uint32_t nArg1, uintptr_t pCtx,
                                     void (*f44480)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t))
{
	(void)nArg1;   // retail passes it on in ECX; the callee seam does not take it
	const int32_t x5 = static_cast<int32_t>(rd(rect, 0)) * 5, y5 = static_cast<int32_t>(rd(rect, 4)) * 5;
	const int32_t w5 = static_cast<int32_t>(rd(rect, 8)) * 5, h5 = static_cast<int32_t>(rd(rect, 0xC)) * 5;
	const int32_t row5 = row * 5, col5 = col * 5;
	for (uint32_t n = rd(rd(pCtx, 0x50), 0x54); n != 0; n = rd(n, 0xC)) {
		const int32_t nx = static_cast<int32_t>(rd(n, 8)), ny = static_cast<int32_t>(rd(n, 0x18));
		if (nx <= x5 || ny <= y5 || nx >= w5 + x5 || ny >= h5 + y5) continue;
		f44480(rd(pOwner, 0), rd(n, 0x14), rd(n, 4), rd(n, 0), nx - x5 + col5, ny - y5 + row5);
	}
}

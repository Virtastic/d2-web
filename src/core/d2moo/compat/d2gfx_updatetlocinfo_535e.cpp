// 1.13c D2Gfx+0x0535E: VS2003 CRT ___updatetlocinfo -- __lock(0xC); p = ___updatetlocinfo_lk() (+0x529D);
// FUN_6fa85390 (SEH epilogue funclet, _unlock(0xC) via +0x19BC); return p. Lock/lk/unlock are other rows: seams.
#include <cstdint>
typedef void* (*D2GfxLkFn)(void);
typedef void (*D2GfxLockFn)(uint32_t);
extern "C" void* D2Gfx_535E_UpdateTlocInfo(D2GfxLockFn lock, D2GfxLkFn lk, D2GfxLockFn unlock)
{
	lock(0xC);
	void* p = lk();
	unlock(0xC);
	return p;
}

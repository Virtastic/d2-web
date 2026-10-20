// 1.13c D2Common+0x03401: VS2003 CRT ___updatetlocinfo -- __lock(0xC); p = ___updatetlocinfo_lk();
// FUN_6fd53433 (the SEH epilogue funclet, _unlock(0xC)); return p. The lk body (0x03340) and the
// lock (__lock, 0x02EF9 region) are separate rows, so they are caller-supplied seams, as for 0x44BC.
#include <cstdint>
#include <emscripten.h>

typedef void* (*D2CommonLkFn)(void);  // 0x03340 ___updatetlocinfo_lk
typedef void (*D2CommonLockFn)(uint32_t);

extern "C" void D2Common_2E44_Unlock(uint32_t nIdx);

extern "C" EMSCRIPTEN_KEEPALIVE void* D2Common_3401_UpdateTlocInfo(D2CommonLockFn lock, D2CommonLkFn lk)
{
	lock(0xC);
	void* p = lk();
	D2Common_2E44_Unlock(0xC);
	return p;
}

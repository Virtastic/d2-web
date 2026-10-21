// D2Common 1.13c implementation (0x12C90): open the profiler/log file once.
#include <cstdint>
#include <emscripten.h>

struct D2CommonLogOpenEnv {
	uint32_t* pLogFile;                                   // DAT_6fdf1470
	void (*initCritSec)();                                // InitializeCriticalSection(&DAT_6fdf13e0)
	void (*ord10050)();
	uint32_t (*fopenFn)(const char* szPath, const char* szMode); // 0x2043
	void (*leaveCritSec)();
	void (*deleteCritSec)();
	void (*sub12C00)(int32_t, int32_t, int32_t, uint32_t, const char*, int32_t, const char*);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_12c90(const char* szPath, const D2CommonLogOpenEnv* env) { // 1.13c D2Common+0x12C90
	if (*env->pLogFile != 0) return 1;
	env->initCritSec();
	env->ord10050();
	*env->pLogFile = env->fopenFn(szPath, "wt");
	env->leaveCritSec();
	if (*env->pLogFile == 0) {
		env->deleteCritSec();
		return 0;
	}
	env->sub12C00(1, 8, 2, 0x80000, "..\\Source\\D2Common\\Logging\\Logging.cpp", 0xf5, "Log Started");
	return 1;
}

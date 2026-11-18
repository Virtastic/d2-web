// Logging-subsystem close gate, moved from the stand-alone d2common tiny batch into the game tree.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Common+0x70F80: always returns 1; when its second argument is zero it closes the logging
// subsystem first (0x12AA0 __log_close) and otherwise touches nothing. The first decompiled argument
// is unread. 0x12AA0 is still UNMAPPED, so it stays a caller-supplied seam, as in
// d2common_batch_233b0.cpp.
typedef void (*D2CommonLogCloseFn)(void);  // 0x12AA0 __log_close

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_70F80(uint32_t unused, int32_t nFlag,
	D2CommonLogCloseFn log_close)
{
	(void)unused;
	if (nFlag == 0)
		log_close();
	return 1;
}

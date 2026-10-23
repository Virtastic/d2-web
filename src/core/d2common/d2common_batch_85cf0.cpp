// 1.13c D2Common batch: +0x85CF0, +0x85DF0. Seams for UNMAPPED callees.
#include <cstdint>
#include <emscripten.h>

typedef void (*D2CommonProcessIdsFn)(const uint32_t* pIds, int32_t nCount, int32_t nPayload);
typedef void (*D2CommonCall2Fn)(int32_t nA, int32_t nB);

// 1.13c D2Common+0x85CF0: 0x85A80 over {0x18C,0x18D}, payload 0.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85CF0_ProcessIds(D2CommonProcessIdsFn fnProcess)
{
	const uint32_t aIds[2] = {0x18C, 0x18D};
	fnProcess(aIds, 2, 0);
}

// 1.13c D2Common+0x85DF0: 0x43100(a, 0x16C) for a = 2, 1, 3.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85DF0_Call(D2CommonCall2Fn fnCall)
{
	fnCall(2, 0x16C);
	fnCall(1, 0x16C);
	fnCall(3, 0x16C);
}

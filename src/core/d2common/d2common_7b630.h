// 1.13c D2Common+0x7B630: A* path search driver (ECX = path info). Retail 32-bit layout; host addresses ride as uint32_t.
#pragma once
#include <cstdint>

struct D2c7B630Env {
	uint32_t (*check)(uint32_t info);                                          // 0x7B2C0: EAX = info; zero rejects the request
	void (*insert)(uint32_t ctx, uint32_t node);                               // 0x7AE20: EDI = search context, ESI = node
	uint32_t (*pop)(uint32_t ctx);                                             // 0x7B220: ESI = context; next pending node or 0
	uint32_t (*explore)(uint32_t cur, uint32_t info, uint32_t ctx, uint32_t target); // 0x7B3C0: EAX = node, stack info, ctx, target; ret 0xC
	uint32_t (*flush)(uint32_t info, uint32_t best);                           // 0x7AB50: EAX = info, ECX = best node
};
extern "C" uint32_t d2c_7b630_run(uint32_t info, const D2c7B630Env* env);

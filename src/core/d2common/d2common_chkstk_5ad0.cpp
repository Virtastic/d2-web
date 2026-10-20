// 1.13c D2Common+0x05AD0 (61 B): __chkstk. `size` arrives in EAX, `esp` is the entry stack pointer (the
// address the return address was pushed to), `retaddr` the word stored. Touches each 0x1000 page downward;
// under wasm there is no guard page, so the observable is the stored address. Same object as BNClient 0x47C0.
#include <cstdint>

extern "C" void d2common_chkstk(uint32_t size, uint8_t* esp, uint32_t retaddr)
{
	if (size < 0x1000) {
		*(uint32_t*)(esp - size) = retaddr;
		return;
	}
	uint8_t* p = esp + 4;
	do {
		p -= 0x1000;
		size -= 0x1000;
	} while (size > 0xFFF);
	*(uint32_t*)(p - 4 - size) = retaddr;
}

// 1.13c D2Common+0x85E20: EAX = drlg (kept in ESI), plain ret. Runs 0x176F0 and 0x17180 on the drlg, then dispatches on the
// level type at drlg+0x1D0: 0x29..0x2E and 0x86 run a fixed chain of the per-type steps below, every other type does nothing more.
// The steps are called through `call(rva, drlg)`: the true register/stack convention of each is the caller's (ESI = EDI = drlg).
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85E20Call)(uint32_t rva, uint32_t drlg);
typedef void (*D2C42A70Fn)(uint32_t ecx, int32_t edx, int32_t y, int32_t tbl, int32_t n1, int32_t z);
extern "C" void d2c_85e20_port(uint32_t drlg, D2C85E20Call call, D2C42A70Fn f42a70)
{
	call(0x176F0, drlg);
	call(0x17180, drlg);
	const uint32_t t = *reinterpret_cast<const uint32_t*>(uintptr_t(drlg) + 0x1D0);
	switch (t) {
	case 0x29:
		call(0x85990, drlg); call(0x85DF0, drlg); call(0x859F0, drlg); call(0x42460, 5); call(0x85D90, drlg);
		return;
	case 0x2A:
		call(0x85930, drlg); call(0x85DF0, drlg); call(0x859F0, drlg); call(0x426A0, drlg); call(0x42460, 5); call(0x85D20, drlg);
		return;
	case 0x2B:
		call(0x85930, drlg); call(0x85DF0, drlg); call(0x859F0, drlg); call(0x85CF0, drlg); call(0x426A0, drlg); call(0x42460, 5); call(0x85C90, drlg);
		return;
	case 0x2C:
		call(0x85930, drlg); call(0x85DF0, drlg); call(0x859F0, drlg); call(0x85C50, drlg); call(0x426A0, drlg); call(0x42460, 5); call(0x85BE0, drlg);
		return;
	case 0x2D:
		call(0x859F0, drlg);
		return;
	case 0x2E:
		call(0x858E0, drlg); call(0x85DF0, drlg); call(0x42460, 5); call(0x85B80, drlg);
		return;
	case 0x86:
		f42a70(drlg, 4, 4, 0x18A, -1, 0); call(0x85DF0, drlg); call(0x85B80, drlg); call(0x42460, 5);
		return;
	default:
		return;
	}
}

// 1.13c D2Common+0x7F3A0: EAX = level (kept in ESI), tail-jumps to 0x7F180. Steps by level type t (+0x1D0):
//  t not 2, 3 or 0x11: 0x7E8C0; always 0x176F0 and 0x17180;
//  2 <= t <= 7: 0x43100 id 0, 0x7EF60, 0x43100 ids 1 and 2, 0x7EE50, 0x43100 id 3 (each with 4), 0x3E000;
//  t == 0x27: 0x43100 ids 0..3; 3 <= t <= 6: 0x426A0; 2 <= t <= 7: 0x42460 (5); then 0x7F180.
// call(rva, id) is the seam: id is the 0x43100 step id and ignored by the others.
#include <cstdint>
typedef void (*D2C7F3A0Call)(uint32_t rva, uint32_t id);
extern "C" void d2c_7f3a0_port(uint32_t level, D2C7F3A0Call call)
{
	const int32_t t = int32_t(*reinterpret_cast<const uint32_t*>(uintptr_t(level) + 0x1D0));
	if (t != 2 && t != 3 && t != 0x11) call(0x7E8C0, 0);
	call(0x176F0, 0);
	call(0x17180, 0);
	if (t >= 2 && t <= 7) {
		call(0x43100, 0); call(0x7EF60, 0); call(0x43100, 1); call(0x43100, 2);
		call(0x7EE50, 0); call(0x43100, 3); call(0x3E000, 0);
	}
	if (t == 0x27) { call(0x43100, 0); call(0x43100, 1); call(0x43100, 2); call(0x43100, 3); }
	if (t >= 3 && t <= 6) call(0x426A0, 0);
	if (t >= 2 && t <= 7) call(0x42460, 0);
	call(0x7F180, 0);
}

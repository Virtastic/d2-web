// 1.13c D2Common+0x7F180: EAX = level (kept in ESI), plain ret. Switch on the level type (+0x1D0) over a fixed list of tile-set steps:
// 0x43350 (EAX level, id, -1), 0x42CE0 (ESI level, id, -1, 0, 0xF) and 0x7EDC0 (EAX tile, ESI level, 1 stack word flag).
// Types 2..7 and 0x27 run their list then the common tail (0x42CE0 0x1D, 0x1E); types 4 and 7 return before the tail; type 0x11 places tile 0x6C
// at (1, 1) with 0x42A70 (ECX level, EDX 1, 1, 0x6C, -1, 0); other types do nothing.
// cb(rva, a, s0): 0x43350 gets a = id; 0x42CE0 gets a = id; 0x7EDC0 gets a = tile and s0 = flag; 0x42A70 gets a = tile.
#include <cstdint>
typedef void (*D2C7F180Cb)(uint32_t rva, uint32_t a, uint32_t s0);
extern "C" void d2c_7f180_port(uint32_t level, D2C7F180Cb cb)
{
	auto t = [&](uint32_t id) { cb(0x43350, id, 0); };
	auto s = [&](uint32_t id) { cb(0x42CE0, id, 0); };
	auto r = [&](uint32_t tile, uint32_t f) { cb(0x7EDC0, tile, f); };
	auto tail = [&]() { s(0x1D); s(0x1E); };
	switch (*reinterpret_cast<const uint32_t*>(uintptr_t(level) + 0x1D0)) {
	case 2: t(0x2E); r(0x2F, 0); tail(); break;
	case 3: r(0x30, 1); s(0x2C); tail(); break;
	case 4: t(0xA0); t(0x2D); s(0xA2); r(0x2F, 1); r(0x2A, 0); s(0x1F); return;
	case 5: s(0xA1); s(0x29); s(0x28); r(0x30, 1); r(0x2B, 0); tail(); break;
	case 6: s(0xA3); s(0x26); s(0x27); r(0x2F, 1); r(0x2A, 0); tail(); break;
	case 7: r(0x30, 1); r(0x2B, 0); s(0x1F); return;
	case 0x11: cb(0x42A70, 0x6C, 0); return;
	case 0x27: s(0x32); s(0x2E); s(0x1F); s(0x26); s(0x27); tail(); break;
	default: break;
	}
}

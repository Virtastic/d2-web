// D2Common 0x460B0, written from the 1.13c disassembly. Every emit goes through 0x45AA0 with EDX = 0x40, ECX = 0 unless noted,
// EAX = s2 and EDI = the entry.
//  mode 0, 2, 5 (3 entries) and 1 (2 entries): entry i is s3 + 0x24 + 0x10 i (mode 0; s3 null or i >= 3 fatal), s3 + 0xC + 0x10 i
//   (mode 1; i >= 12 fatal) or 0x458C0(EAX = i, ECX = s3, EDX = s4) (modes 2 and 5). A null entry or one whose first dword is
//   negative stops the walk (EAX = the entry); otherwise emit(mode, 0, s3, i, s4, s5, s1). A full walk leaves EAX = the count.
//  mode 3: index 0x22640(s2) (negative: stop, EAX = it), asked again, row 0x1830; twelve emits (3, 0, 0, 0, 0, s5, 0) over the
//   row's entries from +0x8C.
//  mode 4: row 0x1800(0x22640(s2)); 0x221C0(s2) with AX zero: two emits (4, 0, 0, 0, 0, s5, 0) from row +0x88. Otherwise nine
//   from +0x88 then ten from +0x118, the latter with ECX = 0xA5 + i / 2 and EDX = 0x2040 when row byte +0x87 is set.
// A mode above 5 does nothing and leaves the caller's EAX (not modelled: 0).
#include <cstdint>
#include "d2common_460b0.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline uint8_t rb(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }

// 1.13c D2Common+0x460B0
extern "C" uint32_t D2Common_460B0_With(uint32_t mode, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5,
	const D2Common460B0Env* e)
{
	uint32_t r = 0;
	if (mode == 3) {
		const int32_t v = e->index(s2);
		if (v < 0) return uint32_t(v);
		const uint32_t row = e->rowA(uint32_t(e->index(s2)));
		if (!row) return 0;
		for (uint32_t k = 0; k < 12; k++) r = e->emit(s2, 0, 0x40, row + 0x8c + 0x10 * k, 3, 0, 0, 0, 0, s5, 0);
		return r;
	}
	if (mode == 4) {
		const uint32_t row = e->rowB(uint32_t(e->index(s2)));
		if (!row) return 0;
		if (!(e->kind(s2) & 0xffff)) {
			for (uint32_t k = 0; k < 2; k++) r = e->emit(s2, 0, 0x40, row + 0x88 + 0x10 * k, 4, 0, 0, 0, 0, s5, 0);
			return r;
		}
		for (uint32_t k = 0; k < 9; k++) r = e->emit(s2, 0, 0x40, row + 0x88 + 0x10 * k, 4, 0, 0, 0, 0, s5, 0);
		for (uint32_t i = 0; i < 10; i++) {
			const bool on = rb(row + 0x87) != 0;
			r = e->emit(s2, on ? 0xa5 + i / 2 : 0, on ? 0x2040 : 0x40, row + 0x118 + 0x10 * i, 4, 0, 0, 0, 0, s5, 0);
		}
		return r;
	}
	uint32_t count;
	if (mode == 1) count = 2;
	else if (mode == 0 || mode == 2 || mode == 5) count = 3;
	else return 0;
	for (uint32_t i = 0; i < count; i++) {
		const uint32_t at = s3 + 0xc + 0x10 * i;
		uint32_t p;
		if (mode == 0) {
			if (!s3) { e->fatal(0x79); return 0; }
			if (i >= 3) { e->fatal(0x7a); return 0; }
			p = at + 0x18;
		} else if (mode == 1) {
			if (i >= 0xc) { e->fatal(0x81); return 0; }
			p = at;
		} else {
			p = e->slot(i, s3, s4);
		}
		if (!p || int32_t(rd(p)) < 0) return p;
		e->emit(s2, 0, 0x40, p, mode, 0, s3, i, s4, s5, s1);
	}
	return count;
}

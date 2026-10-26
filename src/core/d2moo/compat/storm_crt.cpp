// Storm statically linked VC7.1 CRT leaves, C-locale contract. Rows without a portable contract (SEH, TLS, ioinit,
// locale init, live-register thiscall helpers) are left UNMAPPED.
#include <cstdint>
#include <cstddef>

// 1.13c Storm+0x6659: _toupper (C locale, via per-thread locale table: ASCII a-z only)
int storm_toupper(int c) { return (c >= 'a' && c <= 'z') ? c - 32 : c; }

// 1.13c Storm+0x763A: _tolower
int storm_tolower(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }

// 1.13c Storm+0x7660: ___ascii_strnicmp; folds A-Z, stops at either NUL, returns -1/0/1
int storm_ascii_strnicmp(const char* a, const char* b, size_t n) {
	for (; n; --n) {
		uint8_t x = (uint8_t)*a, y = (uint8_t)*b;
		if (!x || !y) { return x == y ? 0 : x < y ? -1 : 1; }
		a++; b++;
		if (x > 0x40 && x < 0x5b) x += 0x20;
		if (y > 0x40 && y < 0x5b) y += 0x20;
		if (x != y) return x < y ? -1 : 1;
	}
	return 0;
}

// 1.13c Storm+0xA18F: __security_check_cookie; match returns, mismatch is fatal (report_failure)
void storm_security_check_cookie(uintptr_t v, uintptr_t cookie) { if (v != cookie) __builtin_trap(); }

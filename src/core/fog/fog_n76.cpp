// Fog n76: libm pow, MSVC CRT x87 float<->text (12-byte LDBL12 helpers), QServer accept, recv thread, SEH unwind tail.
// 12-byte "LDBL12": [0..1] guard limb, [2..9] 64-bit mantissa, [10..11] sign/exponent.
#include <cstdint>
#include <cstring>
#include <cmath>
extern "C" {
void Sleep(uint32_t);
void EnterCriticalSection(void*);
void LeaveCriticalSection(void*);
int QueryPerformanceCounter(void*);
void* CreateIoCompletionPort(void*, void*, uintptr_t, uint32_t);
void _exit(int);
void FUN_6ff61690(void*);
void fog_crt_mantissa_0C1D6(const char*, int32_t, uint32_t*);  // Fog+0xC1D6
char* fog_strcpy_8e70(char*, const char*);                     // Fog+0x8E70
int FUN_6ff66800(void*);
int FUN_6ff6e620(void*, int);
void q_fog_n76_FUN_6ff6e550(void);
int Ordinal_16(uint32_t, void*, int, int);
uint32_t q_fog_n76_Ordinal_111(void);
}
// host seam: CRT 10^n tables (DAT_6ff7ecf0 positive, DAT_6ff7ee50 negative), 12-byte entries; null -> generated.
extern "C" { const uint8_t* fog_n76_pow10_tab[2] = {nullptr, nullptr}; }
// = {nullptr, nullptr};

// ---- 96-bit helpers (___shl_12 / ___shr_12 / ___add_12 / ___addl) ----
static void shl12(uint8_t* p) { uint32_t w[3]; memcpy(w, p, 12); w[2] = (w[2] << 1) | (w[1] >> 31); w[1] = (w[1] << 1) | (w[0] >> 31); w[0] <<= 1; memcpy(p, w, 12); }
static void shr12(uint8_t* p) { uint32_t w[3]; memcpy(w, p, 12); w[0] = (w[0] >> 1) | (w[1] << 31); w[1] = (w[1] >> 1) | (w[2] << 31); w[2] >>= 1; memcpy(p, w, 12); }
static void add12(uint8_t* d, const uint8_t* s) {
	uint32_t a[3], b[3]; memcpy(a, d, 12); memcpy(b, s, 12);
	uint64_t t = (uint64_t)a[0] + b[0]; a[0] = (uint32_t)t;
	t = (uint64_t)a[1] + b[1] + (t >> 32); a[1] = (uint32_t)t; a[2] += b[2] + (uint32_t)(t >> 32);
	memcpy(d, a, 12);
}
static uint16_t ld16(const uint8_t* p, int limb) { uint16_t v; memcpy(&v, p + limb * 2, 2); return v; }
static void st16(uint8_t* p, int limb, uint16_t v) { memcpy(p + limb * 2, &v, 2); }

// 1.13c FOG+0x23419 -- libm pow(x, y); y arrives as two 32-bit halves (param_2 low, param_3 high)
extern "C" double fog_pow_23419(double x, int32_t ylo, uint32_t yhi) {  // 1.13c FOG+0x23419
	uint64_t b = ((uint64_t)yhi << 32) | (uint32_t)ylo; double y; memcpy(&y, &b, 8);
	return pow(x, y);
}

// table entry (k = loop level 1.., m = 1..7): 10^(+-m*8^(k-1)) as LDBL12; seam bytes win, else generated from powl.
static void pow10_entry(int neg, int k, int m, uint8_t* out) {
	if (fog_n76_pow10_tab[neg]) { memcpy(out, fog_n76_pow10_tab[neg] + 12 * (7 * k + m), 12); return; }
	long e = m; for (int i = 1; i < k; i++) e *= 8;
	if (neg) e = -e;
	long double v = powl(10.0L, (long double)e); int e2; long double f = frexpl(v, &e2);
	uint64_t mant = (uint64_t)(ldexpl(f, 64) + 0.5L); uint16_t se = (uint16_t)(e2 - 1 + 0x3fff);
	if (!mant) { mant = 1ull << 63; se++; }  // rounded up past 2^64
	memset(out, 0, 12); memcpy(out + 2, &mant, 8); memcpy(out + 10, &se, 2);
}

// 1.13c FOG+0x0C99A -- LDBL12 multiply: a *= b, round-half-even into limbs 1..5
extern "C" void fog_crt_mul_C99A(uint8_t* a, const uint8_t* b) {  // 1.13c FOG+0x0C99A
	uint16_t eb = ld16(b, 5) & 0x7fff, ea = ld16(a, 5) & 0x7fff, sg = (ld16(b, 5) ^ ld16(a, 5)) & 0x8000;
	uint16_t e = (uint16_t)(eb + ea);
	uint16_t L[8] = {0}; uint16_t ex = 0;
	if (!(ea < 0x7fff && eb < 0x7fff && e < 0xbffe)) goto inf;
	if (e < 0x3fc0) goto zero;
	if (ea == 0) {
		e++; ex = 0;
		if (!((ld16(a, 4) | ld16(a, 5) & 0x7fff) || ld16(a, 3) || ld16(a, 2) || ld16(a, 1) || ld16(a, 0))) { st16(a, 5, ex); return; }
	}
	if (eb == 0) {
		e++;
		if ((ld16(b, 5) & 0x7fff) == 0 && ld16(b, 4) == 0 && ld16(b, 3) == 0 && ld16(b, 2) == 0 && ld16(b, 1) == 0 && ld16(b, 0) == 0) goto zero;
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5 - i; j++) {
			uint32_t d = (uint32_t)L[i] | ((uint32_t)L[i + 1] << 16);
			uint64_t t = (uint64_t)d + (uint32_t)ld16(a, i + j) * (uint32_t)ld16(b, 4 - j);
			L[i] = (uint16_t)t; L[i + 1] = (uint16_t)(t >> 16);
			if (t >> 32) L[i + 2]++;
		}
	}
	ex = (uint16_t)(e + 0xc002);
	{
		uint8_t w[12]; for (int i = 0; i < 6; i++) st16(w, i, L[i]);
		if ((int16_t)ex > 0) {
			do { if (w[11] & 0x80) break; shl12(w); ex--; } while ((int16_t)ex > 0);
		}
		if ((int16_t)ex < 1) {
			ex--;
			if ((int16_t)ex < 0) {
				uint32_t n = (uint16_t)-ex; int sticky = 0; ex = 0;
				do { if (w[0] & 1) sticky++; shr12(w); } while (--n);
				if (sticky) w[0] |= 1;
			}
		}
		uint16_t l0 = ld16(w, 0); uint32_t l01 = l0 | ((uint32_t)ld16(w, 1) << 16);
		uint32_t m1 = ld16(w, 1) | ((uint32_t)ld16(w, 2) << 16), m2 = ld16(w, 3) | ((uint32_t)ld16(w, 4) << 16); uint16_t m3 = ld16(w, 5);
		if (l0 > 0x8000 || (l01 & 0x1ffff) == 0x18000) {
			if (m1 == 0xffffffffu) {
				m1 = 0;
				if (m2 == 0xffffffffu) { if (m3 == 0xffff) { ex++; m3 = 0x8000; m2 = 0; } else { m3++; m2 = 0; } }
				else m2++;
			} else m1++;
		}
		if (ex > 0x7ffe) goto inf;
		uint32_t o1 = m1, o2 = m2;
		memset(a, 0, 12);
		st16(a, 0, (uint16_t)o1); st16(a, 1, (uint16_t)(o1 >> 16)); st16(a, 2, (uint16_t)o2); st16(a, 3, (uint16_t)(o2 >> 16)); st16(a, 4, m3);
		st16(a, 5, ex | sg);
		return;
	}
inf: { uint32_t z = 0, hi = (sg ? 0x80000000u : 0) + 0x7fff8000u; memcpy(a, &z, 4); memcpy(a + 4, &z, 4); memcpy(a + 8, &hi, 4); return; }
zero: memset(a, 0, 12);
}

// 1.13c FOG+0x0CBD0 -- a *= 10^n via the CRT power table (n==0: nothing)
extern "C" void fog_crt_mulpow10_CBD0(uint8_t* a, int32_t n, int32_t keepguard) {  // 1.13c FOG+0x0CBD0
	if (!n) return;
	int neg = 0;
	if (n < 0) { n = -n; neg = 1; }
	if (!keepguard) st16(a, 0, 0);
	for (int k = 1; n; k++) {
		int m = n & 7; n >>= 3;
		if (!m) continue;
		uint8_t e[12]; pow10_entry(neg, k, m, e);
		if (ld16(e, 0) > 0x7fff) { uint32_t d; memcpy(&d, e + 2, 4); d--; memcpy(e + 2, &d, 4); }
		fog_crt_mul_C99A(a, e);
	}
}

// 1.13c FOG+0x0C6FE -- 80-bit -> decimal digits. out: i16 exp @0, sign char @2, ndigits @3, digits @4.. (NUL-terminated)
extern "C" int fog_crt_i10_output_C6FE(uint32_t lo, uint32_t hi, uint16_t se, int nd, uint8_t flag, int16_t* out) {  // 1.13c FOG+0x0C6FE
	char* o = (char*)out; char* dig = o + 4;
	uint32_t ex = se & 0x7fff;
	const uint8_t c01[12] = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfb, 0x3f};
	o[2] = (se & 0x8000) ? '-' : ' ';
	auto zero = [&]() { out[0] = 0; o[2] = ' '; o[3] = 1; o[5] = 0; };
	if (ex == 0 && hi == 0 && lo == 0) { dig[0] = '0'; zero(); return 1; }
	if (ex == 0x7fff) {
		out[0] = 1; const char* s; int n;
		if ((hi == 0x80000000u && lo == 0) || (hi & 0x40000000)) {
			if (!(se & 0x8000) || hi != 0xc0000000u) { if (hi != 0x80000000u || lo) { s = "1#QNAN"; n = 6; } else { s = "1#INF"; n = 5; } }
			else if (lo) { s = "1#QNAN"; n = 6; } else { s = "1#IND"; n = 5; }
		} else { s = "1#SNAN"; n = 6; }
		fog_strcpy_8e70(dig, s); o[3] = (char)n; return 0;
	}
	uint8_t b[12] = {0}; memcpy(b + 2, &lo, 4); memcpy(b + 6, &hi, 4); b[10] = (uint8_t)ex; b[11] = (uint8_t)(ex >> 8);
	int16_t est = (int16_t)(((((ex >> 8) + (hi >> 24) * 2) * 0x4d + (uint32_t)-0x134312f4 + ex * 0x4d10)) >> 16);
	fog_crt_mulpow10_CBD0(b, -(int)est, 1);
	if (ld16(b, 5) > 0x3ffe) { est++; fog_crt_mul_C99A(b, c01); }
	out[0] = est;
	if ((flag & 1) && (nd += est, nd < 1)) { dig[0] = '0'; zero(); return 1; }
	if (nd > 0x15) nd = 0x15;
	int ie = ld16(b, 5) - 0x3ffe;
	b[10] = b[11] = 0;
	for (int i = 8; i; i--) shl12(b);
	if (ie < 0) for (uint32_t i = -ie & 0xff; i; i--) shr12(b);
	char* p = dig;
	for (int c = nd + 1; c > 0; c--) {
		uint8_t cp[12]; memcpy(cp, b, 12);
		shl12(b); shl12(b); add12(b, cp); shl12(b);
		*p++ = (char)(b[11] + '0'); b[11] = 0;
	}
	char* q = p - 1;
	if (*q < '5') {
		for (; dig <= q && *q == '0'; q--) {}
		if (q < dig) { dig[0] = '0'; zero(); return 1; }
	} else {
		for (; dig <= q && *q == '9'; q--) *q = '0';
		if (q < dig) { q++; out[0]++; }
		(*q)++;
	}
	char cnt = (char)(q - dig + 1); o[3] = cnt; dig[(int)cnt] = 0;
	return 1;
}

// 1.13c FOG+0x0BF1C -- fltout front end: double -> 17 digits; res = {sign, exp, rc, digits ptr(32-bit)}, digits copied to buf
extern "C" int32_t* fog_crt_dtold_BF1C(double d, int32_t* res, char* buf) {  // 1.13c FOG+0x0BF1C
	uint64_t u; memcpy(&u, &d, 8);
	uint32_t ex = (u >> 52) & 0x7ff, sign = (uint32_t)(u >> 63); uint64_t f = u & 0xfffffffffffffull, mant; uint16_t se;
	if (ex == 0x7ff) { mant = 0x8000000000000000ull | (f << 11); se = 0x7fff; }
	else if (ex) { mant = 0x8000000000000000ull | (f << 11); se = (uint16_t)(ex - 1023 + 16383); }
	else if (!f) { mant = 0; se = 0; }
	else { int sh = __builtin_clzll(f); mant = f << sh; se = (uint16_t)(16383 - 1022 - (sh - 11)); }
	se |= (uint16_t)(sign << 15);
	struct { int16_t e; char s; char n; char dg[24]; } r;
	int rc = fog_crt_i10_output_C6FE((uint32_t)mant, (uint32_t)(mant >> 32), se, 0x11, 0, &r.e);
	res[2] = rc; res[0] = r.s; res[1] = r.e;
	fog_strcpy_8e70(buf, r.dg);
	res[3] = (int32_t)(uintptr_t)buf;
	return res;
}

// 1.13c FOG+0x0C2BE -- __fltin: text -> LDBL12 at out; *endp = end of scan; returns 0 ok, 1 underflow, 2 overflow, 4 no number
extern "C" int fog_crt_fltin_C2BE(uint16_t* out, const uint8_t** endp, const uint8_t* s, int p4, int p5, int p6, int p7) {  // 1.13c FOG+0x0C2BE
	auto dg = [](int c) { return c >= '0' && c <= '9'; };
	int st = 0, ret = 0, sgnexp = 1, exp10 = 0, nd = 0, adj = 0;
	bool b1 = false, b3 = false, b2 = false;
	uint16_t sg = 0; uint8_t buf[26] = {0}; uint8_t* pd = buf;
	const uint8_t *p = s, *p2, *save = s; int c;
	while ((c = *p) == ' ' || c == 9 || c == 10 || c == 13) p++;
top:
	c = *p; p2 = p + 1;
	switch (st) {
	case 0:
		if (c > '0' && c <= '9') goto to3;
		if (c == '.') goto to5;
		st = 2;
		if (c == '+') { sg = 0; p = p2; goto top; }
		if (c == '-') { sg = 0x8000; p = p2; goto top; }
		break;
	case 1:
		st = 1; b1 = true;
		if (c > '0' && c <= '9') goto to3;
		if (c == '.') goto dot;
		if (c == '+' || c == '-') goto sgn;
		p = p2; if (c != '0') goto l3b6; goto top;
	case 2:
		if (c > '0' && c <= '9') goto to3;
		p = save; if (c == '.') goto to5;
		break;
	case 3:
		while (dg(c)) { if (nd < 0x19) { nd++; *pd++ = (uint8_t)(c - '0'); } else adj++; c = *p2++; }
		if (c != '.') goto l48c;
	dot: b1 = true; st = 4; p = p2; goto top;
	case 4:
		b3 = true;
		if (nd == 0) while (c == '0') { adj--; c = *p2++; }
		while (dg(c)) { if (nd < 0x19) { nd++; *pd++ = (uint8_t)(c - '0'); adj--; } c = *p2++; }
	l48c:
		if (c == '+' || c == '-') {
		sgn: b1 = true; st = 0xb; p = p2 - 1; goto top;
		}
	l3b6:
		b1 = true;
		if (c < 'D' || (c > 'E' && (c < 'd' || c > 'e'))) { p = p2 - 1; goto fin; }
		st = 6; p = p2; goto top;
	case 5:
		b3 = true; p = save;
		if (dg(c)) { st = 4; p = p2 - 1; goto top; }
		goto fin;
	case 6:
		save = p - 1;
		if (c < '1' || c > '9') {
			if (c == '+') goto l592;
			if (c == '-') goto l586;
			p = save; if (c == '0') goto l4f4;
			goto fin;
		}
		goto l55b;
	case 7:
		if (c > '0' && c <= '9') goto l55b;
		p = save; if (c != '0') goto fin;
	l4f4: st = 8; p = p2; goto top;
	case 8:
		b2 = true;
		while (c == '0') c = *p2++;
		if (c > '0' && c <= '9') goto l55b;
		p = p2 - 1; goto fin;
	case 9:
		b2 = true; exp10 = 0; goto l5d9;
	case 0xb:
		if (p7) {
			save = p;
			if (c == '+') { l592: st = 7; p = p2; goto top; }
			if (c != '-') goto fin;
		l586: sgnexp = -1; st = 7; p = p2; goto top;
		}
		st = 10; p = p;  // case 10: scan ends here
		goto fin;
	default: goto fin;
	}
	if (c != '0') goto fin;
	st = 1; p = p2; goto top;
to3: st = 3; p = p2 - 1; goto top;
to5: st = 5; p = p2; goto top;
l55b: st = 9; p = p2 - 1; goto top;
l5d9:
	while (dg(c)) {
		exp10 = c - '0' + exp10 * 10;
		if (exp10 > 0x1450) { exp10 = 0x1451; break; }
		c = *p2++;
	}
	while (dg(c)) c = *p2++;
	p = p2 - 1;
fin:
	*endp = p;
	uint8_t w[12] = {0};
	if (!b1) ret = 4;
	else {
		if (nd > 0x18) { if (buf[23] > 4) buf[23]++; pd--; adj++; nd = 0x18; }
		if (nd) {
			while (*--pd == 0) { nd--; adj++; }
			fog_crt_mantissa_0C1D6((const char*)buf, nd, (uint32_t*)w);
			if (sgnexp < 0) exp10 = -exp10;
			int32_t e = exp10 + adj;
			if (!b2) e += p5;
			if (!b3) e -= p6;
			if (e > 0x1450) { memset(w, 0, 12); st16(w, 5, 0x7fff); w[9] = 0x80; ret = 2; }
			else if (e > -0x1451) fog_crt_mulpow10_CBD0(w, e, p4);
			else { memset(w, 0, 12); ret = 1; }
		}
	}
	if (ret == 4 || ret == 1) memset(w, 0, 12);
	memcpy(out, w, 12);
	st16((uint8_t*)out, 5, (uint16_t)(ld16(w, 5) | sg));
	return ret;
}

// ---- struct/host-seam functions ----
#define U32(p, o) (*(uint32_t*)((uint8_t*)(p) + (o)))
#define NODE(h) ((uint8_t*)(intptr_t)(h))

// 1.13c FOG+0x1E9C0 -- safesock receive thread body; o = socket object (32-bit handle fields)
extern "C" uint32_t fog_safesock_recv_1E9C0(uint8_t* o) {  // 1.13c FOG+0x1E9C0
	uint8_t buf[0x2064]; int fill = 0;
	while (U32(o, 0x110) == 0) { if (U32(o, 0x124)) goto done; Sleep(0); }
	if (U32(o, 0x124) == 0) {
		for (;;) {
			uint8_t* p = NODE(U32(o, 0x110));
			if (!p || !U32(p, 0) || U32(p, 0x2c) || U32(p, 0x30)) break;
			int r = FUN_6ff66800(p);
			if (!U32(o, 0x110) || U32(NODE(U32(o, 0x110)), 0x2c)) break;
			if (r) {
				for (;;) {
					if (U32(o, 0x124)) goto done;
					uint8_t* q = NODE(U32(o, 0x110)); int n;
					if (!q || U32(q, 0x2c)) n = 0;
					else {
						void* cs = q + 0xc; FUN_6ff61690(cs); EnterCriticalSection(cs);
						n = Ordinal_16(U32(q, 8), buf + fill, 0x2064 - fill, 0);
						LeaveCriticalSection(cs);
						if (n < 1) { U32(q, 0x34) = q_fog_n76_Ordinal_111(); U32(q, 0x30) = 4; }
					}
					int room = 0x2064 - fill;
					if (U32(NODE(U32(o, 0x110)), 0x30) || n == 0) goto done;
					fill = FUN_6ff6e620(buf, fill + n);
					if ((uint32_t)n < (uint32_t)room) break;
				}
			}
			if (U32(o, 0x124)) break;
		}
	}
done:
	q_fog_n76_FUN_6ff6e550();
	return 0;
}

// 1.13c FOG+0x22DEE -- SEH catch-frame epilogue; st carries the caller-frame slots the decompile read off EBP/ESI/EDI
struct Fog76Unwind { uint32_t* slotM4; uint32_t saved38; void* frame3c; void* unexpected40; void* translator44; int32_t* rec; int32_t f48; int32_t f20; };
extern "C" {
void _FindAndUnlinkFrame(void*);
void** fog_n76_ptd_unexpected(void);  // host seam: per-thread _tiddata._unexpected
void** fog_n76_ptd_translator(void);  // host seam: per-thread _tiddata._translator
int IsExceptionObjectToBeDestroyed(void*);
void __abnormal_termination(void);
void ___DestructExceptionObject(int32_t*);
}
extern "C" void fog_sub_22dee(Fog76Unwind* s) {  // 1.13c FOG+0x22DEE
	*s->slotM4 = s->saved38;
	_FindAndUnlinkFrame(s->frame3c);
	*fog_n76_ptd_unexpected() = s->unexpected40;
	*fog_n76_ptd_translator() = s->translator44;
	int32_t* r = s->rec;
	if (r[0] == -0x1f928c9d && r[4] == 3 && (r[5] == 0x19930520 || r[5] == 0x19930521) && s->f48 == 0 && s->f20 != 0 &&
	    IsExceptionObjectToBeDestroyed((void*)(intptr_t)r[6])) {
		__abnormal_termination();
		___DestructExceptionObject(r);
	}
}

// 1.13c FOG+0x20420 -- QServer accept of one connection into slot idx; st carries the register inputs (srv=EAX, sock=EBX, etc.)
struct Fog76Accept { uint8_t* srv; uint32_t* sock; uint32_t edi; uint32_t ebp; uint32_t* listen; uint8_t* acc; uint32_t reject; uint8_t ov[32]; };
extern "C" {
int Ordinal_1(void);
void Ordinal_11(uint32_t, const char*);
void q_fog_n76_Ordinal_3(void*);
void Ordinal_12(void*, uint32_t, void*);
void Ordinal_5(uint32_t, void*);
void Ordinal_494(void*, int);
void Ordinal_6(int, void*, void*);
void Ordinal_116(void);
int FUN_6ff5dd90(uint32_t);
uint8_t FUN_6ff5dd10(void*);
void FUN_6ff5f250(void*);
int FUN_6ff5dbe0(void*);
int FUN_6ff5ceb0(uint32_t);
void FUN_6ff5df00(uint32_t, const char*, int);
uint32_t* FUN_6ff5e5f0(uint32_t, uint32_t);
void FUN_6ff690a0(const void*, const char*);
int FUN_6ff70010(void);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff5fc20(void);
void FUN_6ff684a0(void);
extern const char DAT_6ff77040[];
}
static void bump(uint8_t* srv, int off) { void* cs = srv + 0x8b4; FUN_6ff61690(cs); EnterCriticalSection(cs); U32(srv, off)++; LeaveCriticalSection(cs); }
extern "C" intptr_t fog_sub_20420(Fog76Accept* a, int idx, uint32_t* o4, uint32_t* o1) {  // 1.13c FOG+0x20420
	uint8_t* srv = a->srv; uint8_t* r = srv + idx * 0xb0; const char* src = "..\\Source\\Fog\\Src\\QServer\\QServerNT.cpp";
	memset(r + 0x91c, 0, 0x48); QueryPerformanceCounter(r + 0x91c);
	*o1 = 0x10; o4[0] = o4[1] = o4[2] = o4[3] = 0;
	bump(srv, idx * 0xb0 + 0x964);
	int rc = Ordinal_1();
	QueryPerformanceCounter(r + 0x924);
	if (rc == -1) { bump(srv, idx * 0xb0 + 0x968); q_fog_n76_Ordinal_111(); return -1; }
	uint32_t* ap = (uint32_t*)(a->acc + 4);
	uint32_t kind = FUN_6ff5dd90(*ap) != 0;
	QueryPerformanceCounter(r + 0x92c);
	if (!kind && FUN_6ff5dd10(ap)) { FUN_6ff5f250(ap); kind = 2; }
	QueryPerformanceCounter(r + 0x934);
	if (a->edi == 0 && !FUN_6ff5dbe0(a->listen) && !FUN_6ff5ceb0(*a->listen)) FUN_6ff5df00(*a->listen, src, 0x4ab);
	QueryPerformanceCounter(r + 0x93c);
	if (a->reject) {
		bump(srv, idx * 0xb0 + 0x96c);
		Ordinal_11(*a->sock, a->reject == 1 ? "user on hack list" : a->reject == 2 ? "user logged on twice" : "user spamming with reconnects");
		FUN_6ff690a0(DAT_6ff77040, "[sQSNTAccept]               *** Deleted socket %d (%s) - %s ***");
		q_fog_n76_Ordinal_3(a->sock);
		return -1;
	}
	uint32_t* c = FUN_6ff5e5f0((uint32_t)(uintptr_t)a->listen, *a->sock);
	c[5] = a->ebp;
	QueryPerformanceCounter(r + 0x944);
	Ordinal_12(a->listen, 0x8004667e, a->ov);
	Ordinal_5(kind, c + 0x22); Ordinal_494(c + 0x24, 8);
	Ordinal_6((int)(intptr_t)(r + 0x934), c + 0x1e, a->ov + 0x14); Ordinal_494(c + 0x20, 8);
	if ((uint8_t*)(intptr_t)c[4] != a->ov) {
		FUN_6ff5fc20(); Ordinal_116();
		FUN_6ff60f30(6, "", 0x4dd, "Unrecoverable internal error %08x"); FUN_6ff684a0(); _exit(-1);
	}
	CreateIoCompletionPort(a->ov, *(void**)(srv + 0xc08 + kind * 4), *c, 0);
	QueryPerformanceCounter(r + 0x94c);
	if (!FUN_6ff70010()) { bump(srv, idx * 0xb0 + 0x970); return -1; }
	QueryPerformanceCounter(r + 0x954);
	if (U32(srv, 0xbc4)) ((void (*)(const char*, int))(intptr_t)U32(srv, 0xbc4))(src, 0x4f2);
	QueryPerformanceCounter(r + 0x95c);
	bump(srv, idx * 0xb0 + 0x974);
	return (intptr_t)a->ov;
}

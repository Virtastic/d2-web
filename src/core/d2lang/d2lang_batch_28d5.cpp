// 1.13c D2Lang implementation batch: +0x026E0 +0x028D5 +0x0152C +0x01B90 +0x03102 +0x05D31
// (statically linked MSVC CRT). Win32/CRT services come in through the env structs below.
// Not here (already ported): +0x08BB0 +0x011C6 +0x01248 +0x013FB +0x03ACC +0x06BBE.
#include <cstdint>
#include <cstddef>
#include <cstring>

// D2Lang+0x026E0 ___ascii_stricmp: A-Z folded to lower case, result -1/0/1.
static int d2_fn_d2lang_26e0(const char* a, const char* b)
{
	for (;;)
	{
		uint8_t ca = static_cast<uint8_t>(*a++), cb = static_cast<uint8_t>(*b++);
		if (ca != cb)
		{
			uint8_t fa = ca - 'A' < 26u ? ca + 32 : ca, fb = cb - 'A' < 26u ? cb + 32 : cb;
			if (fa != fb)
				return fa < fb ? -1 : 1;
		}
		if (cb == 0)
			return 0;
	}
}

// D2Lang+0x0152C: tail of a locked CRT routine; releases CRT lock 8 when the saved flag is set.
extern "C" void d2_fn_d2lang_152c(int lockedFlag, int zero, void (*unlock)(int))
{
	if (lockedFlag != zero)
		unlock(8);
}

// D2Lang+0x01B90 __aulldvrm: unsigned 64-bit divide; quotient in EDX:EAX, remainder in ECX.
struct d2lang_divrm { uint64_t quot, rem; };
extern "C" d2lang_divrm d2_fn_d2lang_1b90(uint64_t num, uint64_t den)
{
	return { num / den, num % den };
}

// D2Lang+0x03102 _calloc_crt/calloc: size overflow guard, 0 -> 1 byte, retry through the new-handler.
// alloc() is HeapAlloc(HEAP_ZERO_MEMORY) plus the small-block heap (zeroed); newh is __callnewh, NULL when unset.
struct d2lang_calloc_env { void* (*alloc)(uint32_t size); int (*newh)(uint32_t size); };
extern "C" void* d2_fn_d2lang_3102(const d2lang_calloc_env* env, uint32_t count, uint32_t size)
{
	if (count != 0 && size > 0xffffffe0u / count)
		return nullptr;
	uint32_t bytes = count * size;
	if (bytes == 0)
		bytes = 1;
	for (;;)
	{
		void* p = bytes < 0xffffffe1u ? env->alloc(bytes) : nullptr;
		if (p)
			return p;
		if (!env->newh || env->newh(bytes) == 0)
			return nullptr;
	}
}

// D2Lang+0x05D31 __getbuf: give a stream a 4096-byte buffer, or the 2-byte _charbuf on failure.
struct d2lang_FILE { char* ptr; int cnt; char* base; int flag; int file; int charbuf; int bufsiz; };
extern "C" void d2_fn_d2lang_5d31(d2lang_FILE* f, void* (*mallocFn)(size_t), int* getbufCount)
{
	++*getbufCount;
	f->base = static_cast<char*>(mallocFn(0x1000));
	if (!f->base)
	{
		f->flag |= 4;
		f->base = reinterpret_cast<char*>(&f->charbuf);
		f->bufsiz = 2;
	}
	else
	{
		f->flag |= 8;
		f->bufsiz = 0x1000;
	}
	f->cnt = 0;
	f->ptr = f->base;
}

// 1.13c D2Lang+0x028D5 _output: printf engine. Retail's va_list of 4-byte stack slots becomes an array of
// 64-bit slots (one per int / pointer / double bit pattern, 64-bit ints take one slot). Sinks and the
// float/locale services (_fptrap cvt/cropzeros/forcdecpt, _wctomb, isleadbyte) are env members.
struct d2lang_ansistr { uint16_t len, max; const void* buf; }; // %Z argument
struct d2lang_output_env
{
	int (*put)(void* ctx, int ch);                                       // 0 on success
	void* ctx;
	void (*cvt)(uint64_t dbl, char* buf, int fmt, int prec, int caps);   // PTR___fptrap_6fc0fa70
	void (*cropzeros)(char* buf);                                        // ..fa74
	void (*forcdecpt)(char* buf);                                        // ..fa7c
	int (*wctomb)(char* out, char16_t wc);                               // <0 on failure
	int (*isleadbyte)(int ch);                                           // NULL = never
};

// DAT_6fc0c3e0 / DAT_6fc0c400 (one array: character class, then state transitions), 0x59 bytes.
static const uint8_t k_lookup[0x59] = {
	0x06,0x00,0x00,0x06,0x00,0x01,0x00,0x00,0x10,0x00,0x03,0x06,0x00,0x06,0x02,0x10,
	0x04,0x45,0x45,0x45,0x05,0x05,0x05,0x05,0x05,0x35,0x30,0x00,0x50,0x00,0x00,0x00,
	0x00,0x20,0x28,0x38,0x50,0x58,0x07,0x08,0x00,0x37,0x30,0x30,0x57,0x50,0x07,0x00,
	0x00,0x20,0x20,0x08,0x00,0x00,0x00,0x00,0x08,0x60,0x68,0x60,0x60,0x60,0x60,0x00,
	0x00,0x70,0x70,0x78,0x78,0x78,0x78,0x08,0x07,0x08,0x00,0x00,0x07,0x00,0x08,0x08,
	0x08,0x00,0x00,0x08,0x00,0x08,0x00,0x07,0x08 };

// 1.13c D2Lang+0x028D5
extern "C" int d2_fn_d2lang_28d5(const d2lang_output_env* e, const uint8_t* fmt, const uint64_t* args)
{
	int total = 0, state = 0;
	uint32_t flags = 0;
	int width = 0, prec = -1, wide = 0, suppress = 0, prefLen = 0, caps = 0, hexAdd = 0;
	size_t textLen = 0;                 // persists across specs, as the retail locals do
	const char* text = "";
	const char16_t* wtext = nullptr;
	char prefix[2] = { 0, 0 };
	char num[0x210];
	char fbuf[0x400];
	char cbuf[8];
	auto put = [&](int ch) { if (total >= 0 && e->put(e->ctx, ch)) total = -1; else if (total >= 0) ++total; };
	auto multi = [&](int ch, int n) { while (n-- > 0 && total >= 0) put(ch); };
	auto str = [&](const char* s, int n) { while (n-- > 0 && total >= 0) put(static_cast<uint8_t>(*s++)); };

	for (uint8_t c = *fmt; c != 0 && total >= 0; c = *fmt)
	{
		const uint8_t* next = fmt + 1;
		int cls = (static_cast<int8_t>(c) < ' ' || static_cast<int8_t>(c) > 'x') ? 0 : (k_lookup[c - ' '] & 0xf);
		state = static_cast<int8_t>(k_lookup[cls * 8 + state]) >> 4;
	again:
		switch (state)
		{
		case 0:
			wide = 0;
			if (e->isleadbyte && e->isleadbyte(c)) { put(c); put(*next); ++next; }
			else put(c);
			break;
		case 1:
			prec = -1; hexAdd = 0; suppress = 0; width = 0; prefLen = 0; flags = 0; wide = 0;
			break;
		case 2:
			if (c == ' ') flags |= 2; else if (c == '#') flags |= 0x80; else if (c == '+') flags |= 1;
			else if (c == '-') flags |= 4; else if (c == '0') flags |= 8;
			break;
		case 3:
			if (c == '*') { width = static_cast<int32_t>(*args++); if (width < 0) { flags |= 4; width = -width; } }
			else width = (static_cast<int8_t>(c) - '0') + width * 10;
			break;
		case 4: prec = 0; break;
		case 5:
			if (c == '*') { prec = static_cast<int32_t>(*args++); if (prec < 0) prec = -1; }
			else prec = (static_cast<int8_t>(c) - '0') + prec * 10;
			break;
		case 6:
			if (c == 'I')
			{
				uint8_t n1 = *next;
				if (n1 == '6' && fmt[2] == '4') { next = fmt + 3; flags |= 0x8000; }
				else if (n1 == '3' && fmt[2] == '2') { next = fmt + 3; flags &= ~0x8000u; }
				else if (n1 != 'd' && n1 != 'i' && n1 != 'o' && n1 != 'u' && n1 != 'x' && n1 != 'X')
				{ state = 0; goto again; }
			}
			else if (c == 'h') flags |= 0x20; else if (c == 'l') flags |= 0x10; else if (c == 'w') flags |= 0x800;
			break;
		case 7:
		{
			int radix = 0, isInt = 0, kind = 0; // kind: 1 char, 2 string, 3 float, 4 %Z, 5 %n
			switch (c)
			{
			case 'C': if (!(flags & 0x830)) flags |= 0x800; kind = 1; break;
			case 'c': kind = 1; break;
			case 'S': if (!(flags & 0x830)) flags |= 0x800; kind = 2; break;
			case 's': kind = 2; break;
			case 'Z': kind = 4; break;
			case 'E': case 'G': caps = 1; c += 0x20; kind = 3; break;
			case 'e': case 'f': case 'g': kind = 3; break;
			case 'd': case 'i': flags |= 0x40; radix = 10; isInt = 1; break;
			case 'u': radix = 10; isInt = 1; break;
			case 'o': radix = 8; isInt = 1; if (flags & 0x80) flags |= 0x200; break;
			case 'p': prec = 8; /* fallthrough */
			case 'X': hexAdd = 7; radix = 16; isInt = 1; break;
			case 'x': hexAdd = 0x27; radix = 16; isInt = 1; break;
			case 'n': kind = 5; break;
			default: break;
			}
			if (kind == 1)
			{
				uint64_t a = *args++;
				if (!(flags & 0x810)) { cbuf[0] = static_cast<char>(a); textLen = 1; }
				else { int r = e->wctomb(cbuf, static_cast<char16_t>(a)); textLen = static_cast<size_t>(r); if (r < 0) suppress = 1; }
				text = cbuf; wide = 0;
			}
			else if (kind == 2)
			{
				int lim = prec == -1 ? 0x7fffffff : prec;
				const void* p = reinterpret_cast<const void*>(static_cast<uintptr_t>(*args++));
				if (!(flags & 0x810))
				{
					text = p ? static_cast<const char*>(p) : "(null)";
					const char* q = text;
					for (; lim != 0 && (--lim, *q); ++q) {}
					textLen = static_cast<size_t>(q - text); wide = 0;
				}
				else
				{
					static const char16_t nul[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };
					wtext = p ? static_cast<const char16_t*>(p) : nul; wide = 1;
					const char16_t* q = wtext;
					for (; lim != 0 && (--lim, *q); ++q) {}
					textLen = static_cast<size_t>(q - wtext);
				}
			}
			else if (kind == 4)
			{
				auto* z = reinterpret_cast<const d2lang_ansistr*>(static_cast<uintptr_t>(*args++));
				if (!z || !z->buf) { text = "(null)"; textLen = strlen(text); }
				else
				{
					textLen = z->len;
					if (!(flags & 0x800)) { wide = 0; text = static_cast<const char*>(z->buf); }
					else { textLen = static_cast<int>(textLen) / 2; wide = 1; wtext = static_cast<const char16_t*>(z->buf); }
				}
			}
			else if (kind == 3)
			{
				flags |= 0x40;
				if (prec < 0) prec = 6; else if (prec == 0) { if (c == 'g') prec = 1; } else if (prec > 0x200) prec = 0x200;
				uint64_t d = *args++;
				e->cvt(d, fbuf, c, prec, caps);
				if ((flags & 0x80) && prec == 0) e->forcdecpt(fbuf);
				if (c == 'g' && !(flags & 0x80)) e->cropzeros(fbuf);
				text = fbuf;
				if (fbuf[0] == '-') { flags |= 0x100; text = fbuf + 1; }
				textLen = strlen(text); wide = 0;
			}
			else if (kind == 5)
			{
				void* p = reinterpret_cast<void*>(static_cast<uintptr_t>(*args++));
				if (flags & 0x20) *static_cast<int16_t*>(p) = static_cast<int16_t>(total);
				else *static_cast<int*>(p) = total;
				suppress = 1;
			}
			else if (isInt)
			{
				if (c == 'x' || c == 'X' || c == 'p')
				{
					if (flags & 0x80) { prefix[1] = static_cast<char>(hexAdd + 'Q'); prefix[0] = '0'; prefLen = 2; }
				}
				uint64_t v = *args++;
				if (flags & 0x8000) {}
				else if (flags & 0x20) v = (flags & 0x40) ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(v))) : static_cast<uint16_t>(v);
				else v = (flags & 0x40) ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(v))) : static_cast<uint32_t>(v);
				if ((flags & 0x40) && static_cast<int64_t>(v) < 0) { v = 0 - v; flags |= 0x100; }
				if (!(flags & 0x8000)) v = static_cast<uint32_t>(v);
				if (prec < 0) prec = 1; else { flags &= ~8u; if (prec > 0x200) prec = 0x200; }
				if (v == 0) prefLen = 0;
				char* p = num + sizeof num - 1;
				while (prec > 0 || v != 0)
				{
					int dgt = static_cast<int>(v % radix) + '0'; v /= radix; --prec;
					if (dgt > '9') dgt += hexAdd;
					*p-- = static_cast<char>(dgt);
				}
				textLen = static_cast<size_t>(num + sizeof num - 1 - p);
				text = p + 1; wide = 0;
				if ((flags & 0x200) && (*text != '0' || textLen == 0)) { *p = '0'; ++textLen; text = p; }
			}
			if (!suppress)
			{
				if (flags & 0x40)
				{
					if (flags & 0x100) prefix[0] = '-'; else if (flags & 1) prefix[0] = '+'; else if (flags & 2) prefix[0] = ' ';
					if (flags & 0x103) prefLen = 1;
				}
				int pad = width - prefLen - static_cast<int>(textLen);
				if (!(flags & 0xc)) multi(' ', pad);
				str(prefix, prefLen);
				if ((flags & 8) && !(flags & 4)) multi('0', pad);
				if (!wide || static_cast<int>(textLen) < 1) str(text, static_cast<int>(textLen));
				else
					for (size_t i = 0; i < textLen; ++i)
					{
						int r = e->wctomb(cbuf, wtext[i]);
						if (r < 1) break;
						str(cbuf, r);
					}
				if (flags & 4) multi(' ', pad);
			}
			break;
		}
		}
		fmt = next;
	}
	return total;
}

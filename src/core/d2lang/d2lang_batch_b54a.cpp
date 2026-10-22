// 1.13c D2Lang implementation batch b54-lang-a: MSVC CRT low-io helpers.
#include <cstdint>
#include <cstdio>
#include <cstdlib>

// Seam: retail's DAT_6fc11d14 (handle count) and DAT_6fc11d20 (table of 32-entry
// ioinfo blocks, 0x24 bytes each, flags byte at +4).
extern "C" uint32_t d2lang_crt_nhandle = 0;
extern "C" uint8_t* d2lang_crt_pioinfo[64] = {};

// 1.13c D2Lang+0x05D75: __isatty -- nonzero (0x40, FDEV) when the handle is a device.
extern "C" int d2lang_crt_isatty(int fh)
{
	if (d2lang_crt_nhandle <= (uint32_t)fh)
		return 0;
	return (int)(char)d2lang_crt_pioinfo[fh >> 5][4 + (fh & 0x1f) * 0x24] & 0x40;
}

// 1.13c D2Lang+0x07353: __fptrap -- __amsg_exit(2): runtime error R6002, exit 255.
extern "C" void d2lang_crt_fptrap(void)
{
	fputs("\nrun-time error R6002\n- floating point not loaded\n", stderr);
	_Exit(255);
}

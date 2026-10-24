// D2Lang batch i06: CRT process exit and OS-error mapping.
#include <cstdint>
#include <cstdlib>

// 1.13c D2LANG+0x147D: _doexit(code, quick, retcaller) -- run atexit/terminators then exit.
// Single-threaded wasm: no CRT lock, no TerminateProcess re-entry; retcaller!=0 returns to caller.
static bool g_i06_exiting;
extern "C" void d2lang_crt_doexit(unsigned code, int quick, int retcaller)
{
	if (g_i06_exiting) _Exit((int)code);
	g_i06_exiting = true;
	if (!quick) std::exit((int)code);
	if (retcaller) return;
	_Exit((int)code);
}

// 1.13c D2LANG+0x4F60: __dosmaperr -- map a Win32 error to errno (VS2003 errtable, 45 entries).
static int g_i06_doserrno, g_i06_errno;
extern "C" void d2lang_crt_dosmaperr(unsigned long e)
{
	static const struct { unsigned long oscode; int errnocode; } tab[45] = {
		{1,22},{2,2},{3,2},{4,24},{5,13},{6,9},{7,12},{8,12},{9,12},{10,7},{11,8},{12,22},{13,22},
		{15,2},{16,13},{17,18},{18,2},{33,13},{53,2},{65,13},{67,2},{80,17},{82,13},{83,13},{89,11},
		{108,13},{109,32},{112,28},{114,9},{128,10},{129,10},{130,9},{132,13},{145,41},{158,13},
		{161,2},{164,11},{167,13},{183,17},{206,2},{215,11},{1816,12},{0xE8,32},{0x2000,0},{0,0}};
	g_i06_doserrno = (int)e;
	for (unsigned i = 0; i < 45; ++i)
		if (e == tab[i].oscode && e) { g_i06_errno = tab[i].errnocode; return; }
	g_i06_errno = (e > 0x12 && e < 0x25) ? 13 : (e > 0xbb && e < 0xcb) ? 8 : 22;
}

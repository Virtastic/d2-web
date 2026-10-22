// 1.13c D2Lang implementation batch b55: small CRT routines. Retail globals and
// Win32 calls are reached through the env struct below (no direct Win32).
#include <cstddef>
#include <cstdint>
#include <cstdlib>

struct d2lang_b55_env
{
	int (*newHandler)(size_t);        // DAT_6fc10930
	int  (*doexit)(int, int, int);    // 0x147D
	void (*initTerm)(int);            // 0x2215
	int  (*getLocaleInfoA)(uint32_t lcid, uint32_t type, char* buf, int n); // 0
	int  fileMode;                    // DAT_6fc105d4
	int  appType;                     // DAT_6fc105d8
	void (*rtErrorMsg)(int);          // 0x3ACC
	void (*userBanner)(void);         // DAT_6fc10854
};
static d2lang_b55_env d2lang_b55_env_g;
extern "C" d2lang_b55_env* d2lang_b55_getEnv() { return &d2lang_b55_env_g; }

// 1.13c D2Lang+0x1551
extern "C" void d2lang_b55___exit(int code) { d2lang_b55_env_g.doexit(code, 1, 0); }

// 1.13c D2Lang+0x31C8
extern "C" void d2lang_b55_sub_31C8() { d2lang_b55_env_g.initTerm(4); }

// 1.13c D2Lang+0x4250
extern "C" void* d2lang_b55__memset(void* dst, int val, size_t n)
{
	unsigned char* p = static_cast<unsigned char*>(dst);
	while (n--) *p++ = static_cast<unsigned char>(val);
	return dst;
}

// 1.13c D2Lang+0x6AAA
extern "C" int d2lang_b55___callnewh(size_t size)
{
	return d2lang_b55_env_g.newHandler && d2lang_b55_env_g.newHandler(size) != 0;
}

// 1.13c D2Lang+0x3C44
extern "C" void d2lang_b55___FF_MSGBANNER()
{
	d2lang_b55_env& e = d2lang_b55_env_g;
	if (e.fileMode == 1 || (e.fileMode == 0 && e.appType == 1))
	{
		e.rtErrorMsg(0xfc);
		if (e.userBanner) e.userBanner();
		e.rtErrorMsg(0xff);
	}
}

// 1.13c D2Lang+0x6BBE: LOCALE_IDEFAULTANSICODEPAGE (0x1004) as a number, -1 on failure.
extern "C" long d2lang_b55_sub_6BBE(uint32_t lcid)
{
	char buf[7] = {0};
	if (d2lang_b55_env_g.getLocaleInfoA(lcid, 0x1004, buf, 6) == 0) return -1;
	return atol(buf);
}

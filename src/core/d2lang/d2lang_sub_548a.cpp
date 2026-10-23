// 1.13c D2Lang+0x0548A: __security_error_handler(int code). No Win32 in wasm: the
// handler hook and the message-box sink are reached through the env struct.
#include <cstdint>
#include <cstring>

struct d2lang_548a_env
{
	void (*userHandler)(void);                          // DAT_6fc10924
	int  (*moduleFileName)(char* buf, int n);           // GetModuleFileNameA(0,..)
	void (*messageBox)(const char* text, const char* title, uint32_t flags); // 0x6AC5
	void (*exit_)(int);                                 // 0x1551
};
static d2lang_548a_env d2lang_548a_env_g;
extern "C" d2lang_548a_env* d2lang_548a_getEnv() { return &d2lang_548a_env_g; }

extern "C" void d2lang_548a_security_error_handler(int code)
{
	d2lang_548a_env& e = d2lang_548a_env_g;
	if (!e.userHandler) {
		const char* msg = code == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
		const char* tail = code == 1
			? "A buffer overrun has been detected which has corrupted the program's\ninternal state.  The program cannot safely continue execution and must\nnow be terminated.\n"
			: "The program cannot safely continue execution and must now be terminated.\n"; // ponytail: retail strings 0xCF60/0xD020 paraphrased; UI only
		char name[0x104 + 8];
		char buf[0x300];
		name[0] = 0;
		if (!e.moduleFileName || e.moduleFileName(name, 0x104) == 0) strcpy(name, "<program name unknown>");
		const char* p = name;
		size_t n = strlen(p);
		char trunc[0x44];
		if (n + 11 > 0x3C) { strcpy(trunc, "..."); strcat(trunc, name + n - 0x39 + 0); p = trunc; } // last chars, "..." prefix
		strcpy(buf, msg);
		strcat(buf, "\n\n");
		strcat(buf, "Program: ");
		strcat(buf, p);
		strcat(buf, "\n\n");
		strcat(buf, tail);
		if (e.messageBox) e.messageBox(buf, "Microsoft Visual C++ Runtime Library", 0x12010);
	} else e.userHandler();
	if (e.exit_) e.exit_(3);
}

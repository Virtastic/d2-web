// D2Lang 0x0548A (1.13c): __security_error_handler(int code, arg2). No Win32 in wasm: the user handler (DAT 0x10924),
// GetModuleFileNameA, the message box (0x6AC5) and __exit (0x1551) are reached through the env struct. Both paths end in
// __exit(3); retail's `int3` after it is unreachable.
#include <cstdint>
#include <cstring>

struct d2lang_548a_env
{
	void     (*userHandler)(int32_t code, uint32_t arg2);                    // DAT_6fc10924, called (code, [ebp+0xC])
	uint32_t arg2;                                                           // the second stack word the handler is handed
	uint32_t (*moduleFileName)(uint32_t mod, char* buf, uint32_t n);         // GetModuleFileNameA(0, buf, 0x104)
	int32_t  (*messageBox)(const char* text, const char* title, uint32_t flags); // 0x6AC5
	void     (*exit_)(uint32_t);                                             // 0x1551
};
static d2lang_548a_env d2lang_548a_env_g;
extern "C" d2lang_548a_env* d2lang_548a_getEnv() { return &d2lang_548a_env_g; }

// 1.13c D2Lang+0x0548A
extern "C" void d2lang_548a_security_error_handler(int code)
{
	d2lang_548a_env& e = d2lang_548a_env_g;
	if (e.userHandler) {
		e.userHandler((int32_t)code, e.arg2);
	} else {
		const char* title = code == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
		const char* detail = code == 1
			? "A buffer overrun has been detected which has corrupted the program's\n"
			  "internal state.  The program cannot safely continue execution and must\n"
			  "now be terminated.\n"
			: "A security error of unknown cause has been detected which has\n"
			  "corrupted the program's internal state.  The program cannot safely\n"
			  "continue execution and must now be terminated.\n";
		char name[0x104];
		name[0] = 0;
		if (!e.moduleFileName || e.moduleFileName(0, name, 0x104) == 0) strcpy(name, "<program name unknown>");
		const char* shown = name;
		size_t len = strlen(name);
		if (len + 0xB > 0x3C) { strncpy(name + len - 0x31, "...", 3); shown = name + len - 0x31; }
		char text[0x300];
		strcpy(text, title);
		strcat(text, "\n\n");
		strcat(text, "Program: ");
		strcat(text, shown);
		strcat(text, "\n\n");
		strcat(text, detail);
		if (e.messageBox) e.messageBox(text, "Microsoft Visual C++ Runtime Library", 0x12010);
	}
	if (e.exit_) e.exit_(3);
}

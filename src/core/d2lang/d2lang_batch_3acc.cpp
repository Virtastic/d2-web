// 1.13c D2Lang batch 3acc: MSVC CRT _NMSG_WRITE. Table, stdout/MessageBox sinks come from the env.
#include <cstdint>
#include <cstring>
#include <cstddef>

struct d2lang_3acc_msg { int num; const char* text; }; // DAT_6fc0f3c8, 0x13 entries
struct d2lang_3acc_env
{
	const d2lang_3acc_msg* table;
	int fileMode, appType;                          // DAT_6fc105d4 / DAT_6fc105d8
	void (*writeStdout)(const char*, size_t);       // GetStdHandle(-12)+WriteFile
	const char* (*moduleName)(void);                // GetModuleFileNameA(0); NULL on failure
	void (*messageBox)(const char* text, const char* title, unsigned flags);
};
static d2lang_3acc_env d2lang_3acc_env_g;
extern "C" d2lang_3acc_env* d2lang_3acc_getEnv() { return &d2lang_3acc_env_g; }

// 1.13c D2Lang+0x3ACC
extern "C" void d2lang_3acc__NMSG_WRITE(int num)
{
	const d2lang_3acc_env& e = d2lang_3acc_env_g;
	unsigned i = 0;
	while (i < 0x13 && e.table[i].num != num) i++;
	if (i >= 0x13) return;
	const char* msg = e.table[i].text;
	if (e.fileMode == 1 || (e.fileMode == 0 && e.appType == 1)) { e.writeStdout(msg, strlen(msg)); return; }
	if (num == 0xfc) return;
	char buf[0x1f + 0x104 + 0x100 + 64];
	const char* name = e.moduleName();
	if (!name) name = "<program name unknown>";
	char prog[0x104 + 1];
	strncpy(prog, name, 0x104); prog[0x104] = 0;
	char* pn = prog;
	if (strlen(pn) + 1 > 0x3c) { pn += strlen(pn) + 1 - 0x3c; memcpy(pn, "...", 3); }
	name = pn;
	strcpy(buf, "Runtime Error!\n\nProgram: ");
	strcat(buf, name);
	strcat(buf, "\n\n"); // DAT_6fc0c860
	strcat(buf, msg);
	e.messageBox(buf, "Microsoft Visual C++ Runtime Library", 0x12010);
}

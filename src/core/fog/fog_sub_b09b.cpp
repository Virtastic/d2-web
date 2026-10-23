// 1.13c Fog+0x0B09B -- CRT __security_error_handler: report, then exit(3).
#include <cstdint>
#include <cstring>

namespace {
// Host seams: GetModuleFileNameA, __crtMessageBoxA, _exit, and Fog's DAT_6ff806a8 user handler.
using ModuleNameFn = uint32_t (*)(char* buf, uint32_t n);
using MsgBoxFn = void (*)(const char* text, const char* caption, uint32_t flags);
using ExitFn = void (*)(int);
using HandlerFn = void (*)();
ModuleNameFn g_moduleName = nullptr;
MsgBoxFn g_msgBox = nullptr;
ExitFn g_exit = nullptr;
HandlerFn g_userHandler = nullptr;
}

void fog_securityErrorHandler(int code) {
    if (g_userHandler) {
        g_userHandler();
    } else {
        const char* what = code == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
        const char* tail = code == 1 ? "A buffer overrun has been detected which has corrupted the program's\ninternal state.  The program cannot safely continue execution and must\nnow be terminated.\n"
                                     : "The program cannot safely continue execution and must now be terminated.\n"; // ponytail: tail text from rdata 6ff76470/6ff76530, verify
        char prog[0x104 + 4] = {0};
        if (!g_moduleName || !g_moduleName(prog, 0x104)) strcpy(prog, "<program name unknown>");
        const char* p = prog;
        size_t n = strlen(p);
        char tmp[0x104 + 4];
        if (n + 11 > 0x3c) { memcpy(tmp, p + n - 49, 50); memcpy(tmp, "...", 3); p = tmp; }
        char msg[0x200];
        strcpy(msg, what); strcat(msg, "\n\n"); strcat(msg, "Program: "); strcat(msg, p);
        strcat(msg, "\n\n"); strncat(msg, tail, sizeof msg - strlen(msg) - 1);
        if (g_msgBox) g_msgBox(msg, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
    if (g_exit) g_exit(3);
}

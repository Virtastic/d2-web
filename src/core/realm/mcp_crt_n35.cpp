// D2MCPClient CRT leaves: __chkstk, _NMSG_WRITE, __local_unwind2. Host seams stand in for the process
// state (stderr handle, message box, TEB exception list); the logic follows the decompile.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x02430
// __chkstk(EAX = frame size): touches each 4 KB page below ESP so the guard page grows, then moves ESP.
// wasm linear-memory stack has no guard page and the frame is the compiler's; nothing to probe.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_chkstk_2430(uint32_t) {}

// Seam for _NMSG_WRITE: retail's table at 0x6FA2A218 holds 0x13 {code, message*} pairs (8 bytes each).
struct D2McpNmsgEnv {
    const int32_t*  codes;         // 0x13 codes
    const char* const* messages;   // 0x13 message strings
    int32_t         error_mode;    // __error_mode  0x6FA2AA34
    int32_t         app_type;      // __app_type    0x6FA2AA38
    void (*write_stderr)(const char* s, uint32_t len);          // GetStdHandle(-12)+WriteFile
    void (*message_box)(const char* text, const char* caption, uint32_t flags); // ___crtMessageBoxA
};

// 1.13c D2MCPClient+0x025EC
// _NMSG_WRITE(rterrnum): find the code in the table (miss: nothing). Console mode writes the bare message;
// otherwise (except code 0xFC) builds "Runtime Error!\n\nProgram: <name, tail-truncated to "..."+last 60 chars if
// longer than 59>\n\n<message>" and shows it with caption "Microsoft Visual C++ Runtime Library", flags 0x12010.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_nmsg_write_25ec(int32_t errnum, const D2McpNmsgEnv* env, const char* program) {
    uint32_t i = 0;
    while (i < 0x13 && env->codes[i] != errnum) i++;
    if (i == 0x13) return;
    const char* msg = env->messages[i];
    if (env->error_mode == 1 || (env->error_mode == 0 && env->app_type == 1)) {
        env->write_stderr(msg, (uint32_t)strlen(msg));
    } else if (errnum != 0xfc) {
        char buf[0x1b0];
        const char* name = program && *program ? program : "<program name unknown>";
        size_t n = strlen(name);
        char tail[0x40];
        if (n + 1 > 0x3c) { memcpy(tail, "...", 3); strncpy(tail + 3, name + n - 0x3c + 4, sizeof tail - 4); tail[sizeof tail - 1] = 0; name = tail; }
        strcpy(buf, "Runtime Error!\n\nProgram: ");
        strncat(buf, name, 0x80);
        strcat(buf, "\n\n");
        strncat(buf, msg, sizeof buf - strlen(buf) - 1);
        env->message_box(buf, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}

// 1.13c D2MCPClient+0x0293E
// __local_unwind2(registration, stop): walk the scope table (base *(rec+8), index *(rec+0xc), 12-byte
// entries {prev, filter, handler}) from the current index down to `stop` (-1 = all), storing prev as the new
// index each step and running the __finally handler of entries with no filter. The fs:0 relink is the host's.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_local_unwind2_293e(uint8_t* rec, int32_t stop,
                                                                void (*run)(uint32_t handler)) {
    for (;;) {
        const uint8_t* tbl = reinterpret_cast<const uint8_t*>((uintptr_t)*(uint32_t*)(rec + 8));
        int32_t idx = *(int32_t*)(rec + 0xc);
        if (idx == -1 || idx == stop) break;
        const uint32_t* e = reinterpret_cast<const uint32_t*>(tbl + (uint32_t)idx * 0xc);
        *(int32_t*)(rec + 0xc) = (int32_t)e[0];
        if (e[1] == 0) run(e[2]);
    }
}

// src/core/d2gfx/d2gfx_report_gsfailure.cpp -- 1.13c D2Gfx.dll security-failure report (322 B),
// reimplemented from build-wasm/ghidra-d2gfx/decomp/0x608C.c. Image base 0x6FA80000; every global is
// named at its retail RVA. Callees whose rows are still UNMAPPED, and kernel32 GetModuleFileNameA (the
// wasm module has no PE import table), are host seams after the retail arguments, which keep their order
// (the d2gfx_batch_535e / d2gfx_getptd pattern). __SEH_prolog/__chkstk and the post-__exit `swi 3` are
// compiler glue and are not modelled.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GfxReportGsFailureEnv {
    // kernel32 GetModuleFileNameA(NULL, name, size); 0 = failure (the decompile's DVar2 == 0).
    uint32_t (*get_module_file_name)(uint32_t module, char* name, uint32_t size);
    // 0x059E8 ___crtMessageBoxA(text, caption, type); return value unused by the caller.
    int32_t  (*message_box_a)(const char* text, const char* caption, uint32_t type);
    // 0x0151C __exit(code) = doexit(code, quick=1, returning=0); never returns.
    void     (*exit_)(uint32_t code);
};

// +0x910F4 DAT_6fa910f4: installed security-failure handler, NULL unless the host sets one.
static void (*g_security_failure_handler)(void);

// 1.13c D2Gfx+0x608C (322 B, one caller 0x04262 report_failure with kind 1): MSVC 2003 Release CRT
// security-failure reporter. A non-NULL installed handler is called with no arguments and the message
// path is skipped; otherwise the module path goes into a 0x104-byte buffer ("<program name unknown>" on
// failure), and when strlen + 0xB > 0x3C the shown path becomes name + strlen - 0x31 with "..." written
// there (the last 46 characters behind a three-dot tail). The box text is the kind's title
// (1 = "Buffer overrun detected!", else "Unknown security failure detected!"), "\n\n", "Program: ", the
// shown path, "\n\n" and the kind's detail text -- literals read from this image at 0x6FA8DF48 /
// 0x6FA8E008 / 0x6FA8DF38 / 0x6FA8D740. Shown by ___crtMessageBoxA with the CRT caption and 0x12010.
// Both branches end in __exit(3).
// 1.13c D2Gfx+0x608C
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_608c(int32_t failureKind,
                                            const D2GfxReportGsFailureEnv* env) {
    if (g_security_failure_handler != nullptr) {
        g_security_failure_handler();
    } else {
        const char* const title = failureKind == 1 ? "Buffer overrun detected!"
                                                   : "Unknown security failure detected!";
        const char* const detail = failureKind == 1
            ? "A buffer overrun has been detected which has corrupted the program's\n"
              "internal state.  The program cannot safely continue execution and must\n"
              "now be terminated.\n"
            : "A security error of unknown cause has been detected which has\n"
              "corrupted the program's internal state.  The program cannot safely\n"
              "continue execution and must now be terminated.\n";
        char name[0x104];
        name[0] = '\0';
        if (env->get_module_file_name(0, name, 0x104) == 0)
            std::strcpy(name, "<program name unknown>");
        const char* shown = name;
        const size_t len = std::strlen(name);
        if (len + 0xB > 0x3C) {
            std::strncpy(name + len - 0x31, "...", 3);   // "_strncpy" 0x05AF0
            shown = name + len - 0x31;
        }
        char text[0x300];                                 // the retail alloca, bounded
        std::strcpy(text, title);                         // "_strcpy" 0x058F0
        std::strcat(text, "\n\n");                        // "_strcat" 0x05900
        std::strcat(text, "Program: ");
        std::strcat(text, shown);
        std::strcat(text, "\n\n");
        std::strcat(text, detail);
        env->message_box_a(text, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
    env->exit_(3);
}

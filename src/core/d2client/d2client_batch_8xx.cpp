// src/core/d2client/d2client_batch_8xx.cpp -- eight UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every
// RVA below is retail's. The decompiler did not recover the register arguments of several of these, so
// each rule below is confirmed against the retail disassembly (objdump of the 1.13c PE; .text file
// offset == RVA). Unrecovered callees are host callbacks, after the retail arguments.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

// Intrusive-list unlink shared by 0x1030, 0x10B0, 0x1150 and 0x11F0. A node is two words:
//   [0] successor pointer (0 = not linked), [1] encoded predecessor. A non-negative encoding is
//   relative: node + enc - successor[1]; a negative one is ~target. The successor's back-link is
//   written from the reloaded node words (the store to *target can alias the node itself).
static void d2c_unlink(uint32_t node) {
    uint32_t succ = *(const uint32_t*)(uintptr_t)node;
    if (succ == 0) return;
    uint32_t enc = *(const uint32_t*)(uintptr_t)(node + 4);
    uint32_t target;
    if ((int32_t)enc < 0) {
        target = ~enc;
    } else {
        target = (uint32_t)(node + (enc - *(const uint32_t*)(uintptr_t)(succ + 4)));
    }
    *(uint32_t*)(uintptr_t)target = succ;
    uint32_t s = *(const uint32_t*)(uintptr_t)node;
    uint32_t e = *(const uint32_t*)(uintptr_t)(node + 4);
    *(uint32_t*)(uintptr_t)(s + 4) = e;
    *(uint32_t*)(uintptr_t)node = 0;
    *(uint32_t*)(uintptr_t)(node + 4) = 0;
}

// 1.13c D2Client+0x011F0 (63 B): drain a list whose head word lives at base + 8. The head is read
// again every pass; a value <= 0 (signed) ends the loop, a node with a zero successor is skipped.
// Each listed node is unlinked from its two-word ownership chain.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_11f0(uint32_t base) {
    for (;;) {
        uint32_t node = *(const uint32_t*)(uintptr_t)(base + 8);
        if ((int32_t)node <= 0) return;
        d2c_unlink(node);
    }
}

// 1.13c D2Client+0x01030 (120 B): destructor body -- drain the base+8 list, then unlink the embedded
// node at base+4 (entry base+4, encoding base+8). The SEH frame is an injected check only.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_1030(uint32_t base) {
    d2_client_11f0(base);
    d2c_unlink(base + 4);
}

// 1.13c D2Client+0x01150 (160 B): unlink the given node twice (the second pass is a no-op once the
// node's successor is cleared).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_1150(uint32_t node) {
    d2c_unlink(node);
    d2c_unlink(node);
}

// 1.13c D2Client+0x010B0 (160 B): doubly-linked-list splice. ABI: EAX = list base, ECX = anchor,
// stack = node and mode. A null node/anchor selects the embedded node at list base+4. The node is
// unlinked first, then spliced before the anchor when mode == 1, otherwise after the anchor. The
// back-link stores the caller's node argument (0 included), as retail does.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_10b0(uint32_t list_base, uint32_t anchor, uint32_t node, uint32_t mode) {
    uint32_t a = node ? node : list_base + 4;
    d2c_unlink(a);
    uint32_t b = anchor ? anchor : list_base + 4;
    if (mode == 1) {
        *(uint32_t*)(uintptr_t)a = b;
        uint32_t enc = *(const uint32_t*)(uintptr_t)(b + 4);
        *(uint32_t*)(uintptr_t)(a + 4) = enc;
        uint32_t rel = *(const uint32_t*)(uintptr_t)list_base;
        uint32_t target;
        if ((int32_t)enc < 0) {
            target = ~enc;
        } else {
            if ((int32_t)rel < 0) {
                uint32_t first = *(const uint32_t*)(uintptr_t)b;
                rel = (uint32_t)(b - *(const uint32_t*)(uintptr_t)(first + 4));
            }
            target = (uint32_t)(enc + rel);
        }
        *(uint32_t*)(uintptr_t)target = a;
        *(uint32_t*)(uintptr_t)(b + 4) = node;
        return;
    }
    uint32_t first = *(const uint32_t*)(uintptr_t)b;
    *(uint32_t*)(uintptr_t)a = first;
    *(uint32_t*)(uintptr_t)(a + 4) = *(const uint32_t*)(uintptr_t)(first + 4);
    *(uint32_t*)(uintptr_t)(first + 4) = node;
    *(uint32_t*)(uintptr_t)b = a;
}

// 1.13c D2Client+0x012E0 (31 B): call fn(count) times over an array. ABI: EAX = count, EBX = callee,
// ECX = first object, stack = stride. A count <= 0 performs no call; the callee receives the current
// object as its ECX argument and the object advances by stride between calls.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_12e0(int32_t count, void (*fn)(uint32_t obj), uint32_t obj, int32_t stride) {
    if (count <= 0) return;
    uint32_t p = obj;
    for (int32_t i = 0; i < count; ++i) {
        fn(p);
        p += (uint32_t)stride;
    }
}

// 1.13c D2Client+0x014A0 (105 B): signed (a * b) / d with retail's overflow shortcuts. ABI: EAX = a,
// EDX = b, ECX = d. A zero divisor returns 0. Comparisons are signed. The 32-bit paths multiply
// modulo 2^32 and divide with idiv semantics; anything else uses the full 64-bit product.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_client_14a0(int32_t a, int32_t b, int32_t d) {
    if (d == 0) return 0;
    if (b > 0x100000) {
        if (d <= (b >> 4))
            return (int32_t)((uint32_t)(b / d) * (uint32_t)a);
    } else if (a <= 0x10000) {
        return (int32_t)((uint32_t)b * (uint32_t)a) / d;
    } else if (d <= (a >> 4)) {
        return (int32_t)((uint32_t)(a / d) * (uint32_t)b);
    }
    return (int32_t)(((int64_t)b * (int64_t)a) / (int64_t)d);
}

// Host seams for the two security-failure reporters below.
struct D2ClientBatch8xxEnv {
    uint32_t (*get_module_file_name)(char* buf, uint32_t size);                     // GetModuleFileNameA(NULL,..)
    void     (*message_box)(const char* text, const char* caption, uint32_t type);  // ___crtMessageBoxA
    void     (*exit_)(uint32_t code);                                               // D2Client+0x3675 __exit
    void     (*exit_process)(uint32_t code);                                        // ExitProcess [0x0CEF48]
};

// +0x0FAAC5C DAT_6fbaac5c: installed security-failure handler, NULL unless the host sets one.
static void (*g_security_handler)(int32_t code, uint32_t data);

// 1.13c D2Client+0x0B6AB (322 B): MSVC security-failure reporter. A non-null +0x0FAAC5C handler gets
// (code, data) and the call returns to the exit below; otherwise the module path is fetched into a
// 0x104-byte buffer, defaulted to "<program name unknown>" on failure, and truncated to 49 bytes
// ("..." written at len - 0x31, which also becomes the appended tail) when len + 0xB > 0x3C. The
// message is the title + "\n\nProgram: " + name + "\n\n" + the code's detail text, shown in a modal
// box with type 0x12010. The retail alloca is round4(len + bias + 0xC), bias 0xB9 for code 1 and
// 0xD4 otherwise; a fixed 0x300-byte buffer holds every case. Both paths exit(3).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_b6ab(int32_t code, uint32_t data, const D2ClientBatch8xxEnv* env) {
    if (g_security_handler != 0) {
        g_security_handler(code, data);
        env->exit_(3);
        return;
    }
    // D2Client+0xD0138 / +0xD020C (titles), +0xD0098 / +0xD0158 (details), +0xCF950 (caption).
    const char* title = code == 1 ? "Buffer overrun detected!"
                                  : "Unknown security failure detected!";
    const char* detail = code == 1
        ? "A buffer overrun has been detected which has corrupted the program's\n"
          "internal state.  The program cannot safely continue execution and must\n"
          "now be terminated.\n"
        : "A security error of unknown cause has been detected which has\n"
          "corrupted the program's internal state.  The program cannot safely\n"
          "continue execution and must now be terminated.\n";
    char name[0x104];
    name[0] = '\0';
    if (env->get_module_file_name(name, 0x104) == 0)
        std::strcpy(name, "<program name unknown>");   // D2Client+0xCF998
    const char* shown = name;
    size_t len = std::strlen(name);
    if (len + 0xB > 0x3C) {
        std::strncpy(name + len - 0x31, "...", 3);     // D2Client+0xD4CA8
        shown = name + len - 0x31;
    }
    char msg[0x300];
    std::strcpy(msg, title);
    std::strcat(msg, "\n\n");                          // D2Client+0xCF978
    std::strcat(msg, "Program: ");                     // D2Client+0xD008C
    std::strcat(msg, shown);
    std::strcat(msg, "\n\n");
    std::strcat(msg, detail);
    env->message_box(msg, "Microsoft Visual C++ Runtime Library", 0x12010);
    env->exit_(3);
}

// 1.13c D2Client+0x077A1 (41 B): _report_failure -- report code 1 with no data, then ExitProcess(3)
// through the import slot at +0x0CEF48. The SEH funclets are compiler glue.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77a1(const D2ClientBatch8xxEnv* env) {
    d2_client_b6ab(1, 0, env);
    env->exit_process(3);
}

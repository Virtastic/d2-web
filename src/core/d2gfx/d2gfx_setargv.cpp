// D2Gfx VC7.1 command-line startup. The two-pass parser is the 0x302D routine;
// its first pass counts 32-bit argv slots and string bytes in the CRT block.
#include <cstdint>
#include <emscripten.h>

struct D2GfxSetargvEnv {
    uint32_t mbctable_initialized;
    void (*init_mbctable)(void);
    void (*get_module_file_name)(char*, uint32_t);
    const char* (*get_command_line)(void);
    int32_t (*parse_cmdline)(void* argv, int32_t* slots, const char* command_line,
                             char* string_area);
    void* (*allocate)(uint32_t);
    char module_path[0x104];
    const char* program_path;
    int32_t wildcard;
    int32_t argc;
    void* argv;
};

// 1.13c D2Gfx+0x03199 (__setargv). Initialise the code-page table when needed,
// clear the wildcard word, publish the process module path, and parse the CRT
// command line twice. The first pass sizes one block for 32-bit argv slots and
// argument bytes; the second fills it. Allocation failure returns -1 without
// publishing argc or argv. Otherwise argc excludes the terminating null slot.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_setargv(D2GfxSetargvEnv* env) {
    if (env->mbctable_initialized == 0) env->init_mbctable();
    env->wildcard = 0;
    env->get_module_file_name(env->module_path, 0x104);
    env->program_path = env->module_path;
    const char* command_line = env->get_command_line();  // _acmdln; null or empty -> the module path
    if (command_line == nullptr || *command_line == 0) command_line = env->module_path;
    int32_t slots = 0;
    const int32_t chars = env->parse_cmdline(nullptr, &slots, command_line, nullptr);
    void* block = env->allocate(static_cast<uint32_t>(chars) +
                                static_cast<uint32_t>(slots) * 4u);
    if (block == nullptr) return -1;
    env->parse_cmdline(block, &slots, command_line,
                       static_cast<char*>(block) + static_cast<uint32_t>(slots) * 4u);
    env->argc = slots - 1;
    env->argv = block;
    return 0;
}

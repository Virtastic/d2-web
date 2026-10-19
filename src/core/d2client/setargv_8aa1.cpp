// 1.13c D2Client+0x08AA1 (__setargv). Retail module globals and CRT calls are
// passed through this seam so callers can pin the command line and the allocator.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct D2ClientSetargvState {
    uint32_t mb_table_ready;    // DAT_6fbd4ad8, RVA 0x124AD8
    uint8_t  setargv_static;    // DAT_6fbaab7c, RVA 0x0FAB7C (a byte store in retail)
    char     module_path[0x104];// DAT_6fbaaa78, RVA 0x0FAA78
    char*    pgmptr;            // _DAT_6fbaa8d4, RVA 0x0FA8D4
    const char* command_line;   // DAT_6fbd4ac4, RVA 0x124AC4
    int32_t  argc;              // _DAT_6fbaa8b8, RVA 0x0FA8B8
    char**   argv;              // _DAT_6fbaa8bc, RVA 0x0FA8BC
    void (*init_mb_table)();    // 0x0B627
    uint32_t (*get_module_file_name)(char* buffer, uint32_t size); // GetModuleFileNameA
    // 0x08935: argv slot array (or null), slot count out, command line, string area (or null),
    // character count out. Retail passes the command line in EAX and the string area in ECX.
    void (*parse_cmdline)(char** argv, int32_t* slots, const char* cmd, char* args, int32_t* chars);
    void* (*allocate)(uint32_t retail_size, size_t host_size);    // 0x07D5F
};

// Native pointers are 64-bit in wasm64. Retail sizes the block as char_count + slots*4 (x86
// pointers); the seam receives both that request and the host size for the slot array.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_8aa1(D2ClientSetargvState* state) {
    if (state->mb_table_ready == 0) state->init_mb_table();
    state->setargv_static = 0;
    state->get_module_file_name(state->module_path, 0x104);
    state->pgmptr = state->module_path;
    const char* cmd = state->command_line;
    if (cmd == nullptr || *cmd == '\0') cmd = state->module_path;

    int32_t slots = 0;
    int32_t chars = 0;
    state->parse_cmdline(nullptr, &slots, cmd, nullptr, &chars);

    char** const argv = static_cast<char**>(
        state->allocate((uint32_t)chars + (uint32_t)slots * 4u,
                        (size_t)chars + (size_t)slots * sizeof(char*)));
    if (argv == nullptr) return -1;

    const int32_t slot_bytes = slots * (int32_t)sizeof(char*);
    state->parse_cmdline(argv, &slots, cmd, (char*)argv + slot_bytes, &chars);
    state->argc = slots - 1;
    state->argv = argv;
    return 0;
}

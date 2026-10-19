// 1.13c D2Game+0x0656C (__setargv). Module CRT globals and calls use a pinned host seam.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct D2GameSetargvState {
    uint32_t mb_table_ready;       // DAT_6fd3b358, RVA 0x13B358
    uint8_t setargv_state;         // DAT_6fd2fff4, RVA 0x02FFF4
    char module_path[0x104];      // DAT_6fd2fef0, RVA 0x02FEF0
    char* pgmptr;                  // _DAT_6fd2fd54, RVA 0x02FD54
    int32_t argc;                  // _DAT_6fd2fd38, RVA 0x02FD38
    char** argv;                   // _DAT_6fd2fd3c, RVA 0x02FD3C
    void (*init_mb_table)();       // 0x07DB5
    uint32_t (*get_module_file_name)(char* buffer, uint32_t size); // GetModuleFileNameA
    void (*parse_cmdline)(char** argv, int32_t* slots, const char* source,
                          char* string_area, int32_t* chars); // 0x06400
    void* (*allocate)(uint32_t retail_size, size_t host_size); // 0x05F78
};

// The CRT requests 32-bit pointer slots; the wasm64 seam reserves native-width slots.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_setargv(D2GameSetargvState* state) {
    if (state->mb_table_ready == 0) state->init_mb_table();
    state->setargv_state = 0;
    state->get_module_file_name(state->module_path, 0x104);
    state->pgmptr = state->module_path;

    int32_t slots = 0;
    int32_t chars = 0;
    state->parse_cmdline(nullptr, &slots, state->pgmptr, nullptr, &chars);

    char** const argv = static_cast<char**>(state->allocate(
        static_cast<uint32_t>(chars) + static_cast<uint32_t>(slots) * 4u,
        static_cast<size_t>(chars) + static_cast<size_t>(slots) * sizeof(char*)));
    if (argv == nullptr) return -1;

    state->parse_cmdline(argv, &slots, state->pgmptr,
                         reinterpret_cast<char*>(argv) +
                             static_cast<size_t>(slots) * sizeof(char*),
                         &chars);
    state->argc = slots - 1;
    state->argv = argv;
    return 0;
}

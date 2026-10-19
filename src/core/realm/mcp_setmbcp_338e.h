#pragma once
#include <cstdint>

// State read or written by D2MCPClient's 1.13c __setmbcp wrapper.
struct D2McpSetmbcpState {
    uint32_t codepage;
    uint32_t max_char_size;
    uint32_t locale;
    uint16_t cp_extra[5];
    uint8_t classes[257];
    uint8_t lower[256];
    uint32_t* saved_block;
};

struct D2McpSetmbcpEnv {
    void (*lock)(uint32_t);
    void (*unlock)(uint32_t);
    uint32_t (*get_oem_cp)();
    uint32_t (*get_acp)();
    uint32_t oem_cp;
    void* (*allocate)(uint32_t);
    void (*release)(void*);
    int32_t (*select_codepage)(uint32_t, uint32_t);
};

extern "C" int32_t d2_mcp_setmbcp_338e(int32_t codepage,
                                        D2McpSetmbcpState* state,
                                        const D2McpSetmbcpEnv* env);

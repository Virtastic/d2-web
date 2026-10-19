#pragma once
#include <cstdint>
struct McpStringTypeHost {
    uint32_t wide_state;       // module global +0xAD80: 0 unknown, 1 wide, 2 ANSI
    uint32_t default_locale;   // +0xAD68
    uint32_t default_codepage; // +0xAD78
    int32_t (*wide_type)(uint32_t, const uint16_t*, int32_t, uint16_t*);
    uint32_t (*last_error)();
    int32_t (*to_wide)(uint32_t, uint32_t, const char*, int32_t, uint16_t*, int32_t);
    int32_t (*ansi_codepage)(uint32_t); // +0x5909
    char* (*convert)(uint32_t, uint32_t, const char*, uint32_t*, char*, int32_t); // +0x5950
    int32_t (*ansi_type)(uint32_t, uint32_t, const char*, int32_t, uint16_t*);
    void* (*temporary)(uint32_t); // replaces x86 stack allocation / calloc fallback
    void (*release)(void*);
    void (*free_converted)(void*); // +0x1BB4
};
extern "C" int32_t d2_mcp_crt_getstringtype_4c38(uint32_t,const char*,int32_t,uint16_t*,uint32_t,uint32_t,int32_t,McpStringTypeHost*);

struct McpCodepageHost {
    // GetCPInfo's MaxCharSize, or zero when the API fails.
    uint32_t (*max_char_size)(uint32_t);
    int32_t (*to_wide)(uint32_t,uint32_t,const char*,int32_t,uint16_t*,int32_t);
    int32_t (*from_wide)(uint32_t,uint32_t,const uint16_t*,int32_t,char*,int32_t);
    void* (*allocate)(uint32_t,uint32_t); // calloc(count, size), +0x1C25
    void (*release)(void*);              // free, +0x1BB4
};
extern "C" char* d2_mcp_convert_5950(uint32_t,uint32_t,const char*,uint32_t*,char*,int32_t,const McpCodepageHost*);
extern "C" uint32_t d2_mcp_isctype_5b2a(const uint16_t*,uint32_t,uint32_t,int32_t,uint32_t,McpStringTypeHost*);

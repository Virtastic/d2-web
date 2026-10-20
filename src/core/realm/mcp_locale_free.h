#pragma once
#include <array>
#include <cstdint>

// ABI proof: cached 0x4963.c takes undefined4*, 0x49C2.c takes int; both
// are cdecl with one stack pointer argument. Their loads at four-byte strides
// are native retail addresses, not on-wire integers. Parent 0x2E3B has no
// in-tree callsites; this header is its shared owner-facing native adapter.
// Native ownership model: retail lconv slots +0x00..+0x24 are ten x86
// pointers, not serialized values. Keep their identities at native width.
struct D2McpLconv { std::array<void*, 10> strings; };
struct D2McpLocaleFreeHost {
    const D2McpLconv* current; // PTR_PTR_6fa2a7bc
    const D2McpLconv* initial; // PTR_DAT_6fa2a78c..a7b0
    void (*release)(void*);
};
// Fields named by their retail offset; pointer-bearing objects are native.
struct D2McpLocaleOwned {
    int32_t* ref_2c;
    int32_t* ref_30;
    int32_t* ref_34;
    D2McpLconv* table_3c;
    int32_t* ref_40;
    uint8_t* biased_44;
    void* time_50;
    int32_t* time_references; // retail time_50 +0xb4
};
struct D2McpFreeTlocinfoEnv {
    const D2McpLconv* default_locinfo;
    const int32_t* default_lconv_mon;
    const int32_t* default_lconv_num;
    const int32_t* default_lc_time;
    const void* lc_time_global;
    D2McpLocaleFreeHost strings;
    void (*free_lc_time)(void*);
};
extern "C" void d2_mcp_free_lconv_num(D2McpLconv*, const D2McpLocaleFreeHost*);
extern "C" void d2_mcp_free_lconv_mon(D2McpLconv*, const D2McpLocaleFreeHost*);
extern "C" void d2_mcp_freetlocinfo(D2McpLocaleOwned*, const D2McpFreeTlocinfoEnv*);

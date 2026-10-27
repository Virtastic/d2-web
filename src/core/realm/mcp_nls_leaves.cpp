// Prior art used: BNClient's MSVC classification sibling (bnclient_batch2_crt.cpp)
// and the existing MCP NLS callers. D2MOO's MCP module has only a close stub.
// Windows NLS and allocation results are supplied explicitly; no private CRT state.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "mcp_stringtype.h"



// 1.13c D2MCPClient+0x05950
extern "C" EMSCRIPTEN_KEEPALIVE char* d2_mcp_convert_5950(
    uint32_t from, uint32_t to, const char* source, uint32_t* count,
    char* destination, int32_t capacity, const McpCodepageHost* h) {
    if (from == to) return nullptr; // caller only invokes this on differing codepages
    const uint32_t original = *count;
    const bool single = h->max_char_size(from) == 1 && h->max_char_size(to) == 1;
    uint32_t wide_count;
    if (single) wide_count = original == UINT32_MAX ? uint32_t(std::strlen(source)) + 1 : original;
    else {
        wide_count = uint32_t(h->to_wide(from,1,source,int32_t(original),nullptr,0));
        if (!wide_count) return nullptr;
    }
    // Replaces retail's zeroed stack temporary (with calloc fallback), preserving
    // ownership separately from the allocated result returned to the caller.
    auto* wide = static_cast<uint16_t*>(h->allocate(2,wide_count));
    if (!wide) return nullptr;
    char* result = nullptr;
    if (h->to_wide(from,1,source,int32_t(original),wide,int32_t(wide_count))) {
        if (destination) {
            if (h->from_wide(to,0,wide,int32_t(wide_count),destination,capacity)) result=destination;
        } else {
            // retail reuses local_40: after the sizing call it holds the BYTE count, and that is the wide length of the second convert
            uint32_t bytes = wide_count;
            if (!single) bytes=uint32_t(h->from_wide(to,0,wide,int32_t(wide_count),nullptr,0));
            if (single || bytes) {
                result=static_cast<char*>(h->allocate(1,bytes));
                if (result) {
                    const uint32_t converted=uint32_t(h->from_wide(to,0,wide,int32_t(bytes),result,int32_t(bytes)));
                    if (!converted) { h->release(result); result=nullptr; }
                    else if (original != UINT32_MAX) *count=converted;
                }
            }
        }
    }
    h->release(wide);
    return result;
}

// 1.13c D2MCPClient+0x05B2A
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_isctype_5b2a(
    const uint16_t* ctype, uint32_t codepage, uint32_t locale,
    int32_t character, uint32_t mask, McpStringTypeHost* host) {
    // ctype points after the EOF entry, so index -1 is valid.
    if (uint32_t(character)+1u < 0x101u) return ctype[character] & mask;
    const uint32_t high=(uint32_t(character)>>8)&0xff;
    char bytes[2];
    int32_t count;
    if (ctype[high]&0x8000) {
        bytes[0]=char(high); bytes[1]=char(character); count=2;
    } else { bytes[0]=char(character); count=1; }
    uint16_t flags=0;
    if (!d2_mcp_crt_getstringtype_4c38(1,bytes,count,&flags,codepage,locale,1,host)) return 0;
    return flags & mask;
}

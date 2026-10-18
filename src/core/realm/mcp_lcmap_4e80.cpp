// CRT locale mapping dispatch; host locale answers are explicit seams.
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

struct D2McpLcMapHost {
    int32_t (*map_w)(uint32_t,uint32_t,const uint16_t*,int32_t,uint16_t*,int32_t);
    uint32_t (*last_error)();
    int32_t (*to_w)(uint32_t,uint32_t,const char*,int32_t,uint16_t*,int32_t);
    int32_t (*from_w)(uint32_t,uint32_t,const uint16_t*,int32_t,char*,int32_t);
    int32_t (*map_a)(uint32_t,uint32_t,const char*,int32_t,char*,int32_t);
    uint32_t (*locale_cp)(uint32_t);
    // 0x5950: allocated result when dst is null; updates count; caller owns it.
    char* (*convert)(uint32_t,uint32_t,const char*,int32_t*,char*,int32_t);
    void (*release)(void*);
};
struct D2McpLcMapState {
    uint32_t dispatch; // DAT_6fa2ada8: 0 unknown, 1 Unicode, 2 ANSI
    uint32_t codepage; // DAT_6fa2ad78
    uint32_t locale;   // DAT_6fa2ad68
};

// 1.13c D2MCPClient+0x04E80
extern "C" uint32_t d2_mcp_lcmap_4e80(D2McpLcMapState* state, const D2McpLcMapHost* h,
    uint32_t locale, uint32_t flags, const char* src, int32_t count, char* dst,
    int32_t capacity, uint32_t cp, int32_t strict) {
    if (!state->dispatch) {
        const uint16_t empty[] = {0};
        if (h->map_w(0,0x100,empty,1,nullptr,0)) state->dispatch=1;
        else if (h->last_error()==0x78) state->dispatch=2;
    }
    if (count>0) {
        int32_t n=0;
        while (n<count && src[n]) ++n;
        count=n; // positive counts exclude the first NUL, including an initial NUL
    }
    if (state->dispatch!=0 && state->dispatch!=2) {
        if (state->dispatch!=1) return 0;
        if (!cp) cp=state->codepage;
        int32_t n=h->to_w(cp,strict ? 9:1,src,count,nullptr,0);
        if (!n) return 0;
        std::vector<uint16_t> wide(static_cast<uint32_t>(n));
        if (!h->to_w(cp,1,src,count,wide.data(),n)) return 0;
        uint32_t result=h->map_w(locale,flags,wide.data(),n,nullptr,0);
        if (!result) return 0;
        if (flags&0x400) {
            // SORTKEY returns bytes. Retail retains the queried result even if the fill fails.
            if (capacity!=0 && static_cast<int32_t>(result)<=capacity)
                h->map_w(locale,flags,wide.data(),n,reinterpret_cast<uint16_t*>(dst),capacity);
            return result;
        }
        std::vector<uint16_t> mapped(result);
        if (h->map_w(locale,flags,wide.data(),n,mapped.data(),static_cast<int32_t>(result)))
            result=h->from_w(cp,0,mapped.data(),static_cast<int32_t>(result),capacity ? dst:nullptr,capacity ? capacity:0);
        return result;
    }
    if (!locale) locale=state->locale;
    if (!cp) cp=state->codepage;
    uint32_t locale_cp=h->locale_cp(locale);
    if (locale_cp==0xffffffffu) return 0;
    if (locale_cp==cp) return h->map_a(locale,flags,src,count,dst,capacity);
    char* converted=h->convert(cp,locale_cp,src,&count,nullptr,0);
    if (!converted) return 0;
    std::unique_ptr<char,void(*)(void*)> input(converted,h->release);
    int32_t mapped_count=h->map_a(locale,flags,input.get(),count,nullptr,0);
    if (!mapped_count) return 0;
    std::vector<char> mapped(static_cast<uint32_t>(mapped_count),0);
    mapped_count=h->map_a(locale,flags,input.get(),count,mapped.data(),mapped_count);
    if (!mapped_count) return 0;
    char* output=h->convert(locale_cp,cp,mapped.data(),&mapped_count,dst,capacity);
    if (!output) return 0;
    if (output!=dst) h->release(output);
    return static_cast<uint32_t>(mapped_count);
}

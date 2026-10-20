// Prior art: MSVC 2003 CRT locale ownership, cross-checked against the
// BNClient+0x6975/0x69d4 reading. Retail 1.13c cached bodies settle bounds:
// monetary owns SEVEN pointers (+0x0c..+0x24), numeric owns three.
#include "mcp_locale_free.h"

namespace {
void release_fields(D2McpLconv* table, const D2McpLocaleFreeHost* h,
                    unsigned first, unsigned end) {
    if (!table) return;
    for (unsigned i = first; i < end; ++i) {
        void* value = table->strings[i];
        if (value != h->current->strings[i] && value != h->initial->strings[i])
            h->release(value); // retail also passes null to _free
    }
}
}
// 1.13c D2MCPClient+0x04963
extern "C" void d2_mcp_free_lconv_num(D2McpLconv* table, const D2McpLocaleFreeHost* h) {
    release_fields(table, h, 0, 3);
}
// 1.13c D2MCPClient+0x049C2
extern "C" void d2_mcp_free_lconv_mon(D2McpLconv* table, const D2McpLocaleFreeHost* h) {
    release_fields(table, h, 3, 10);
}
// 1.13c D2MCPClient+0x02E3B
// Native adapter for retail's pointer-bearing locale object. Defaults and
// allocator come from its owner; no second locale state or integer handles.
extern "C" void d2_mcp_freetlocinfo(D2McpLocaleOwned* p, const D2McpFreeTlocinfoEnv* h) {
    auto release = h->strings.release;
    if (p->table_3c != h->default_locinfo && p->table_3c && *p->ref_2c == 0) {
        if (p->ref_34 && *p->ref_34 == 0 && p->ref_34 != h->default_lconv_mon) {
            release(p->ref_34);
            d2_mcp_free_lconv_mon(p->table_3c, &h->strings);
        }
        if (p->ref_30 && *p->ref_30 == 0 && p->ref_30 != h->default_lconv_num) {
            release(p->ref_30);
            d2_mcp_free_lconv_num(p->table_3c, &h->strings);
        }
        release(p->ref_2c);
        release(p->table_3c);
    }
    if (p->ref_40 != h->default_lc_time && p->ref_40 && *p->ref_40 == 0) {
        release(p->ref_40);
        release(p->biased_44 - 0xfe);
    }
    if (p->time_50 != h->lc_time_global && p->time_50 && *p->time_references == 0) {
        h->free_lc_time(p->time_50);
        release(p->time_50);
    }
    release(p);
}

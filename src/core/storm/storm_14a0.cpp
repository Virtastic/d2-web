// Storm list-record allocator.
// Allocate a list record of 0x30 + extra bytes, zero its two link words, and when mode != 0 link it into `list`
// through Storm+0x4390 (self = 0). The allocator (retail Storm+0x1C580, tagged with the record's type name,
// line -2, flags | 8) is a seam.
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32;

extern "C" void storm_sub_4390(i32* list /*eax*/, i32* self /*ecx*/, i32* node, i32 mode);

// 1.13c Storm+0x014A0  cdecl(list, extra, flags), hidden edi = mode
extern "C" u32* storm_14a0(i32* list, i32 extra, u32 flags, i32 mode /*edi*/,
                           u32* (*alloc)(i32 size, i32 line, u32 flags)) {
    u32* p = alloc(extra + 0x30, -2, flags | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (mode != 0) storm_sub_4390(list, nullptr, (i32*)p, mode);
    return p;
}

// The same constructor body recurs at 1.13c Storm+0x1290 (AUINST, +0x18), +0x1560 (AUBREAKCMD, +0xC) and
// +0x15E0 (AUWNDREC, +0xC): stdcall(list, extra, flags), hidden EDI = mode, ret 0xC; only the header size
// and the allocator tag differ.
extern "C" void* Storm_Sub1C580(u32 size, const char* file, i32 line, u32 flags);
extern "C" u32* storm_list_record_new(i32* list, i32 extra, u32 flags, i32 mode, i32 hdr, const char* tag) {
    u32* p = (u32*)Storm_Sub1C580((u32)(extra + hdr), tag, -2, flags | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (mode != 0) storm_sub_4390(list, nullptr, (i32*)p, mode);
    return p;
}

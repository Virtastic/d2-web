// src/core/gfx/fog_batch_1400.cpp -- twelve UNMAPPED 1.13c Fog.dll rows reimplemented from the
// cached Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c) and confirmed against the retail
// disassembly in retail/D2/D2SE/CORES/1.13c/Fog.dll (base 0x6FF50000). Alpha lane: no spec, no vector.
//
// Rows 0x1400..0x17D0 are one array/priority-queue type the MSVC 2003 compiler instantiated into Fog.dll:
// +0x00 capacity, +0x04 end (one past the last element), +0x08 element-pointer array, +0x0C growth block
// (0 means "ask Fog+0x1790"), +0x10 byte offset from each stored pointer to its node. A node is
// { vtable, owner, index }: +0x04 gets the array pointer and +0x08 the node's own slot, and its virtual
// slot 0 is the heap comparator (retail thiscall, one pointer argument on the stack). The array's four
// allocator calls are Storm #405 (realloc, Fog+0xCE7E), #401 (alloc, Fog+0xCDC4) and #403 (free,
// Fog+0xCDCA); this port has one heap, so they are std::realloc/malloc/free (the D2DDraw CRT batch,
// src/core/gfx/d2ddraw_batch_rowex.cpp, uses the same correspondence for _realloc/__msize).
//
// Rows 0x184D..0x1A22 are MSVC 2003 CRT glue: the multibyte __tolower_mt/__toupper_mt pair, their
// _tolower/_toupper wrappers, and ___crtExitProcess. Prior art: src/core/gfx/d2ddraw_batch_rowex.cpp
// models the same _crtExitProcess in D2DDraw.dll and the same one-heap substitution; the locale fields
// and the two locale seams are read off the Fog decompiles, not from D2MOO (its Fog module has no such
// source, the CRT came from the linker).
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

namespace {

// ---- the Fog array/priority-queue type (+0x1400, 0x1520, 0x1630, 0x1670, 0x16B0, 0x1750, 0x17D0) ----
struct FogArray {
    uint32_t capacity;    // +0x00
    uint32_t end;         // +0x04  one past the last element
    uint8_t** items;      // +0x08
    uint32_t blockSize;   // +0x0C  0 => fog_next_block_size
    uint32_t nodeBase;    // +0x10  added to every items[i]
};

struct FogNodeVtbl {
    // Retail vtable slot 0: thiscall, `this` = the node, `other` the stack argument.
    int (*less)(void* self, void* other);
};

struct FogNode {
    const FogNodeVtbl* vtbl;  // +0x00
    FogArray* owner;          // +0x04
    uint32_t index;           // +0x08
};

// A handle/iterator object (+0x16B0, and Fog+0x1390 outside this batch): vtable, array, its slot.
struct FogHandle {
    const void* vtbl;         // +0x00
    FogArray* array;          // +0x04
    uint32_t index;           // +0x08
};

static uint8_t* fog_array_node(const FogArray* a, uint32_t i) {
    return a->items[i] + a->nodeBase;
}

static int fog_node_less(void* self, void* other) {
    const FogNode* n = static_cast<const FogNode*>(self);
    return n->vtbl->less(self, other);
}

// Fog+0x1790 (already mapped:): below 0x40 the highest power of two not
// exceeding n (1 for 0); 0x40 and above stores 0x40 in the growth block and returns 0x40.
static uint32_t fog_next_block_size(FogArray* a, uint32_t n) {
    if (n < 0x40) {
        uint32_t r = (n - 1) & n;
        while (r != 0) {
            n = r;
            r = (r - 1) & r;
        }
        return n == 0 ? 1 : n;
    }
    a->blockSize = 0x40;
    return 0x40;
}

// Fog+0x17D0: resize the element array to `capacity`. Retail passes the capacity on the stack and the
// array in ESI; realloc keeps the elements, and on failure a fresh allocation copies min(capacity, end)
// entries before freeing the old block. Allocation failures are not otherwise signalled.
static void fog_array_resize(FogArray* a, uint32_t capacity) {
    uint8_t** old = a->items;
    a->capacity = capacity;
    uint8_t** p = static_cast<uint8_t**>(std::realloc(old, capacity * sizeof(uint8_t*)));
    a->items = p;
    if (p != nullptr) return;
    p = static_cast<uint8_t**>(std::malloc(capacity * sizeof(uint8_t*)));
    a->items = p;
    if (old == nullptr) return;
    uint32_t n = capacity < a->end ? capacity : a->end;
    for (uint32_t i = 0; i < n; ++i) p[i] = old[i];
    std::free(old);
}

// Fog+0x1400 (and its two unwind funclets, Fog+0x1630 and Fog+0x1670): unlink every node in
// [0, end), then free the pointer array. The array pointer is not cleared and the object is not
// otherwise touched. The funclets only install an SEH frame around the same call.
static void fog_array_clear(FogArray* a) {
    for (uint32_t i = 0; i < a->end; ++i) {
        uint8_t* node = fog_array_node(a, i);
        std::memset(node + 4, 0, 8);
    }
    if (a->items != nullptr) std::free(a->items);
}

// Fog+0x1520: remove the node at `index` from the array and restore the heap order below it. The
// removed node's owner/index links are cleared first; the last element fills the hole and sifts down,
// choosing the left or right child by the right child's comparator and stopping while the moved value
// compares less than the child. Retail's shrink guard compares the new end against the old end and
// capacity; with end >= 1 the first comparison is false, so the guard is only reachable from end == 0
// and is transcribed as written.
static void fog_array_remove_at(FogArray* a, uint32_t index) {
    uint8_t* removed = fog_array_node(a, index);
    std::memset(removed + 4, 0, 8);

    uint32_t oldEnd = a->end;
    uint8_t* last = oldEnd != 0 ? a->items[oldEnd - 1] : nullptr;
    uint32_t newEnd = oldEnd - 1;

    if (newEnd > a->end && newEnd > a->capacity) {
        uint32_t block = a->blockSize;
        if (block == 0) block = fog_next_block_size(a, newEnd);
        uint32_t cap = newEnd;
        if (newEnd % block != 0) cap = newEnd + (block - newEnd % block);
        fog_array_resize(a, cap);
    }
    a->end = newEnd;
    if (newEnd == index) return;

    uint32_t lastIndex = oldEnd - 2;          // newEnd - 1
    uint32_t parent = (oldEnd - 3) >> 1;      // (lastIndex - 1) / 2
    if (index >= lastIndex || index > parent) {
        a->items[index] = last;
        FogNode* n = reinterpret_cast<FogNode*>(fog_array_node(a, index));
        n->owner = a;
        n->index = index;
        return;
    }
    for (;;) {
        uint32_t child = index * 2 + 1;
        if (child < lastIndex && fog_node_less(fog_array_node(a, child + 1), fog_array_node(a, child)))
            ++child;
        if (fog_node_less(last, fog_array_node(a, child))) break;
        a->items[index] = a->items[child];
        FogNode* n = reinterpret_cast<FogNode*>(fog_array_node(a, index));
        n->owner = a;
        n->index = index;
        index = child;
        if (index > parent) break;
    }
    a->items[index] = last;
    FogNode* n = reinterpret_cast<FogNode*>(fog_array_node(a, index));
    n->owner = a;
    n->index = index;
}

// Fog+0x16B0: destroy a handle object. Reinstall the vtable, then unlink its slot when it holds an
// array; the array and index fields are left as they were.
static void fog_handle_destroy(FogHandle* h) {
    static const uint32_t kFogHandleVtable[1] = {0};  // PTR_LAB_6ff78388, no entries reachable here
    h->vtbl = kFogHandleVtable;
    if (h->array != nullptr) fog_array_remove_at(h->array, h->index);
}

// Fog+0x1750: release the element array (register argument in EAX). The pointer is not cleared.
static void fog_array_free_items(FogArray* a) {
    if (a->items != nullptr) std::free(a->items);
}

// ---- MSVC 2003 locale helpers (+0x184D, 0x1915, 0x1937, 0x1A00) ---------------------------------
// The retail offsets into __crt_locale_data are kept in the comments; only the fields read here exist.
struct FogCrtLocale {
    uint32_t flags;            // +0x04  locale flags word, the 7th __crtLCMapStringA argument
    uint32_t lcid;             // +0x14  0 means the C locale
    uint32_t mbcs;             // +0x24  non-zero when a multibyte code page is active
    uint32_t mbcurmax;         // +0x28  1 for a single-byte code page
    const uint8_t* ctype;      // +0x48  two-byte entries: [0] type bits, [1] lead-byte flag 0x80
};

// Type bits used by the two entry points: Fog tests bit 0 for lower and bit 1 for upper.
enum { FOG_CTYPE_LOWER = 1, FOG_CTYPE_UPPER = 2 };

// Fog+0x3279 __crtLCMapStringA; the port has no locale tables, so with no seam installed the mapping
// fails and both entry points return the input byte(s) unchanged.
typedef int (*FogLcmapStringA)(uint32_t lcid, uint32_t mapFlags, const char* src, int srcLen,
                               char* dst, int dstLen, uint32_t localeFlags, int isUnicode);
static FogLcmapStringA g_lcmap_string = nullptr;

// Fog+0x364F ___isctype_mt, used only for mbcurmax >= 2. ASCII is the default class set.
typedef uint32_t (*FogIsctypeMt)(const FogCrtLocale* locale, uint32_t c, uint32_t type);
static uint32_t fog_ascii_isctype(const FogCrtLocale*, uint32_t c, uint32_t type) {
    if (c >= 0x80) return 0;
    if (type == FOG_CTYPE_LOWER) return c >= 'a' && c <= 'z';
    if (type == FOG_CTYPE_UPPER) return c >= 'A' && c <= 'Z';
    return 0;
}
static FogIsctypeMt g_isctype_mt = fog_ascii_isctype;

// __getptd()->_tfpecode, PTR_DAT_6ff7e0d4 and ___updatetlocinfo (Fog+0x3857, 0x38CB). The default
// locale is the C locale, whose lcid is 0 and which therefore takes the ASCII branch.
static FogCrtLocale g_default_locale = {0, 0, 0, 1, nullptr};
static FogCrtLocale* g_ptd_tfpecode = &g_default_locale;
static FogCrtLocale* (*g_update_tlocinfo)(void) = nullptr;

// Fog+0x184D ___tolower_mt (this in ECX, locale and character on the stack): C locale or an ASCII
// byte under a multibyte code page maps 'A'..'Z' by +0x20 and leaves everything else alone. Otherwise
// a single byte below 0x100 is first tested against the lower-case bit of the locale table (or
// _isctype_mt when mbcurmax >= 2); a failing test returns the input. The character is then encoded
// lead byte first when the high byte is flagged as a DBCS lead, and __crtLCMapStringA with
// LCMAP_LOWERCASE (0x100) maps it; a one-byte result is returned in the low byte, a two-byte result
// byte-swapped as retail's CONCAT11 does, and a failed map returns the input.
static uint32_t fog_tolower_mt(const FogCrtLocale* locale, uint32_t c) {
    if (locale->lcid == 0 || (locale->mbcs != 0 && c < 0x80)) {
        if (c > 0x40 && c < 0x5B) return c + 0x20;
        return c;
    }
    if (c < 0x100) {
        uint32_t is;
        if (locale->mbcurmax < 2)
            is = locale->ctype[static_cast<size_t>(c) * 2] & FOG_CTYPE_LOWER;
        else
            is = g_isctype_mt(locale, c, FOG_CTYPE_LOWER);
        if (is == 0) return c;
    }
    char src[2];
    char dst[2] = {0, 0};
    int len;
    if ((locale->ctype[1 + ((c >> 8) & 0xFF) * 2] & 0x80) == 0) {
        src[0] = static_cast<char>(c);
        len = 1;
    } else {
        src[0] = static_cast<char>(c >> 8);
        src[1] = static_cast<char>(c);
        len = 2;
    }
    if (g_lcmap_string == nullptr) return c;
    int n = g_lcmap_string(locale->lcid, 0x100, src, len, dst, 3, locale->flags, 1);
    if (n == 0) return c;
    if (n != 1)
        return (static_cast<uint32_t>(static_cast<uint8_t>(dst[0])) << 8) |
               static_cast<uint8_t>(dst[1]);
    return static_cast<uint8_t>(dst[0]);
}

// Fog+0x1937 ___toupper_mt: the same contract with 'a'..'z' by -0x20, the upper-case table bit and
// LCMAP_UPPERCASE (0x200).
static uint32_t fog_toupper_mt(const FogCrtLocale* locale, uint32_t c) {
    if (locale->lcid == 0 || (locale->mbcs != 0 && c < 0x80)) {
        if (c > 0x60 && c < 0x7B) return c - 0x20;
        return c;
    }
    if (c < 0x100) {
        uint32_t is;
        if (locale->mbcurmax < 2)
            is = locale->ctype[static_cast<size_t>(c) * 2] & FOG_CTYPE_UPPER;
        else
            is = g_isctype_mt(locale, c, FOG_CTYPE_UPPER);
        if (is == 0) return c;
    }
    char src[2];
    char dst[2] = {0, 0};
    int len;
    if ((locale->ctype[1 + ((c >> 8) & 0xFF) * 2] & 0x80) == 0) {
        src[0] = static_cast<char>(c);
        len = 1;
    } else {
        src[0] = static_cast<char>(c >> 8);
        src[1] = static_cast<char>(c);
        len = 2;
    }
    if (g_lcmap_string == nullptr) return c;
    int n = g_lcmap_string(locale->lcid, 0x200, src, len, dst, 3, locale->flags, 1);
    if (n == 0) return c;
    if (n != 1)
        return (static_cast<uint32_t>(static_cast<uint8_t>(dst[0])) << 8) |
               static_cast<uint8_t>(dst[1]);
    return static_cast<uint8_t>(dst[0]);
}

// Fog+0x1915 _tolower: take __getptd()->_tfpecode; when it is not the default locale pointer run
// ___updatetlocinfo and use the locale it returns, then map through ___tolower_mt.
static uint32_t fog_tolower(int c) {
    FogCrtLocale* locale = g_ptd_tfpecode;
    if (locale != &g_default_locale && g_update_tlocinfo != nullptr) locale = g_update_tlocinfo();
    return fog_tolower_mt(locale, static_cast<uint32_t>(c));
}

// Fog+0x1A00 _toupper: the same wrapper around ___toupper_mt.
static uint32_t fog_toupper(int c) {
    FogCrtLocale* locale = g_ptd_tfpecode;
    if (locale != &g_default_locale && g_update_tlocinfo != nullptr) locale = g_update_tlocinfo();
    return fog_toupper_mt(locale, static_cast<uint32_t>(c));
}

// Fog+0x1A22 ___crtExitProcess: when mscoree.dll is loaded and exports CorExitProcess, call it with
// the code, then ExitProcess(code), which never returns. Neither kernel object exists under wasm;
// like D2DDraw's copy of this row the terminating call is a host seam and with no seam installed the
// code is simply consumed.
static void (*g_cor_exit_process)(int) = nullptr;  // mscoree.dll!CorExitProcess
static void (*g_exit_process)(int) = nullptr;      // kernel32!ExitProcess

static void fog_crt_exit_process(int code) {
    if (g_cor_exit_process != nullptr) g_cor_exit_process(code);
    if (g_exit_process != nullptr) g_exit_process(code);
}

}  // namespace

// Probe record ABI: arguments in retail register/stack order starting at r[0], result in r[4].
extern "C" {

// 1.13c Fog+0x1400
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1400(uint32_t* r) {
    fog_array_clear(reinterpret_cast<FogArray*>(static_cast<uintptr_t>(r[0])));
}

// 1.13c Fog+0x1520
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1520(uint32_t* r) {
    fog_array_remove_at(reinterpret_cast<FogArray*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c Fog+0x1630
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1630(uint32_t* r) {
    fog_array_clear(reinterpret_cast<FogArray*>(static_cast<uintptr_t>(r[0])));
}

// 1.13c Fog+0x1670
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1670(uint32_t* r) {
    fog_array_clear(reinterpret_cast<FogArray*>(static_cast<uintptr_t>(r[0])));
}

// 1.13c Fog+0x16B0
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16b0(uint32_t* r) {
    fog_handle_destroy(reinterpret_cast<FogHandle*>(static_cast<uintptr_t>(r[0])));
}

// 1.13c Fog+0x1750
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1750(uint32_t* r) {
    fog_array_free_items(reinterpret_cast<FogArray*>(static_cast<uintptr_t>(r[0])));
}

// 1.13c Fog+0x17D0
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_17d0(uint32_t* r) {
    fog_array_resize(reinterpret_cast<FogArray*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c Fog+0x184D
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_184d(uint32_t* r) {
    r[4] = fog_tolower_mt(reinterpret_cast<const FogCrtLocale*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c Fog+0x1915
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1915(uint32_t* r) {
    r[4] = fog_tolower(static_cast<int>(r[0]));
}

// 1.13c Fog+0x1937
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1937(uint32_t* r) {
    r[4] = fog_toupper_mt(reinterpret_cast<const FogCrtLocale*>(static_cast<uintptr_t>(r[0])), r[1]);
}

// 1.13c Fog+0x1A00
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1a00(uint32_t* r) {
    r[4] = fog_toupper(static_cast<int>(r[0]));
}

// 1.13c Fog+0x1A22
EMSCRIPTEN_KEEPALIVE void d2_fn_fog_1a22(uint32_t* r) {
    fog_crt_exit_process(static_cast<int>(r[0]));
}

}  // extern "C"

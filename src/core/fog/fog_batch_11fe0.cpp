// src/core/fog/fog_batch_11fe0.cpp -- eight 1.13c Fog.dll alpha-lane rows from the string/code
// linker (Source/Fog/Src/Excel/Excel.cpp), reimplemented from the cached Ghidra decompiles
// (build-wasm/ghidra-fog/decomp/0x<RVA>.c) and the disassembly of retail/D2/D2SE/CORES/1.13c/Fog.dll.
// Base 0x6FF50000. Rows: string-from-row (+0x11FE0), tree insert (+0x12020), node free (+0x120A0),
// bin-file free (+0x120E0), linker free (+0x12120), row lookup (+0x121A0), linker alloc (+0x12720),
// SHA-1 state init (+0x134E0).
//
// Linker layout (D2TxtLinkStrc, 0x10 bytes): nSize +0x00, code-table capacity +0x04, pTbl +0x08,
// string-tree root +0x0C. String node (0x2C bytes): szKey[0x20] +0x00, nIndex +0x20, left +0x24,
// right +0x28. The still UNMAPPED Fog memory rows +0x1B560 (alloc) / +0x1B0C0 (free) are host seams
// (std::malloc/std::free) here; +0x10F30 is the reporter seam.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <emscripten.h>

extern "C" void* fog_sub_11DF0(void* node, int32_t key);  // Fog+0x11DF0, fog_batch_102C0.cpp

namespace {

constexpr const char* kExcelFile = "..\\Source\\Fog\\Src\\Excel\\Excel.cpp";

// Fog+0x1B560 alloc seam: size, zero flag (ECX), file, line.
void* fog_alloc_block(uint32_t size, uint32_t zero, const char* file, uint32_t line) {
    (void)zero; (void)file; (void)line;
    return std::malloc(size);
}

// Fog+0x1B0C0 free seam (the retail entry takes the address of the caller's slot; the pool free
// itself is the effect).
void fog_free_block(void* block, const char* file, uint32_t line) {
    (void)file; (void)line;
    std::free(block);
}

// Fog+0x10F30 fatal reporter seam: logs the record, Win32 dialog/file effects omitted.
void fog_error_report(int32_t type, const char* file, uint32_t line, const char* format, ...) {
    std::fprintf(stderr, "Fog error %d at %s:%u: ", static_cast<int>(type), file ? file : "", line);
    va_list ap;
    va_start(ap, format);
    std::vfprintf(stderr, format, ap);
    va_end(ap);
    std::fputc('\n', stderr);
}

uint32_t fog_capture_ip(void) { return 0; }        // Fog+0xFC20 seam
void fog_stack_dump(void) {}                       // Fog+0xCD8E seam
void fog_abort(void) {}                            // Fog+0x684A0 seam
[[noreturn]] void fog_exit_process(int32_t code) { std::_Exit(code); }  // Fog+0x1BA9

// Storm #501 SStrCopy: copy at most max-1 bytes, always terminate.
void fog_str_copy(char* dest, const char* src, uint32_t max) {
    if (max == 0) return;
    uint32_t i = 0;
    while (i + 1 < max && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// Fog+0x6FF76610, the 0x80 bytes before the case-fold table at +0x6FF76690. Retail indexes the fold
// table with a SIGN-EXTENDED key byte, so 0x80..0xFF read this prefix. Index here is 0x80 + (int8_t)c.
const uint8_t kFoldPrefix[0x80] = {
    0xda, 0xb0, 0xf5, 0x6f, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x95, 0xb4, 0xf5, 0x6f, 0x65, 0x2b, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc0, 0x7e, 0x01, 0x50, 0x41, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x47, 0x41,
    0x49, 0x73, 0x50, 0x72, 0x6f, 0x63, 0x65, 0x73, 0x73, 0x6f, 0x72, 0x46, 0x65, 0x61, 0x74, 0x75,
    0x72, 0x65, 0x50, 0x72, 0x65, 0x73, 0x65, 0x6e, 0x74, 0x00, 0x00, 0x00, 0x4b, 0x45, 0x52, 0x4e,
    0x45, 0x4c, 0x33, 0x32, 0x00, 0x00, 0x00, 0x00, 0x31, 0x23, 0x51, 0x4e, 0x41, 0x4e, 0x00, 0x00,
    0x31, 0x23, 0x49, 0x4e, 0x46, 0x00, 0x00, 0x00, 0x31, 0x23, 0x49, 0x4e, 0x44, 0x00, 0x00, 0x00,
    0x31, 0x23, 0x53, 0x4e, 0x41, 0x4e, 0x00, 0x00, 0x3c, 0x4e, 0x55, 0x4c, 0x4c, 0x3e, 0x00, 0x00,
};

// Fog+0x6FF76690: identity except 'A'..'Z' -> 'a'..'z' (the 0x40 '@' entry is itself).
uint8_t fog_fold_byte(uint8_t c) {
    if (c >= 0x80) return kFoldPrefix[0x80 + static_cast<int8_t>(c)];
    if (c >= 'A' && c <= 'Z') return static_cast<uint8_t>(c + 0x20);
    return c;
}

// The 2-byte-unrolled compare in retail: strcmp order on unsigned bytes, sign only.
int32_t fog_key_compare(const uint8_t* a, const uint8_t* b) {
    while (*a != 0 && *a == *b) { ++a; ++b; }
    return (*a > *b) - (*a < *b);
}

}  // namespace

// 1.13c Fog+0x11FE0 (ordinal 10255)
// String from a row index: reject a NULL linker, a negative index and an index >= nSize by returning
// the "???" sentinel (the +0x11DF0 miss value). Otherwise look the index up in the +0x0C string tree
// and return its szKey pointer (the node itself). A miss returns the sentinel unchanged.
extern "C" EMSCRIPTEN_KEEPALIVE char* fog_sub_11FE0(void* linker, int32_t index, int32_t log_missing) {  // 1.13c Fog+0x11FE0
    char* const miss = static_cast<char*>(fog_sub_11DF0(nullptr, 0));
    if (linker == nullptr) return miss;
    if (index < 0 || index >= *static_cast<int32_t*>(linker)) return miss;
    void* const root = *reinterpret_cast<void**>(static_cast<uint8_t*>(linker) + 0xC);
    char* const found = static_cast<char*>(fog_sub_11DF0(root, index));
    if ((found == nullptr || found == miss) && log_missing != 0) return miss;
    return found;
}

// 1.13c Fog+0x12020
// Insert a 0x2C node into the string BST reachable through the link field in EAX (our first
// argument): walk left (+0x24) when the new key sorts lower, right (+0x28) when higher, write it
// into the first empty slot. An exact duplicate frees the new node (Excel.cpp:0x4A7) and keeps the
// existing one.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_12020(uint8_t** slot, uint8_t* node) {  // 1.13c Fog+0x12020
    for (;;) {
        uint8_t* const current = *slot;
        if (current == nullptr) {
            *slot = node;
            return;
        }
        const int32_t order = fog_key_compare(node, current);
        if (order < 0) {
            slot = reinterpret_cast<uint8_t**>(current + 0x24);
        } else if (order > 0) {
            slot = reinterpret_cast<uint8_t**>(current + 0x28);
        } else {
            fog_free_block(node, kExcelFile, 0x4A7);
            return;
        }
    }
}

// 1.13c Fog+0x120A0
// Free a string-tree subtree post-order: the +0x24 child, the +0x28 child, then the node itself
// (Excel.cpp:0x346). Callers test the child pointer before recursing.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_120A0(void* node) {  // 1.13c Fog+0x120A0
    uint8_t* const base = static_cast<uint8_t*>(node);
    void* const left = *reinterpret_cast<void**>(base + 0x24);
    if (left != nullptr) fog_sub_120A0(left);
    void* const right = *reinterpret_cast<void**>(base + 0x28);
    if (right != nullptr) fog_sub_120A0(right);
    fog_free_block(node, kExcelFile, 0x346);
}

// 1.13c Fog+0x120E0 (ordinal 10209)
// Free a parsed-bin-file handle: the data buffer at +0x00 (:0x9A), then the handle itself (:0x9B).
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_120E0(void* bin_file) {  // 1.13c Fog+0x120E0
    void* const data = *reinterpret_cast<void**>(bin_file);
    fog_free_block(data, kExcelFile, 0x9A);
    fog_free_block(bin_file, kExcelFile, 0x9B);
}

// 1.13c Fog+0x12120 (ordinal 10212)
// Free a linker: the string tree (root at +0x0C) via +0x120A0 on each child then the root
// (Excel.cpp:0x346), the code table at +0x08 (:0x35D), and the 0x10-byte linker itself (:0x360).
// A NULL linker returns immediately.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_12120(void* linker) {  // 1.13c Fog+0x12120
    if (linker == nullptr) return;
    uint8_t* const base = static_cast<uint8_t*>(linker);
    void* const root = *reinterpret_cast<void**>(base + 0xC);
    if (root != nullptr) {
        uint8_t* const root_base = static_cast<uint8_t*>(root);
        void* const left = *reinterpret_cast<void**>(root_base + 0x24);
        if (left != nullptr) fog_sub_120A0(left);
        void* const right = *reinterpret_cast<void**>(root_base + 0x28);
        if (right != nullptr) fog_sub_120A0(right);
        fog_free_block(root, kExcelFile, 0x346);
    }
    void* const table = *reinterpret_cast<void**>(base + 0x8);
    if (table != nullptr) fog_free_block(table, kExcelFile, 0x35D);
    fog_free_block(linker, kExcelFile, 0x360);
}

// 1.13c Fog+0x121A0 (ordinal 10217)
// Row index from a text key: copy at most 0x1F bytes of the key into a 0x20-byte stack buffer,
// case-fold it through the +0x6FF76690 table (signed byte index), then strcmp-walk the string tree
// at +0x0C. Returns nIndex (+0x20) on a match, -1 on a miss. An empty linker returns -1; a linker
// whose nSize is non-zero but whose tree root is NULL is unrecoverable (:0x475). The log argument is
// not read in 1.13c.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_sub_121A0(void* linker, const char* text, int32_t log_missing) {  // 1.13c Fog+0x121A0
    (void)log_missing;
    if (linker == nullptr) return -1;
    char key[0x20];
    fog_str_copy(key, text, 0x20);
    for (char* p = key; *p != '\0' && p < key + 0x20; ++p) {
        *p = static_cast<char>(fog_fold_byte(static_cast<uint8_t>(*p)));
    }

    void* node = *reinterpret_cast<void**>(static_cast<uint8_t*>(linker) + 0xC);
    if (*static_cast<int32_t*>(linker) == 0) {
        if (node == nullptr) return -1;
    } else if (node == nullptr) {
        const uint32_t ip = fog_capture_ip();
        fog_stack_dump();
        fog_error_report(6, "", 0x475, "Unrecoverable internal error %08x", ip);
        fog_abort();
        fog_exit_process(-1);
    }

    while (node != nullptr) {
        uint8_t* const base = static_cast<uint8_t*>(node);
        const int32_t order = fog_key_compare(reinterpret_cast<const uint8_t*>(key), base);
        if (order == 0) return *reinterpret_cast<int32_t*>(base + 0x20);
        if (order < 0) {
            node = *reinterpret_cast<void**>(base + 0x24);
        } else {
            node = *reinterpret_cast<void**>(base + 0x28);
        }
    }
    return -1;
}

// 1.13c Fog+0x12720 (ordinal 10211)
// Allocate a zeroed 0x10-byte linker with the caller's (file, line). An allocation failure reports
// level 2 at that line with an empty message (Excel.cpp:0x4A5); retail then still writes the zero
// fields, so a NULL allocation faults.
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_sub_12720(const char* file, uint32_t line) {  // 1.13c Fog+0x12720
    void* const linker = fog_alloc_block(0x10, 0, file, line);
    if (linker == nullptr) fog_error_report(2, file, line, "");
    std::memset(linker, 0, 0x10);
    return linker;
}

// 1.13c Fog+0x134E0 (ordinal 10248)
// SHA-1 state init: two zeroed length/count dwords, then the five SHA-1 chaining words
// +0x08..+0x18. Writes exactly 0x1C bytes.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_134E0(void* state) {  // 1.13c Fog+0x134E0
    uint32_t* const words = static_cast<uint32_t*>(state);
    words[0] = 0;
    words[1] = 0;
    words[2] = 0x67452301;
    words[3] = 0xefcdab89;
    words[4] = 0x98badcfe;
    words[5] = 0x10325476;
    words[6] = 0xc3d2e1f0;
}

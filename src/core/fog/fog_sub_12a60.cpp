// src/core/fog/fog_sub_12a60.cpp -- 1.13c Fog+0x12A60 (ordinal 10207), the .txt table compiler
// (Source/Fog/Src/Excel/Excel.cpp), from the cached Ghidra decompile and the 1.13c disassembly.
// Also holds its two private helpers, Fog+0x12290 (column mapper) and Fog+0x12510 (find-or-add a
// name in a string linker), kept file-local: retail reaches them by register-argument calls.
//
// File (D2BinFileStrc): header names +0 (NUL-separated), cell text +4 (NUL per cell, '\n' ends a
// row), rows +8, columns +0xC. Field entry (0x14 bytes): name, type, size, offset, link slot.
// Link slot points at the pointer to the table's linker (code table or string tree).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <emscripten.h>

struct ExcelIdx { int count; int cap; uint32_t (*ents)[2]; int flag; };       // as fog_batch_b69.cpp
int Fog_12930(ExcelIdx* t, uint32_t key);                                      // Fog+0x12930
void Fog_12610(int* t, const void* name);                                      // Fog+0x12610
extern "C" void fog_sub_12020(uint8_t** slot, uint8_t* node);                  // Fog+0x12020
extern "C" int32_t fog_sub_121A0(void* linker, const char* text, int32_t log); // Fog+0x121A0
extern "C" void fog_sub_1F8A0(char* s, int32_t len);                           // Fog+0x1F8A0

namespace {

struct File { const char* names; const char* cells; int32_t rows; int32_t cols; };
struct Field { const char* name; int32_t type; int32_t size; int32_t offset; void* link; };
typedef uint16_t (*TxtCbText)(const char*);                                              // type 22
typedef void (*TxtCbCell)(const char*, void*, int32_t, int32_t, int32_t, int32_t);       // 23..25

[[noreturn]] void fatal(uint32_t line) {
    std::fprintf(stderr, "Fog error 6 at :%u: Unrecoverable internal error\n", line);
    std::_Exit(-1);
}

const char* skip(const char* p) { while (*p) ++p; return p + 1; }   // past this cell's NUL

// Copy up to 4 bytes of the cell, pad with spaces (a 4-char code); advance past the cell.
uint32_t read_code(const char*& p) {
    char c[4] = {' ', ' ', ' ', ' '};
    for (int i = 0; i < 4 && p[0]; ++i, ++p) c[i] = *p;
    p = skip(p);
    uint32_t v; std::memcpy(&v, c, 4);
    return v;
}

// 1.13c Fog+0x12510: index of `name` in the string linker, adding a node when absent.
int find_or_add(int* linker, char* name) {
    if (!linker) fatal(0x4cc);
    if (linker[2]) fatal(0x4d0);
    int idx = fog_sub_121A0(linker, name, 0);
    if (idx >= 0) return idx;
    uint8_t* rec = static_cast<uint8_t*>(std::calloc(1, 0x2c));
    if (!rec) fatal(0x4d9);
    std::strncpy(reinterpret_cast<char*>(rec), name, 0x1f);
    fog_sub_1F8A0(reinterpret_cast<char*>(rec), 0x20);
    *reinterpret_cast<int*>(rec + 0x20) = *linker;
    fog_sub_12020(reinterpret_cast<uint8_t**>(reinterpret_cast<uint8_t*>(linker) + 0xc), rec);
    return (*linker)++;
}

// Case-insensitive hash/compare (Storm #502 / #509 stand-ins; the bucket layout is unobservable).
uint32_t shash(const char* s) { uint32_t h = 0; for (; *s; ++s) h = h * 33 + (uint8_t)((*s >= 'a' && *s <= 'z') ? *s - 32 : *s); return h ? h : 1; }
bool seq(const char* a, const char* b) {
    for (;; ++a, ++b) {
        char x = (*a >= 'a' && *a <= 'z') ? *a - 32 : *a, y = (*b >= 'a' && *b <= 'z') ? *b - 32 : *b;
        if (x != y) return false;
        if (!x) return true;
    }
}

// Fog+0x12290: out[col] = field index for each header column (0x7FFF if unknown), then the fields
// the file lacks, in table order. Returns the total.
int map_columns(uint16_t* out, const File* f, const Field* fl) {  // 1.13c Fog+0x12290
    for (const Field* p = fl; p->type; ) {   // type-5 runs must be contiguous
        if (p->type == 5 && p->size > 1) {
            uint32_t n = 1, bound;
            do { if (p[1].type != 5) fatal(0xc4); bound = (uint32_t)p[1].size; ++n; ++p; } while (n < bound);
        }
        ++p;
    }
    struct Node { Node* next; uint32_t hash; const Field* fld; } nodes[0x118], *bucket[256] = {};
    int n = 0;
    for (const Field* p = fl; p->type; ++p, ++n) {
        if (n > 0x117) fatal(0xcc);
        uint32_t h = shash(p->name);
        Node*& b = bucket[(h ^ h >> 8 ^ h >> 16 ^ h >> 24) & 0xff];
        nodes[n] = {b, h, p}; b = &nodes[n];
    }
    int k = 0;
    const char* nm = f->names;
    for (int c = 0; c < f->cols; ++c, nm = skip(nm)) {
        uint32_t h = shash(nm);
        out[k] = 0x7fff;
        for (Node* q = bucket[(h ^ h >> 8 ^ h >> 16 ^ h >> 24) & 0xff]; q; q = q->next)
            if (q->hash == h && seq(nm, q->fld->name)) { q->hash = 0; out[k] = (uint16_t)(q->fld - fl); break; }
        ++k;
    }
    for (int i = 0; i < n; ++i) if (nodes[i].hash) out[k++] = (uint16_t)(nodes[i].fld - fl);
    return k;
}

}  // namespace

// 1.13c Fog+0x12A60 (ordinal 10207)
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_12A60(File* f, Field* fl, uint8_t* out, int32_t count, int32_t stride) {  // 1.13c Fog+0x12A60
    if (!count) fatal(0x112);
    if (!stride) fatal(0x113);
    // Text layout in one frame as retail: cell text for type 23-25, for 22, then the column map.
    struct { char t7[256]; char t6[256]; uint16_t map[280]; } fr;
    uint16_t* const map = fr.map;
    const int mapped = map_columns(map, f, fl);
    const int cols = f->cols;

    bool pre = false;   // any file column that needs the linker pass
    for (int i = 0; i < cols; ++i) {
        if (map[i] == 0x7fff) continue;
        switch (fl[map[i]].type) { case 10: case 12: case 14: case 16: case 17: case 18: pre = true; }
    }
    if (pre) {
        const char* p = f->cells;
        uint8_t* row = out;
        for (int r = 0; r < f->rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (map[c] == 0x7fff) { p = skip(p); continue; }
                const Field& e = fl[map[c]];
                uint8_t* dst = row + e.offset;
                switch (e.type) {
                case 10: case 12: case 14: {
                    uint32_t code = read_code(p);
                    int idx = Fog_12930(*static_cast<ExcelIdx**>(e.link), code);
                    if (e.type == 10) *reinterpret_cast<uint32_t*>(dst) = code;
                    else if (e.type == 12) { if (idx > 0xff) fatal(0x156); *dst = (uint8_t)code; }
                    else { if (idx > 0xffff) fatal(0x15a); *reinterpret_cast<uint16_t*>(dst) = (uint16_t)code; }
                    break; }
                case 16: {
                    uint8_t* d = dst;
                    for (; *p && d < dst + e.size - 1; ++d, ++p) *d = (uint8_t)*p;
                    *d = 0;
                    p = skip(p);
                    Fog_12610(*static_cast<int**>(e.link), dst);
                    break; }
                case 17: case 18: {
                    char name[32]; char* d = name;
                    for (; *p && d < name + 0x1f; ++d, ++p) *d = *p;
                    *d = 0;
                    p = skip(p);
                    int idx = find_or_add(*static_cast<int**>(e.link), name);
                    if (e.type == 17) *reinterpret_cast<uint16_t*>(dst) = (uint16_t)idx;
                    else *reinterpret_cast<int32_t*>(dst) = idx;
                    break; }
                default: p = skip(p);
                }
            }
            if (*p != '\n') fatal(0x185);
            row += stride; ++p;
        }
    }

    const char* p = f->cells;
    uint8_t* row = out;
    for (int r = 0; r < f->rows; ++r) {
        int c = 0;
        for (; c < cols; ++c) {
            if (map[c] == 0x7fff) { p = skip(p); continue; }
            const Field& e = fl[map[c]];
            uint8_t* dst = row + e.offset;
            switch (e.type) {
            case 1: case 7: {   // string; at/over size the NUL lands at dst+size (as retail)
                uint8_t* lim = dst + e.size;
                while (*p && dst < lim) *dst++ = (uint8_t)*p++;
                if (*p) p = skip(p); else ++p;
                *dst = 0;
                break; }
            case 2: case 3: case 4: case 5: case 6: case 8: case 26: {
                const char* s = p;
                if (*s == '-') ++s;
                int32_t v = 0;
                while (*s) v = *s++ - 0x30 + v * 10;
                if (*p == '-') v = -v;
                const char* end = s + 1;
                switch (e.type) {
                case 2: case 8: *reinterpret_cast<int32_t*>(dst) = v; break;
                case 3: *reinterpret_cast<int16_t*>(dst) = (int16_t)v; break;
                case 4: case 6: *dst = (uint8_t)v; break;
                case 5: if (p + 1 != end) *dst = (uint8_t)v; break;
                case 26: {  // ponytail: gdwBitMasks[i] taken as 1<<i
                    uint8_t* b = dst + (e.size >> 3);
                    if (v == 0) *b &= (uint8_t)~(1u << (e.size & 7)); else *b |= (uint8_t)(1u << (e.size & 7));
                    break; }
                }
                p = end;
                break; }
            case 9: { uint32_t v = read_code(p); std::memcpy(dst, &v, 4); break; }
            case 10: case 12: case 14: case 16: case 17: case 18: p = skip(p); break;
            case 11: case 13: case 15: {
                uint32_t code = read_code(p);
                const ExcelIdx* t = *static_cast<ExcelIdx**>(e.link);
                int32_t v = -1, lo = 0, hi = t->count;
                while (lo < hi) {
                    int mid = (hi - lo) / 2 + lo;
                    uint32_t k = t->ents[mid][0];
                    if (code > k) lo = mid + 1;
                    else if (code < k) hi = mid;
                    else { v = (int32_t)t->ents[mid][1]; break; }
                }
                if (e.type == 11) *reinterpret_cast<int32_t*>(dst) = v;
                else if (e.type == 13) *dst = (uint8_t)v;
                else *reinterpret_cast<int16_t*>(dst) = (int16_t)v;
                break; }
            case 19: case 20: case 21: {
                char name[32]; char* d = name;
                for (; *p && d < name + 0x1f; ++d, ++p) *d = *p;
                *d = 0;
                p = skip(p);
                int32_t v = fog_sub_121A0(*static_cast<void**>(e.link), name, 0);
                if (e.type == 19) *reinterpret_cast<int32_t*>(dst) = v;
                else if (e.type == 20) *reinterpret_cast<int16_t*>(dst) = (int16_t)v;
                else *dst = (uint8_t)v;
                break; }
            case 22: {
                char* d = fr.t6;
                for (; *p && d < fr.t6 + 256; ++d, ++p) *d = *p;
                if (*p) p = skip(p); else ++p;
                *d = 0;
                if (e.link) *reinterpret_cast<uint16_t*>(dst) = reinterpret_cast<TxtCbText>(e.link)(fr.t6);
                break; }
            case 23: case 24: case 25: {
                char* d = fr.t7;
                for (; *p && d < fr.t7 + 256; ++d, ++p) *d = *p;
                if (*p) p = skip(p); else ++p;
                *d = 0;
                if (e.link) reinterpret_cast<TxtCbCell>(e.link)(fr.t7, row, e.offset, e.size, r, c);
                break; }
            default: p = skip(p);
            }
        }
        for (; c < mapped; ++c) {   // fields the file does not carry: type default
            const Field& e = fl[map[c]];
            uint8_t* dst = row + e.offset;
            switch (e.type) {
            case 1: case 4: case 5: case 6: case 7: case 12: case 16: *dst = 0; break;
            case 2: case 8: case 9: case 10: case 18: *reinterpret_cast<int32_t*>(dst) = 0; break;
            case 3: case 14: case 17: case 22: *reinterpret_cast<int16_t*>(dst) = 0; break;
            case 11: case 19: *reinterpret_cast<int32_t*>(dst) = -1; break;
            case 13: case 21: *dst = 0xff; break;
            case 15: case 20: *reinterpret_cast<int16_t*>(dst) = -1; break;
            case 23: case 24: case 25:
                if (e.link) reinterpret_cast<TxtCbCell>(e.link)(nullptr, row, e.offset, e.size, r, c);
                break;
            }
        }
        if (*p != '\n') fatal(0x286);
        row += stride; ++p;
    }
}

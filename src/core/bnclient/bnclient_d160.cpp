// BNClient 0xD160 (EAX = source name; stack hMpq, dest name buffer; ret 8): extracts an MPQ file to disk. strcpy(dest, src);
// the first '.' (CRT strchr 0x21940) is cut; the 5 bytes at RVA 0x1AFC4 (".dll\0") are appended. SFileOpenFileEx(hMpq,
// dest, 0, &hf) (Storm 268); 0 returns 0. size = SFileGetFileSize(hf, 0) (265); buf = SMemAlloc(size, BnDownload.cpp, 0xE7,
// 0) (401); NULL skips to the close. SFileReadFile(hf, buf, size, 0, 0) (269); the disk file dest is created (GENERIC_WRITE,
// CREATE_ALWAYS) and, if it opens, written whole and closed (the result is then 1); SMemFree(buf, BnDownload.cpp, 0x101, 0)
// (403). SFileCloseFile(hf) (253) when hf is nonzero. Returns the result.
#include "core/bnclient/bnclient_file_seams.h"
#include <cstring>
#include <emscripten.h>

struct BnD160Seams {
    const BnFileSeams* f;
    uint32_t (*sopen)(uint32_t mpq, const char* name, uint32_t flags, uint32_t* hf);                   // 0x80C6 Storm 268
    uint32_t (*ssize)(uint32_t hf, uint32_t* hi);                                                      // 0x80AE Storm 265
    uint32_t (*sread)(uint32_t hf, uint8_t* buf, uint32_t n, uint32_t* read, uint32_t ov);             // 0x80B4 Storm 269
    uint32_t (*sclose)(uint32_t hf);                                                                   // 0x80BA Storm 253
    uint8_t* (*alloc)(uint32_t n, uint32_t file, uint32_t line, uint32_t flags);                      // 0x8018 Storm 401
    uint32_t (*free_)(uint8_t* p, uint32_t file, uint32_t line, uint32_t flags);                      // 0x801E Storm 403
    const uint8_t* suffix;    // RVA 0x1AFC4, 5 bytes
    uint32_t file;            // RVA 0x1AFCC, the source-file string handed to the allocator
};

// 1.13c BNClient+0x0D160
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t bnclient_extract_file_d160(const char* src, uint32_t mpq, char* dest, const BnD160Seams* s) {
    uint32_t result = 0, hf = 0, written = 0;
    std::strcpy(dest, src);
    if (std::strchr(dest, '.')) *std::strchr(dest, '.') = 0;
    std::memcpy(dest + std::strlen(dest), s->suffix, 5);
    if (s->sopen(mpq, dest, 0, &hf) != 0) {
        const uint32_t size = s->ssize(hf, nullptr);
        uint8_t* buf = s->alloc(size, s->file, 0xE7, 0);
        if (buf) {
            s->sread(hf, buf, size, nullptr, 0);
            const uint32_t h = s->f->open(dest, 0x40000000u, 0, 0, 2, 0x80, 0);
            if (h != 0xFFFFFFFFu) {
                s->f->write(h, buf, size, &written, 0);
                s->f->close(h);
                result = 1;
            }
            s->free_(buf, s->file, 0x101, 0);
        }
    }
    if (hf) s->sclose(hf);
    return result;
}

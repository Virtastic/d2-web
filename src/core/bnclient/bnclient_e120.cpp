// BNClient 0xE120 (EBX = FILETIME*; stack kind, flags, a, b, name, text; ret 0x18): start a download of `name` unless the
// cached copy is current. Empty or NULL name: nothing. NULL text becomes "". 0x14050 looks the name up in the local cache
// (ECX = name, EDX = &cached FILETIME; stack &data, &size); on a hit whose FILETIME equals *ft (CompareFileTime 0): flag
// bit 1 rewrites the file (0xD280: ECX = name, EDI = ft; stack data, size), then 0xE2E0 delivers it (ECX = kind, EDX =
// flags; stack a, b, ft, name, text, data, size, 1) and a non-NULL data is freed (BnClient.cpp-family file, line 0x15A);
// done. A hit with another FILETIME frees data and falls through. Otherwise SMemAlloc(0x230, BnDownload.cpp, 0xA4, 0)
// (NULL: done) is zeroed and filled as the 0xDC50 thread request:
//   +0 u16 strlen(name)+0x21, +2 u8 0, +3 u8 1, +4 '68XI', +8 'VD2D', +C a, +10 b, +14 0, +18 ft[0], +1C ft[1],
//   +20 name (SStrCopy 0x104), +124 text (SStrCopy 0x104), +228 kind, +22C flags
// then InterlockedIncrement(thread count DAT+0x3F66C), _beginthreadex(0, 0, 0xDC50, req, 0, &tid), CloseHandle on success.
#include <cstdint>
#include <cstring>

struct BnE120Seams {
    uint32_t (*lookup)(const char* name, uint32_t* ft_out, uint32_t* data, uint32_t* size);      // 0x14050 (ret 8)
    int32_t (*cmp)(const uint32_t* a, const uint32_t* b);                                     // kernel32 CompareFileTime
    void (*write_ft)(const char* name, const uint32_t* ft, uint32_t data, uint32_t size);      // 0xD280
    void (*deliver)(uint32_t kind, uint32_t flags, uint32_t a, uint32_t b, const uint32_t* ft, const char* name,
                    const char* text, uint32_t data, uint32_t size, uint32_t one);           // 0xE2E0
    void (*free_)(uint32_t p, uint32_t file, uint32_t line, uint32_t flags);                  // 0x801E Storm #403
    uint8_t* (*alloc)(uint32_t size, uint32_t file, uint32_t line, uint32_t flags);           // 0x8018 Storm #401
    void (*scopy)(char* dst, const char* src, uint32_t max);                                  // 0x804E Storm #501
    void (*inc)(uint32_t* counter);                                                           // kernel32 InterlockedIncrement
    uint32_t (*begin)(uint32_t sec, uint32_t stack, uint32_t start, uint8_t* arg, uint32_t fl, uint32_t* tid); // 0x1F1F
    void (*close)(uint32_t h);                                                                // kernel32 CloseHandle
    const char* empty;      // "" (DAT+0x3A813)
    uint32_t free_file;     // DAT+0x3AB4C
    uint32_t dl_file;       // "..\Source\D2BNClient\BnDownload.cpp" (DAT+0x3AFCC)
    uint32_t* counter;      // DAT+0x3F66C
    uint32_t thread_start;  // 0xDC50
};

static void put32(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }

// 1.13c BNClient+0x0E120
extern "C" void bnclient_start_download_e120(const uint32_t* ft, uint32_t kind, uint32_t flags, uint32_t a, uint32_t b,
                                             const char* name, const char* text, const BnE120Seams* s) {
    if (name == nullptr || *name == 0) return;
    if (text == nullptr) text = s->empty;
    uint32_t cached[2], data, size, tid;
    if (s->lookup(name, cached, &data, &size) != 0) {
        if (s->cmp(ft, cached) == 0) {
            if (flags & 2) s->write_ft(name, ft, data, size);
            s->deliver(kind, flags, a, b, ft, name, text, data, size, 1);
            if (data != 0) s->free_(data, s->free_file, 0x15A, 0);
            return;
        }
        if (data != 0) s->free_(data, s->free_file, 0x15A, 0);
    }
    uint8_t* req = s->alloc(0x230, s->dl_file, 0xA4, 0);
    if (req == nullptr) return;
    memset(req, 0, 0x230);
    const uint16_t n = (uint16_t)(strlen(name) + 0x21);
    memcpy(req, &n, 2);
    req[2] = 0;
    req[3] = 1;
    put32(req + 4, 0x49583836u);
    put32(req + 8, 0x44324456u);
    put32(req + 0xC, a);
    put32(req + 0x10, b);
    put32(req + 0x14, 0);
    put32(req + 0x18, ft[0]);
    put32(req + 0x1C, ft[1]);
    put32(req + 0x228, kind);
    put32(req + 0x22C, flags);
    s->scopy((char*)req + 0x20, name, 0x104);
    s->scopy((char*)req + 0x124, text, 0x104);
    s->inc(s->counter);
    const uint32_t h = s->begin(0, 0, s->thread_start, req, 0, &tid);
    if (h != 0) s->close(h);
}

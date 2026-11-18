// BNClient 0xE2E0 (thiscall: ECX = kind, EDX = flags (unread); stack a, b, ft, name, text, data, size, one; ret 0x20): the
// download-complete dispatcher. kind - 0x80000001 selects (jump table at 0xE71C; anything past 5 returns):
//   1 0x80000001: strcpy(DAT_6FF3F510, name).
//   2 0x80000002: under the critical section DAT_6FF3F8E8 walk the NUL-separated string list DAT_6FF3F670 (one string per
//     step) for an _stricmp match of name; a match whose next string is non-empty starts that download (0xE120, ft = 0, kind
//     0x80000002, flags 2, a = b = 0, text NULL); otherwise 0xD2F0(0x65). Leave the section.
//   3 0x80000003: the received ad. A 6-dword block {a, b, name copy, text copy, data copy, size}; each copy is Fog 10042
//     (thunk 0x80EA: ECX = bytes, EDX = BnDownload.cpp; stack line 0x180 / 0x189 / 0x192, 0) or 0 when the source is NULL or
//     empty (data: NULL or size 0). 0xC340(); sprintf(buf, "<Received Ad: %s, %s>\n", name copy, text copy); 0xFE00(1, buf);
//     0xFEC0(EAX = 0x18; stack 3, &block).
//   4 0x80000004: the gateway list: n = SStrLen(data); p = SMemAlloc(n+1, BnDownload.cpp, 0x246, 0); SStrCopy(p, data, n+1);
//     0x15260(DAT_6FF3E9F0, p); SMemFree(p, .., 0x24C, 0); 0x15030(DAT_6FF3E9F0).
//   5 0x80000005: one != 0: DeleteFileA(name), return; else as 6.
//   6 0x80000006: the ExtraWork archive. SFileOpenArchive(name, 0, 0, &h) == 0 returns. SFileAuthenticateArchive(h, &r);
//     r in 1..4 skips to the close. Else 0xD160(EAX = name; h, buf[0x104]) extracts the DLL; LoadLibraryA(buf);
//     GetProcAddress(mod, "ExtraWork"); SFileOpenFile("data\local\use", &hf); SFileReadFile(hf, &byte, 1, &got, 0) with
//     got == 1; SFileCloseFile(hf); a zeroed 0x1004-byte block {u16 1, u16 4, u32 byte} goes to ExtraWork in ECX; a
//     non-zero answer sends frame 0x4B (0xEB00: ECX = u16[2] + 4, EDI = the block). FreeLibrary. Then the close: a non-zero
//     h is SFileCloseArchive'd, Sleep(50), DeleteFileA(name), and DeleteFileA(buf) when buf is non-empty.
#include <cstdint>
#include <cstdio>
#include <cstring>

struct BnE2e0Seams {
    uint8_t* (*falloc)(uint32_t n, uint32_t file, uint32_t line, uint32_t fl);              // 0x80EA Fog 10042
    void (*c340)();                                                                         // 0xC340
    void (*format)(char* buf, uint32_t fmt, const char* a, const char* b);                 // 0x159A sprintf
    void (*text)(uint32_t kind, const char* buf);                                           // 0xFE00
    void (*bin)(uint32_t len, uint32_t id, const uint32_t* blk);                            // 0xFEC0 (EAX = len)
    void (*enter)(uint32_t cs);                                                             // EnterCriticalSection
    void (*leave)(uint32_t cs);                                                             // LeaveCriticalSection
    int32_t (*icmp)(const char* a, const char* b);                                         // 0x18C6 _stricmp
    void (*status)(uint32_t code);                                                          // 0xD2F0
    void (*start)(const uint32_t* ft, uint32_t kind, uint32_t flags, uint32_t a, uint32_t b, const char* name,
                  const char* text);                                                        // 0xE120 (EBX = ft)
    uint32_t (*slen)(const char* s);                                                        // 0x8042 Storm 506
    uint8_t* (*alloc)(uint32_t n, uint32_t file, uint32_t line, uint32_t fl);               // 0x8018 Storm 401
    void (*scopy)(uint8_t* d, const char* s, uint32_t n);                                   // 0x804E Storm 501
    void (*gw_load)(uint32_t gw, const uint8_t* ini);                                       // 0x15260
    void (*free_)(uint8_t* p, uint32_t file, uint32_t line, uint32_t fl);                   // 0x801E Storm 403
    void (*gw_save)(uint32_t gw);                                                           // 0x15030
    void (*del)(const char* path);                                                          // DeleteFileA
    uint32_t (*aopen)(const char* name, uint32_t a, uint32_t b, uint32_t* h);               // 0x8096 Storm 266
    uint32_t (*aauth)(uint32_t h, uint32_t* r);                                             // 0x80A8 Storm 251
    uint32_t (*extract)(const char* name, uint32_t h, char* buf);                           // 0xD160 (EAX = name)
    uint32_t (*loadlib)(const char* path);                                                  // LoadLibraryA
    uint32_t (*getproc)(uint32_t mod, uint32_t name);                                       // GetProcAddress
    uint32_t (*fopen)(uint32_t name, uint32_t* hf);                                         // 0x80A2 Storm 267
    uint32_t (*fread)(uint32_t hf, uint8_t* b, uint32_t n, uint32_t* got, uint32_t ov);     // 0x80B4 Storm 269
    void (*fclose)(uint32_t hf);                                                            // 0x80BA Storm 253
    uint32_t (*extra)(uint32_t proc, uint8_t* blk);                                         // ExtraWork (ECX = blk)
    void (*send)(uint32_t len, uint32_t id, const uint8_t* p);                              // 0xEB00 (ECX len, EDI p)
    void (*freelib)(uint32_t mod);                                                          // FreeLibrary
    void (*aclose)(uint32_t h);                                                             // 0x80C0 Storm 252
    void (*sleep)(uint32_t ms);                                                             // Sleep
    char* motd;            // DAT_6FF3F510
    const char* list;      // DAT_6FF3F670
    uint32_t cs;           // DAT_6FF3F8E8
    uint32_t file;         // "..\Source\D2BNClient\BnDownload.cpp" (DAT_6FF3AFCC)
    uint32_t fmt;          // "<Received Ad: %s, %s>\n" (DAT_6FF3AFAC)
    uint32_t gw;           // DAT_6FF3E9F0
    uint32_t extra_name;   // "ExtraWork" (DAT_6FF3AFA0)
    uint32_t use_name;     // "data\local\use" (DAT_6FF3AF90)
};

static char* copy_str(const char* src, uint32_t line, const BnE2e0Seams* s) {
    if (src == nullptr || *src == 0) return nullptr;
    char* d = (char*)s->falloc((uint32_t)strlen(src) + 1, s->file, line, 0);
    strcpy(d, src);
    return d;
}

static void extra_work(const char* name, const BnE2e0Seams* s) {
    char buf[0x104];
    memset(buf, 0, sizeof buf);
    uint32_t h = 0, hf = 0, r, got;
    uint8_t byte;
    if (s->aopen(name, 0, 0, &h) == 0) return;
    s->aauth(h, &r);
    if (!(r != 0 && r < 5) && s->extract(name, h, buf) != 0) {
        const uint32_t mod = s->loadlib(buf);
        if (mod != 0) {
            const uint32_t proc = s->getproc(mod, s->extra_name);
            if (proc != 0 && s->fopen(s->use_name, &hf) != 0 && s->fread(hf, &byte, 1, &got, 0) != 0 && got == 1) {
                s->fclose(hf);
                uint8_t blk[0x1004];
                memset(blk, 0, sizeof blk);
                const uint16_t one = 1, four = 4;
                const uint32_t b = byte;
                memcpy(blk, &one, 2);
                memcpy(blk + 2, &four, 2);
                memcpy(blk + 4, &b, 4);
                if (s->extra(proc, blk) != 0) {
                    uint16_t len;
                    memcpy(&len, blk + 2, 2);
                    s->send((uint32_t)len + 4, 0x4B, blk);  // EDI = the block itself (lea after the push)
                }
            }
            s->freelib(mod);
        }
    }
    if (h != 0) s->aclose(h);
    s->sleep(0x32);
    s->del(name);
    if (buf[0] != 0) s->del(buf);
}

// 1.13c BNClient+0x0E2E0
extern "C" void bnclient_download_done_e2e0(uint32_t kind, uint32_t flags, uint32_t a, uint32_t b, const uint32_t* ft,
                                            const char* name, const char* text, const uint8_t* data, uint32_t size,
                                            uint32_t one, const BnE2e0Seams* s) {
    (void)flags; (void)ft;
    switch (kind) {
    case 0x80000001u:
        strcpy(s->motd, name);
        return;
    case 0x80000002u: {
        s->enter(s->cs);
        const char* p = s->list;
        while (*p != 0) {
            if (s->icmp(p, name) == 0) {
                const char* next = p + strlen(p) + 1;
                if (*next != 0) {
                    const uint32_t zero[2] = { 0, 0 };
                    s->start(zero, 0x80000002u, 2, 0, 0, next, nullptr);
                    s->leave(s->cs);
                    return;
                }
                break;
            }
            p += strlen(p) + 1;
        }
        s->status(0x65);
        s->leave(s->cs);
        return;
    }
    case 0x80000003u: {
        uint32_t blk[6];
        blk[0] = a;
        blk[1] = b;
        char* nc = copy_str(name, 0x180, s);
        char* tc = copy_str(text, 0x189, s);
        uint8_t* dc = nullptr;
        if (size != 0 && data != nullptr) {
            dc = s->falloc(size, s->file, 0x192, 0);
            memcpy(dc, data, size);
        }
        blk[2] = (uint32_t)(uintptr_t)nc;
        blk[3] = (uint32_t)(uintptr_t)tc;
        blk[4] = (uint32_t)(uintptr_t)dc;
        blk[5] = size;
        s->c340();
        char buf[0x1000];
        s->format(buf, s->fmt, nc, tc);
        s->text(1, buf);
        s->bin(0x18, 3, blk);
        return;
    }
    case 0x80000004u: {
        const char* ini = (const char*)data;
        const uint32_t n = s->slen(ini) + 1;
        uint8_t* p = s->alloc(n, s->file, 0x246, 0);
        s->scopy(p, ini, n);
        s->gw_load(s->gw, p);
        s->free_(p, s->file, 0x24C, 0);
        s->gw_save(s->gw);
        return;
    }
    case 0x80000005u:
        if (one != 0) { s->del(name); return; }
        extra_work(name, s);
        return;
    case 0x80000006u:
        extra_work(name, s);
        return;
    default:
        return;
    }
}

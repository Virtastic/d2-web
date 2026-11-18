// BNClient 0xE740 (fastcall: ECX = list, EDX = len): store the patch list and start its first download.
// Fog 10001 (thunk 0x80F6) == 0 returns 0. Otherwise DAT_6FF3F508 = 0; under the section DAT_6FF3F8E8 the list is
// SMemCopy'd (Storm 491) into DAT_6FF3F670; DAT_6FF3F508 = 0 again; SRegSaveData("Patch", "Patches", 0x82, list, len + 1).
// A NULL or empty list returns 0. Then the first entry goes the way 0xE120 sends a name, with a zero FILETIME, kind
// 0x80000002 and flags 2: a current cached copy (0x14050 + CompareFileTime) is delivered through 0xD280 and 0xE2E0 (one = 1)
// and freed with 0x12EC0, returning 0; a stale one is freed and falls through. The request is SMemAlloc(0x230,
// BnDownload.cpp, 0x6A2, 0) (NULL returns 0) and the return value is _beginthreadex(0xDC50)'s handle (never closed).
#include <cstdint>
#include <cstring>

struct BnE740Seams {
    uint32_t (*ready)();                                                                      // 0x80F6 Fog 10001
    void (*enter)(uint32_t cs);                                                               // EnterCriticalSection
    void (*leave)(uint32_t cs);                                                               // LeaveCriticalSection
    void (*mcopy)(uint32_t dst, const char* src, uint32_t n);                                 // 0x807E Storm 491
    void (*reg_save)(uint32_t key, uint32_t value, uint32_t flags, const char* data, uint32_t n); // 0x8066 Storm 424
    uint32_t (*lookup)(const char* name, uint32_t* ft_out, uint32_t* data, uint32_t* size);   // 0x14050 (ret 8)
    int32_t (*cmp)(const uint32_t* a, const uint32_t* b);                                    // CompareFileTime
    void (*write_ft)(const char* name, const uint32_t* ft, uint32_t data, uint32_t size);     // 0xD280 (EDI = ft)
    void (*deliver)(uint32_t kind, uint32_t flags, uint32_t a, uint32_t b, const uint32_t* ft, const char* name,
                    const char* text, uint32_t data, uint32_t size, uint32_t one);          // 0xE2E0
    void (*release)(uint32_t data, uint32_t size);                                            // 0x12EC0 (ECX, EDX)
    uint8_t* (*alloc)(uint32_t size, uint32_t file, uint32_t line, uint32_t flags);          // 0x8018 Storm 401
    void (*scopy)(char* dst, const char* src, uint32_t max);                                 // 0x804E Storm 501
    void (*inc)(uint32_t* counter);                                                          // InterlockedIncrement
    uint32_t (*begin)(uint32_t sec, uint32_t stack, uint32_t start, uint8_t* arg, uint32_t fl, uint32_t* tid); // 0x1F1F
    uint32_t* state;        // DAT_6FF3F508
    uint32_t cs;            // DAT_6FF3F8E8
    uint32_t list;          // DAT_6FF3F670
    uint32_t key, value;    // "Patch", "Patches"
    const char* empty;      // "" (DAT_6FF3A813)
    uint32_t dl_file;       // "..\Source\D2BNClient\BnDownload.cpp" (DAT_6FF3AFCC)
    uint32_t* counter;      // DAT_6FF3F66C
    uint32_t thread_start;  // 0xDC50
};

static void put32(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }

// 1.13c BNClient+0x0E740
extern "C" uint32_t bnclient_patch_list_e740(const char* list, uint32_t len, const BnE740Seams* s) {
    if (s->ready() == 0) return 0;
    *s->state = 0;
    s->enter(s->cs);
    s->mcopy(s->list, list, len);
    *s->state = 0;
    s->leave(s->cs);
    s->reg_save(s->key, s->value, 0x82, list, len + 1);
    const uint32_t ft[2] = { 0, 0 };
    if (list == nullptr || *list == 0) return 0;
    uint32_t cached[2], data, size, tid;
    if (s->lookup(list, cached, &data, &size) != 0) {
        if (s->cmp(ft, cached) == 0) {
            s->write_ft(list, ft, data, size);
            s->deliver(0x80000002u, 2, 0, 0, ft, list, nullptr, data, size, 1);
            s->release(data, size);
            return 0;
        }
        s->release(data, size);
    }
    uint8_t* req = s->alloc(0x230, s->dl_file, 0x6A2, 0);
    if (req == nullptr) return 0;
    memset(req, 0, 0x230);
    const uint16_t n = (uint16_t)(strlen(list) + 0x21);
    memcpy(req, &n, 2);
    req[2] = 0;
    req[3] = 1;
    put32(req + 4, 0x49583836u);
    put32(req + 8, 0x44324456u);
    put32(req + 0x228, 0x80000002u);
    put32(req + 0x22C, 2);
    put32(req + 0x18, ft[0]);
    put32(req + 0x1C, ft[1]);
    s->scopy((char*)req + 0x20, list, 0x104);
    s->scopy((char*)req + 0x124, s->empty, 0x104);
    s->inc(s->counter);
    return s->begin(0, 0, s->thread_start, req, 0, &tid);
}

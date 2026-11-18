// 1.13c D2Client+0x872D0: Warden module signature check. The Storm and Fog calls are seams in the retail argument order.
#include <cstdint>
#include <emscripten.h>

struct D2c872d0Env {
    void (*fatal)(uint32_t code);
    void* (*alloc)(uint32_t n, const char* file, int32_t line, int32_t fl);   // Storm 401
    void (*free_)(void* p, const char* file, int32_t line, int32_t fl);       // Storm 403
    void (*copy)(void* dst, const void* src, uint32_t n);                     // Storm 491
    void (*fill)(void* dst, uint32_t n, uint32_t v);                          // Storm 492
    int32_t (*cmp)(const void* a, const void* b, uint32_t n);                 // Storm 495
    void (*bigNew)(void** slot);                                              // Storm 624
    void (*bigDel)(void* big);                                                // Storm 606
    void (*bigFromBinary)(void* big, const void* p, uint32_t n);              // Storm 609
    void (*bigPowMod)(void* r, void* b, void* e, void* m);                    // Storm 628
    void (*bigToBinary)(void* big, void* buf, uint32_t n, uint32_t* outLen);  // Storm 638
    void (*hashFill)(uint32_t len, uint32_t off, const uint8_t* data, const void* a0, void* out); // D2Client+0x87160
};

static const char kWardenFile[] = "..\\Source\\D2Client\\WARDEN\\WardenClient.cpp";

// ECX=minLen, EAX=total, EDI=sigLen; stack a0..a4
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_872d0(uint32_t ecx, uint32_t eax, uint32_t edi, const void* a0,
                                                         const uint8_t* a1, const void* a2, const void* a3,
                                                         uint32_t a4, const D2c872d0Env* e) {
    if (eax < ecx) { e->fatal(0xa3); return 0; }
    if (eax < edi + 4) return 0;
    uint32_t rest = eax - edi - 4;
    const uint8_t* sigp = a1 + rest;
    uint32_t magic = 0;
    e->copy(&magic, sigp, 4);
    if (magic != 0x5349474e) return 0;
    uint8_t* want = (uint8_t*)e->alloc(edi, kWardenFile, 0xab, 0);
    e->fill(want, edi, 0xbb);
    want[edi - 1] = 0xb;
    e->hashFill(rest, ecx, a1, a0, want);
    uint8_t* sig = (uint8_t*)e->alloc(edi, kWardenFile, 0xb5, 0);
    e->copy(sig, sigp + 4, edi);
    void* n0; void* n1; void* n2; void* n3;
    e->bigNew(&n0); e->bigNew(&n1); e->bigNew(&n2); e->bigNew(&n3);
    e->bigFromBinary(n1, sig, edi);
    e->bigFromBinary(n2, a3, a4);
    e->bigFromBinary(n3, a2, edi);
    e->bigPowMod(n0, n1, n2, n3);
    e->bigToBinary(n0, sig, edi, nullptr);
    uint32_t ok = e->cmp(sig, want, edi) == 0;
    e->free_(want, kWardenFile, 0xcc, 0);
    e->free_(sig, kWardenFile, 0xcd, 0);
    e->bigDel(n0); e->bigDel(n1); e->bigDel(n2); e->bigDel(n3);
    return ok;
}

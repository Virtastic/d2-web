#pragma once
#include <cstdint>
struct D2ClientBatch1de0Env {
    // 0x1E070
    void* (*spawnA7780)(uint32_t* rec, uint32_t a, uint32_t b, int32_t c);      // 0xA7780
    // 0x1E5F0 (the loading-screen draw; all imported ordinals of the retail thunk table)
    void     (*ord10227)();
    uint32_t (*ord10265)();
    void     (*ord10024)(const uint32_t* msg);
    void*    (*ord10048)(const uint32_t* path);
    void     (*fn3CC60)(uint32_t handle, uint32_t* table, uint32_t count);       // 0x3CC60
    void     (*fn1C720)();                                                       // 0x1C720
    void     (*ord10021)();
    void     (*ord10060)();
    const void* (*ord10003)(uintptr_t arg);
    void     (*ord10028)();
    void     (*ord10150)(const void* text, int32_t x, int32_t y, int32_t color, int32_t bfalse);
    void     (*drawCel)(uint32_t cel, uint32_t frame, int32_t x, int32_t y, int32_t palette, int32_t mode);
    uint8_t  (*fn1C980)(uint32_t mode);                                          // 0x1C980, mode in EAX
    uint32_t (*fn1D840)(int32_t x, int32_t y);                                   // 0x1D840
    void     (*exitProcess)(int32_t code);
    // 0x200E0 / 0x20B70
    void     (*ord10569)(void* obj, uint8_t* out, uint8_t* out2);
    uint32_t (*ord10750)(uint32_t p);
    uint32_t (*ord10867)(uint32_t p);
    uint32_t (*fnB0450)(void* obj, uint32_t value);                              // 0xB0450, obj in ESI
    void     (*ord10973)(void* obj, uint32_t a, uint32_t b);
    // 0x20180
    void*    (*fnA60D0)(void* obj);                                              // 0xA60D0
    bool     (*fn1FD00)(void* obj, uint32_t mask);                               // 0x1FD00, obj in EAX
    void*    (*fnA5F70)(void* obj);                                              // 0xA5F70, obj in ESI
    uint32_t (*fn1630)(void* obj);                                               // 0x1630
    uint32_t (*fn1660)(void* obj);                                               // 0x1660
    void     (*ord10983)(uint32_t p);
    uint32_t (*ord10764)(uint32_t p);
    void     (*ord10936)();
    void     (*ord10614)();
    uint32_t (*ord11029)(void* obj);
    uint32_t (*ord10240)(void* obj, uint32_t value);
    // 0x20340 / 0x20A60
    void     (*fnB30F0)(uint32_t* ctx);                                          // 0xB30F0
    void     (*fn88A70)(void* obj, uint32_t a, uint32_t b, uint32_t c);           // 0x88A70, key in EBX
    // 0x20620
    const void* (*ord10038)(int32_t id);
    void     (*ord10971)();
    uint32_t (*ord10852)(uint32_t a);
    const void* (*ord10249)(uint32_t a);
    uint32_t (*ord10272)(uint32_t a);
    void     (*fn90CB0)(void* dst, const void* fmt, const void* arg);            // 0x90CB0
    uint32_t (*fn1330)(uint64_t* state, uint32_t n);                             // 0x1330, n in EAX
    void     (*unicodeCtor300)(void* buf);                                       // Unicode x300, stride 4
    void     (*unicodeToUnicode)(void* dst, const char* src, int32_t n);
    // 0x20FB0
    void     (*fn53070)();
    void     (*fn52860)();
    void     (*fn52230)(void* a, void* b);
    void     (*fn52620)();
    void     (*fn527A0)();
    void     (*fn52270)();
    void     (*fn52600)();
    void     (*fn52BD0)(void* obj);
    void     (*fn52320)();
    void     (*fn526E0)();
    void     (*fn530D0)();
    void     (*fn529F0)();
    void     (*fn52570)();
    // 0x21400
    // retail callback call: ECX = key, EDX = unit, four stack words (a, b, node+0xC, node+0x10); fn is a retail code address
    uint32_t (*invoke1410)(uint32_t fn, uint32_t ecx, uint32_t edx, uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    uint8_t* (*resolve)(uint32_t retailAddress);
};

// src/core/d2win/d2win_batch_b54b.cpp -- 1.13c D2Win.dll alpha batch (no spec): 0x17A00, 0x7DC0.
// Skipped (decompile uses unaff_/in_EAX register args, contract not recoverable faithfully):
// 0x7970 0x17260 0x12280 0xDFF0 0xB250 0xEA30 0xCFC0 0xD4D0 0xE5F0 0x1B3C.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Win+0x17A00 : append node to the singly linked list headed at DAT_6f9014a4 (next at +0x3c).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2win_sub_17a00(uint8_t** head, uint8_t* node) {
    *(uint8_t**)(node + 0x3c) = nullptr;
    if (*head) {
        uint8_t* p = *head;
        while (*(uint8_t**)(p + 0x3c)) p = *(uint8_t**)(p + 0x3c);
        *(uint8_t**)(p + 0x3c) = node;
        return 1;
    }
    *head = node;
    return 1;
}

struct D2WinB54bEnv {
    void (*ord501)();                              // Storm/Fog #501
    int32_t (*ord266)(char* buf);                  // #266 (nonzero = buffer holds a usable path)
    uint32_t (*getModuleFileName)(char* buf, uint32_t n); // seam for GetModuleFileNameA(NULL,..)
    void (*ord503)(char* dst, const char* src, int32_t max); // #503 string copy
    int32_t (*sub_7b30)();                         // 0x7B30
};

// 1.13c D2Win+0x7DC0
extern "C" EMSCRIPTEN_KEEPALIVE
char* d2win_sub_7dc0(char* buf, const char* name, const D2WinB54bEnv* e) {
    e->ord501();
    if (e->ord266(buf) == 0) {
        if (e->getModuleFileName(buf, 0x104) == 0) buf[0] = 0;
        char* p = strrchr(buf, '\\');
        if (p) p[1] = 0;
        e->ord503(buf, name, 0x7fffffff);
        if (e->ord266(buf) == 0)
            return e->sub_7b30() != 0 ? buf : nullptr;
    }
    return buf;
}

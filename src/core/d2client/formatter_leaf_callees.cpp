// Real extern "C" forms of six small leaf callees of the skill-description formatter D2Client+0x316C0.
// Imported-DLL ordinals and the not-yet-exported sibling formatters are env function pointers, the
// d2client_batch_tiny_1019i.cpp pattern; the logic is the retail decompile's.
#include <cstdint>
#include <emscripten.h>
#include "text_append.h"

struct D2ClientListEnv {            // the four D2Common list ordinals the scan uses
    void*    (*first)(void* list);          // #10947
    int16_t* (*data)(void* node);           // #10858
    int32_t  (*count)(void* node);          // #10353
    void*    (*next)(void* node);           // #10273
};

// Shared body of 3CE60 / 3CEB0 / 3CF00 / 3CF50 (byte-identical in retail): EAX = list, EBX = wanted
// short. Walk to the first node with a non-null data pointer, count >= 1 and *data == wanted; the
// result is never used, the function returns 0.
static uint32_t list_scan(void* list, int32_t want, const D2ClientListEnv* e) {
    if (list) {
        void* it = e->first(list);
        while (it) {
            int16_t* d = e->data(it);
            if (d && e->count(it) >= 1 && *d == want) break;
            it = e->next(it);
        }
    }
    return 0;
}

// 1.13c D2Client+0x3CE60
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3ce60(void* eax, int32_t ebx, const D2ClientListEnv* e) { return list_scan(eax, ebx, e); }
// 1.13c D2Client+0x3CEB0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3ceb0(void* eax, int32_t ebx, const D2ClientListEnv* e) { return list_scan(eax, ebx, e); }
// 1.13c D2Client+0x3CF00
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3cf00(void* eax, int32_t ebx, const D2ClientListEnv* e) { return list_scan(eax, ebx, e); }
// 1.13c D2Client+0x3CF50
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_3cf50(void* eax, int32_t ebx, const D2ClientListEnv* e) { return list_scan(eax, ebx, e); }

struct D2ClientDmgEnv {
    int32_t  (*ord10653)(uint32_t a, uint32_t b);   // D2Common #10653
    int32_t  (*ord10121)(uint32_t unit);            // D2Common #10121
    int32_t  (*ord11091)(uint32_t unit);            // D2Common #11091
    D2ClientTextEnv text;                           // D2Lang #10003 for 0x2CF20 / 0x2D9B0
};

// 1.13c D2Client+0x2EEF0: (ECX=param_1, EDX=param_2, EBX=dst) a string id from #10653(param_2,param_1);
// nonzero -> 0x2D9B0(0x10a6, 0x10b3, id, 1).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2eef0(char16_t* dst, uint32_t p1, uint32_t p2, const D2ClientDmgEnv* e) {
    int32_t id = e->ord10653(p2, p1);
    if (id) d2_client_2d9b0(dst, 0x10a6, 0x10b3, id, 1, &e->text);
}

// 1.13c D2Client+0x2ED80: stats #10121 and #11091 of the unit, each *75>>8 (signed), emitted by
// 0x2CF20(lo, hi, 0x10a1) into the register-passed destination.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2ed80(char16_t* dst, uint32_t unit, const D2ClientDmgEnv* e) {
    int32_t a = e->ord10121(unit);
    int32_t b = e->ord11091(unit);
    d2_client_2cf20(dst, a * 0x4b >> 8, b * 0x4b >> 8, 0x10a1, &e->text);
}

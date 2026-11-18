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

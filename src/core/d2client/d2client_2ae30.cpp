// Unit action gate. Retail: EAX = unit, EBX = id, ESI = unit id word,
// stack (ret 8) = p1, p2. A unit whose +0xC4 bit 1 is set asks callee(ESI, EBX); true calls callee(EBX; ESI, p1, p2) and returns 0,
// false returns 1. No unit or bit clear: globals callee := 1, callee := 0, return 0.
#include <cstdint>
#include <emscripten.h>

struct D2c2ae30Env {
    uint32_t (*c29e80)(uint32_t esi, uint32_t ebx);                              
    void (*c2aa40)(uint32_t ebx, uint32_t esi, uint32_t p1, uint32_t p2);        
    uint32_t* g_cc2f8;                                                           
    uint32_t* g_cc2f4;                                                           
};


extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2ae30(const uint8_t* unit, uint32_t ebx, uint32_t esi,
                                                         uint32_t p1, uint32_t p2, const D2c2ae30Env* env) {
    if (unit && ((*(const uint32_t*)(unit + 0xC4) >> 1) & 1u)) {
        if (env->c29e80(esi, ebx)) {
            env->c2aa40(ebx, esi, p1, p2);
            return 0;
        }
        return 1;
    }
    *env->g_cc2f8 = 1;
    *env->g_cc2f4 = 0;
    return 0;
}

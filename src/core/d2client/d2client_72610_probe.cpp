// 1.13c D2Client+0x72610 probe: r[0]=skill (EAX), r[1]=a (ECX), r[2]=unit blob (EDX), r[3]=f, r[5]=x, r[6]=y, r[7]=log blob, r[8]=dt blob (+0xBCC table, +0xBD4 count), r[9]=mask blob (byte +4);
// hook values at lg+0x1c0: C1C0, C442, C4B4.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
namespace {
uint32_t* g_lg;
uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t hv(int i) { return g_lg[0x70 + i]; }   // hook return values the descriptor put at lg+0x1c0
const uint32_t kFile = 0xB2000000u | 0xD4CD0u;   // the PlrSkills.cpp path string in the module image
uint32_t fileTag(const char* s) { return s && !strcmp(s, "..\\Source\\D2Client\\UNIT\\PlrSkills.cpp") ? kFile : 0xBADF11Eu; }
void L(uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3) {
    uint32_t* w = g_lg + 1 + g_lg[0]++ * 10; w[0] = rva; w[1] = a; w[2] = c; w[3] = d; w[4] = b; w[5] = s; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
}
}
struct D2c72610Env {
    uint32_t (*c1c0)(uint32_t, uint32_t); uint32_t (*c442)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); void (*c2f2)(uint32_t, uint32_t);
    void (*c36a)(uint32_t, uint32_t, uint32_t); uint32_t (*c4b4)(uint32_t, uint32_t, uint32_t); void (*c562)(uint32_t, uint32_t, uint32_t, uint32_t);
    void (*f559e0)(uint32_t, uint32_t); void (*c5c8)(uint32_t, const char*, uint32_t);
};
extern "C" uint32_t d2_client_72610(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int32_t, uint32_t, uint32_t, const D2c72610Env*);
namespace {
uint32_t b1c0(uint32_t u, uint32_t a) { L(0xC1C0, 0, 0, 0, 0, 0, u, a, 0, 0); return hv(0); }
uint32_t b442(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t) { L(0xC442, 0, 0, 0, 0, 0, a, b, c, d); return hv(1); }   // the 5th word (unit+0xC) is not in the 4-word log
void b2f2(uint32_t h, uint32_t a) { L(0xC2F2, 0, 0, 0, 0, 0, h, a, 0, 0); }
void b36a(uint32_t u, uint32_t h, uint32_t one) { L(0xC36A, 0, 0, 0, 0, 0, u, h, one, 0); }
uint32_t b4b4(uint32_t h, uint32_t s, uint32_t y) { L(0xC4B4, 0, 0, 0, 0, 0, h, s, y, 0); return hv(2); }
void b562(uint32_t h, uint32_t s, uint32_t x, uint32_t y) { L(0xC562, 0, 0, 0, 0, 0, h, s, x, y); }
void b559(uint32_t x, uint32_t ebx) { L(0xA59E0, 0, 0, 0, ebx, 0, x, 0, 0, 0); }
void b5c8(uint32_t u, const char* f, uint32_t line) { L(0xC5C8, 0, 0, 0, 0, 0, u, fileTag(f), line, 0); } }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_72610(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[7]; g_lg[0] = 0;
    D2c72610Env e{b1c0, b442, b2f2, b36a, b4b4, b562, b559, b5c8};
    r[4] = d2_client_72610(r[0], r[1], r[2], r[3], r[5], r[6], (int32_t)rd(r[8] + 0xbd4), rd(r[8] + 0xbcc), *(const uint8_t*)(uintptr_t)(r[9] + 4), &e);
}

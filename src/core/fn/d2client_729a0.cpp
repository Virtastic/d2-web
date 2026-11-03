// 1.13c D2Client+0x729A0 probe: r[0]=idx (EDX), r[1]=ctx (ECX), r[2]=dt blob (+0xBC table, +0xC4 count), r[3]=log blob.
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
extern "C" void d2_client_729a0(int32_t idx, uint32_t dt_bc, int32_t dt_c4, void (*sub_727f0)(int32_t));
namespace { uint32_t g_ctx; void s(int32_t x) { L(0x727F0, g_ctx, 0, 0, 0, 0, (uint32_t)x, 0, 0, 0); } }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_729a0(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[3]; g_lg[0] = 0; g_ctx = r[1];
    d2_client_729a0((int32_t)r[0], rd(r[2] + 0xbc), (int32_t)rd(r[2] + 0xc4), s);
    r[4] = 0;
}

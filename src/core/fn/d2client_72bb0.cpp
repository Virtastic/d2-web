// 1.13c D2Client+0x72BB0 probe: r[0]=p1 (ECX), r[1]=p2 (EDX), r[2]=dt blob (+0xB98 table, +0xBA0 count), r[3]=log blob; hook values at lg+0x1c0: C1C0, C4B4, 0x225D0.
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
extern "C" void d2_client_72bb0(int32_t p1, uint32_t p2, uint32_t dt_b98, int32_t dt_ba0, uint32_t (*ord10871)(int32_t, uint32_t), int32_t (*ord10680)(uint32_t, int, int),
    uint32_t (*sub_225d0)(int32_t, int32_t), void (*sub_88c20)(int32_t, int));
namespace {
uint32_t a10871(int32_t p1, uint32_t p2) { L(0xC1C0, 0, 0, 0, 0, 0, (uint32_t)p1, p2, 0, 0); return hv(0); }
int32_t a10680(uint32_t h, int a, int b) { L(0xC4B4, 0, 0, 0, 0, 0, h, (uint32_t)a, (uint32_t)b, 0); return (int32_t)hv(1); }
uint32_t a225d0(int32_t p1, int32_t v) { L(0x225D0, (uint32_t)v, (uint32_t)p1, 0, 0, 0, 0, 0, 0, 0); return hv(2); }
void a38c20(int32_t p1, int x) { L(0x88C20, 0, 0, 0, 0, 0, (uint32_t)p1, (uint32_t)x, 0, 0); } }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_72bb0(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[3]; g_lg[0] = 0;
    d2_client_72bb0((int32_t)r[0], r[1], rd(r[2] + 0xb98), (int32_t)rd(r[2] + 0xba0), a10871, a10680, a225d0, a38c20);
    r[4] = 0;
}

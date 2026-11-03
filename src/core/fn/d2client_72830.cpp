// 1.13c D2Client+0x72830 probe: r[0]=EAX unit arg, r[1]=EBX, r[2]=dt blob (+0xB98 table, +0xBA0 count), r[3]=log blob; hook values at lg+0x1c0: C310, 724E0, C42A, C628.
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
extern "C" void d2_client_72830(uint32_t in_eax, uint32_t ebx, uint32_t dt_b98, int32_t dt_ba0,
    uint32_t (*ord10511)(uint32_t), int (*sub_724e0)(uint32_t), int32_t (*ord10601)(uint32_t, const char*, int), uint32_t (*ord10397)(uint32_t),
    void (*sub_2b960)(uint32_t, uint32_t, uint32_t), void (*sub_75e50)(uint32_t, uint32_t));
namespace {
uint32_t a10511(uint32_t e) { L(0xC310, 0, 0, 0, 0, 0, e, 0, 0, 0); return hv(0); }
int a724e0(uint32_t e) { L(0x724E0, e, 0, 0, 0, 0, 0, 0, 0, 0); return (int)hv(1); }
int32_t a10601(uint32_t u, const char* f, int line) { L(0xC42A, 0, 0, 0, 0, 0, u, fileTag(f), (uint32_t)line, 0); return (int32_t)hv(2); }
uint32_t a10397(uint32_t u) { L(0xC628, 0, 0, 0, 0, 0, u, 0, 0, 0); return hv(3); }
void a2b960(uint32_t e, uint32_t b, uint32_t st) { L(0x2B960, 0, e, b, 0, 0, st, 0, 0, 0); }
void a75e50(uint32_t b, uint32_t z) { L(0xC5E50, 0, 0, 0, 0, 0, b, z, 0, 0); } }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_72830(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[3]; g_lg[0] = 0;
    d2_client_72830(r[0], r[1], rd(r[2] + 0xb98), (int32_t)rd(r[2] + 0xba0), a10511, a724e0, a10601, a10397, a2b960, a75e50);
    r[4] = 0;
}

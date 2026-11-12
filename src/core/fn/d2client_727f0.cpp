// 1.13c D2Client+0x727F0 probe: r[0]=ctx (EAX), r[1]=id, r[2]=log blob, r[3]=n (the 0xCB80 hook value).
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
extern "C" void d2_client_727f0(uint32_t ctx, int32_t id, int32_t (*ord10799)(int32_t),
                                uint32_t (*sub_69660)(uint32_t edi, int32_t edx, uint32_t eax));
namespace { uint32_t g_n;
int32_t o(int32_t id) { L(0xCB80, 0, 0, 0, 0, 0, (uint32_t)id, 0, 0, 0); return (int32_t)g_n; }
// the descriptor's 69660 hook logs EAX and EDX and returns 0
uint32_t s(uint32_t, int32_t x, uint32_t eax) { L(0x69660, eax, 0, (uint32_t)x, 0, 0, 0, 0, 0, 0); return 0; } }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_727f0(uint32_t* r) {
    g_lg = (uint32_t*)(uintptr_t)r[2]; g_lg[0] = 0; g_n = r[3];
    d2_client_727f0(r[0], (int32_t)r[1], o, s);
    r[4] = 0;
}

// Prior art: none (Storm SCOMP explode wrapper); body from the cached 1.13c decompile.
#include <cstdint>
extern "C++" {
extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);                          // 0x1B960 SMemFree
extern "C" uint32_t* Storm_Sub1C580(uint32_t size, const char* file, int32_t line, uint32_t flags);  // 0x1C580 SMemAlloc
uint8_t Storm_Sub0EC20(const void* readCb, const void* writeCb, uint32_t* state, void* user); // 0xEC20
extern const char Storm_Cb11C70[], Storm_Cb11C30[]; // code labels 0x11C70 / 0x11C30 (read/write callbacks)

// 1.13c Storm+0x22AB0 (fastcall: ecx=param_1, edx=param_2 ptr, stack param_3, param_4)
void Storm_Sub22AB0(uint32_t p1, uint32_t* p2, uint32_t p3, uint32_t p4) {
    static const char kF[] = "..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP";
    uint32_t* st = Storm_Sub1C580(0x3134, kF, 0x41C, 0);
    uint32_t ctx[6];
    ctx[0] = p1; ctx[1] = 0; ctx[2] = *p2; ctx[3] = p3; ctx[4] = 0; ctx[5] = p4;
    Storm_Sub0EC20(Storm_Cb11C70, Storm_Cb11C30, st, ctx);
    Storm_Sub1B960(st, kF, 0x42E);
    *p2 = ctx[1];
}
}

// 1.13c Fog+0x24330: MSVC CRT x87 math-error thunk, __fastcall(ecx = name, edx = type, stack cw, p4..p8),
// ST0 in. Fills the 8-dword _exception record on its frame {type slot (uninit), ecx, p5, p6, p7, p8, ST0}
// (p4 unused), calls the CRT matherr dispatcher Fog+0x247AA(type, &rec, &cw), then returns the record's
// retval (fld [ebp-8]: the dispatcher may rewrite it). Same bytes as Storm+0x3EA60.
#include <cstdint>
#include <cstring>
extern "C" void fog_sub_247aa(int fn, int* exc, uint16_t* fpcw);  // 1.13c Fog+0x247AA

double fog_24330_impl(uint32_t ecx, int type, uint16_t cw, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t p7,
                      uint32_t p8, double st0, void (*matherr)(int, uint32_t*, uint16_t*)) {  // 1.13c Fog+0x24330
    uint32_t rec[8] = {0, ecx, p5, p6, p7, p8, 0, 0};
    std::memcpy(rec + 6, &st0, 8);
    (void)p4;
    matherr(type, rec, &cw);
    double ret; std::memcpy(&ret, rec + 6, 8);
    return ret;
}
namespace {
void fog_matherr_default(int type, uint32_t* rec, uint16_t* cw) { fog_sub_247aa(type, (int*)rec, cw); }
}
extern "C" double fog_sub_24330(uint32_t ecx, int type, uint16_t cw, uint32_t p4, uint32_t p5, uint32_t p6,
                                uint32_t p7, uint32_t p8, double st0) {  // 1.13c Fog+0x24330
    return fog_24330_impl(ecx, type, cw, p4, p5, p6, p7, p8, st0, fog_matherr_default);
}

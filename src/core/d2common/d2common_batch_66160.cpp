// D2Common 1.13c batch: the load-all-tables driver. Sibling loaders are the vendored DATATBLS_*
// functions (real headers); unnamed ones are weak d2_fn_d2common_<rva> decls.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"

extern "C" {
#define W __attribute__((weak))
W void d2_fn_d2common_56a10(void* a);
void d2_port_d2common_55400(HD2ARCHIVE a);
W void d2_fn_d2common_77b70(void* a);
W void d2_ours_d2common_74400(HD2ARCHIVE a);
W void d2_fn_d2common_72c50(void* a);
W void d2_fn_d2common_69650(void* a);
W void d2_fn_d2common_69190(void* a);
void d2_port_d2common_60920(HD2ARCHIVE a);
W void d2_fn_d2common_5d570(void* a);
W void d2_fn_d2common_6c3a0(void* a);
W void d2_fn_d2common_6fba0(void* a, int32_t b, int32_t c);
void d2_port_d2common_6ccf0(HD2ARCHIVE a);
void* D2ReadFile(void*, const char* filename, uint32_t* size);
W void d2_fn_d2common_7e4f0(void* a);
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
W void Ordinal_10212(uint32_t p);
W char DAT_6fdf006c[4];
W char DAT_6fde9e20[4];
W char DAT_6fdf0070[4];
W char DAT_6fdf0074[4];
W char DAT_6fdf0b4c[8];
void d2_port_d2common_616d0(HD2ARCHIVE archive);

// Trace seam (the FN replay, src/core/fn/d2common_66160.cpp): when set, every callee of the driver is reported as
// (retail RVA, a, b, c) instead of run; the probe logs it as the capture's hook recorder does and returns the
// recorded answer (the AnimData pointer, the expfield file pointer). a = the archive handle for all but the EDI-only
// callees; b, c = the extra words of the call (p2, p3; the size slot for 0x9900; the size for 0x7E4F0). Null = retail's loaders.
uint32_t (*d2_66160_tr)(uint32_t rva, uint32_t a, uint32_t b, uint32_t c) = nullptr;
#define X(rva, A, B, C, ...) do { if (T) T(rva, (uint32_t)(uintptr_t)(A), (uint32_t)(B), (uint32_t)(C)); else { __VA_ARGS__; } } while (0)

// 1.13c D2Common+0x66160
EMSCRIPTEN_KEEPALIVE void d2_port_d2common_66160(HD2ARCHIVE h, int32_t p2, int32_t p3) {
 void* const a = (void*)h;
 auto T = d2_66160_tr;
 X(0x616d0, h, 0, 0, d2_port_d2common_616d0(h));
 X(0x72510, h, 0, 0, DATATBLS_LoadItemTypesTxt(h));
 X(0x56a10, h, 0, 0, d2_fn_d2common_56a10(a));
 X(0x55400, h, 0, 0, d2_port_d2common_55400(h));
 X(0x5f310, h, 0, 0, DATATBLS_LoadPetTypeTxt(h));
 X(0x82f90, h, 0, 0, DATATBLS_LoadOverlayTxt(h));
 X(0x73330, h, 0, 0, DATATBLS_LoadItemStatCostTxt(h));
 X(0x72e20, h, 0, 0, DATATBLS_LoadPropertiesTxt(h));
 X(0x7c8d0, h, 0, 0, DATATBLS_LoadMissilesTxt(h));
 X(0x5f6e0, h, 0, 0, DATATBLS_LoadStatesTxt(h));
 X(0x61ec0, h, 0, 0, DATATBLS_LoadSkills_SkillDescTxt(h));
 X(0x60a80, h, 0, 0, DATATBLS_LoadCharStatsTxt(h));
 X(0x80d50, h, 0, 0, DATATBLS_LoadArenaTxt(h));
 X(0x80000, h, 0, 0, DATATBLS_LoadCharTemplateTxt(h));
 X(0x787c0, h, 0, 0, DATATBLS_LoadItemsTxt(h));
 X(0x77f00, h, 0, 0, DATATBLS_LoadMagicSuffix_Prefix_AutomagicTxt(h));
 X(0x77b70, h, 0, 0, d2_fn_d2common_77b70(a));
 X(0x76f30, h, 0, 0, DATATBLS_LoadUniqueItemsTxt(h));
 X(0x75290, h, 0, 0, DATATBLS_LoadSets_SetItemsTxt(h));
 X(0x74660, h, 0, 0, DATATBLS_LoadGemsTxt(h));
 X(0x744b0, h, 0, 0, DATATBLS_LoadBooksTxt(h));
 X(0x74e70, h, 0, 0, DATATBLS_LoadQualityItemsTxt(h));
 X(0x74400, h, 0, 0, d2_ours_d2common_74400(h));
 X(0x71c20, h, 0, 0, DATATBLS_LoadRunesTxt(h));
 X(0x740c0, h, 0, 0, DATATBLS_LoadItemRatioTxt(h));
 X(0x72c50, h, 0, 0, d2_fn_d2common_72c50(a));
 X(0x19650, h, 0, 0, d2_fn_d2common_69650(a));
 X(0x19330, h, 0, 0, DATATBLS_LoadMonModeTxt(h));
 X(0x19190, h, 0, 0, d2_fn_d2common_69190(a));
 X(0x19110, h, 0, 0, DATATBLS_LoadCompositTxt(h));
 X(0x19090, h, 0, 0, DATATBLS_LoadArmTypeTxt(h));
 X(0x60920, h, 0, 0, d2_port_d2common_60920(h));
 if (T) *(uint32_t*)DAT_6fdf0b4c = T(0x41e50, (uint32_t)(uintptr_t)h, 0, 0);   // retail: mov ds:0x6FDF0B4C,eax (one dword; the high dword stays)
 else *(uint32_t*)DAT_6fdf0b4c = (uint32_t)(uintptr_t)DATATBLS_LoadAnimDataD2(h);
 X(0x5d570, h, 0, 0, d2_fn_d2common_5d570(a));
 X(0x6e130, h, 0, 0, DATATBLS_LoadLevelsTxt(h));
 X(0x6d9b0, h, 0, 0, DATATBLS_LoadLevelDefsBin(h));
 X(0x6d3a0, h, 0, 0, DATATBLS_LoadLevelTypesTxt(h));
 if (p2 != 0) X(0x6c3a0, h, p2, 0, d2_fn_d2common_6c3a0(a));
 X(0x70110, h, p2, 0, DATATBLS_LoadLvlPrestTxt(h, p2));
 X(0x6d190, h, 0, 0, DATATBLS_LoadLvlWarpTxt(h));
 X(0x6d050, h, 0, 0, DATATBLS_LoadLvlMazeTxt(h));
 X(0x6fba0, h, p2, p3, d2_fn_d2common_6fba0(a, p2, p3));
 X(0x6ccf0, h, 0, 0, d2_port_d2common_6ccf0(h));
 X(0x3f0c0, h, 0, 0, DATATBLS_LoadObjectsTxt(h));
 X(0x3ec60, h, 0, 0, DATATBLS_LoadObjGroupTxt(h));
 X(0x3ea90, h, 0, 0, DATATBLS_LoadShrinesTxt(h));
 X(0x820a0, h, 0, 0, DATATBLS_LoadInventoryTxt(h));
 uint32_t sz = 0, f = 0;
 // retail: wsprintfA("%s\\expfield.d2", "DATA\\GLOBAL") into a stack buffer, then 0x9900(archive, &size) (EAX = the buffer), then 0x7E4F0(file, size)
 if (T) { f = T(0x9900, (uint32_t)(uintptr_t)h, 0, (uint32_t)(uintptr_t)&sz); T(0x7e4f0, f, sz, 0); }
 else d2_fn_d2common_7e4f0(D2ReadFile(a, "DATA\\GLOBAL\\expfield.d2", &sz));
 X(0x13400, h, 0, 0, DATATBLS_LoadBeltsTxt(h));
 X(0x59f70, h, 0, 0, DATATBLS_LoadMonItemPercentTxt(h));
 X(0x1c4c0, h, 0, 0, DATATBLS_LoadCubeMainTxt(h));
 X(0x603f0, h, 0, 0, DATATBLS_LoadDifficultyLevelsTxt(h));
 if (*(uint32_t*)DAT_6fdf006c != 0) {
  const uint32_t line = *(uint32_t*)DAT_6fde9e20 == 0 ? 0x8f8 : 0x8f6;
  if (T) { T(0x9228, 0, line, 0); T(0x9246, *(uint32_t*)DAT_6fdf0070, 0, 0); }
  else { Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\DataTbls.cpp", line, 0); Ordinal_10212(*(uint32_t*)DAT_6fdf0070); }
  *(uint32_t*)DAT_6fdf0074 = 0;
 }
}
}

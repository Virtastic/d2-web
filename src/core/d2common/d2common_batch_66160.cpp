// D2Common 1.13c batch: the load-all-tables driver. Sibling loaders are the vendored DATATBLS_*
// functions (real headers); unnamed ones are weak d2_fn_d2common_<rva> decls.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"

extern "C" {
#define W __attribute__((weak))
W void d2_fn_d2common_56a10(void* a);
void d2_fn_d2common_55400(HD2ARCHIVE a);
W void d2_fn_d2common_77b70(void* a);
W void d2_fn_d2common_74400(HD2ARCHIVE a);
W void d2_fn_d2common_72c50(void* a);
W void d2_fn_d2common_69650(void* a);
W void d2_fn_d2common_69190(void* a);
void d2_fn_d2common_60920(HD2ARCHIVE a);
W void d2_fn_d2common_5d570(void* a);
W void d2_fn_d2common_6c3a0(void* a);
W void d2_fn_d2common_6fba0(void* a, int32_t b, int32_t c);
void d2_fn_d2common_6ccf0(HD2ARCHIVE a);
W void* d2_fn_d2common_9900(void* a, void* b);
W void d2_fn_d2common_7e4f0(void* a);
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
W void Ordinal_10212(uint32_t p);
W char DAT_6fdf006c[4];
W char DAT_6fde9e20[4];
W char DAT_6fdf0070[4];
W char DAT_6fdf0074[4];
W char DAT_6fdf0b4c[8];
void d2_fn_d2common_616d0(HD2ARCHIVE archive);

// 1.13c D2Common+0x66160
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_66160(HD2ARCHIVE h, int32_t p2, int32_t p3) {
 void* const a = (void*)h;
 d2_fn_d2common_616d0(h);
 DATATBLS_LoadItemTypesTxt(h);
 d2_fn_d2common_56a10(a);
 d2_fn_d2common_55400(h);
 DATATBLS_LoadPetTypeTxt(h);
 DATATBLS_LoadOverlayTxt(h);
 DATATBLS_LoadItemStatCostTxt(h);
 DATATBLS_LoadPropertiesTxt(h);
 DATATBLS_LoadMissilesTxt(h);
 DATATBLS_LoadStatesTxt(h);
 DATATBLS_LoadSkills_SkillDescTxt(h);
 DATATBLS_LoadCharStatsTxt(h);
 DATATBLS_LoadArenaTxt(h);
 DATATBLS_LoadCharTemplateTxt(h);
 DATATBLS_LoadItemsTxt(h);
 DATATBLS_LoadMagicSuffix_Prefix_AutomagicTxt(h);
 d2_fn_d2common_77b70(a);
 DATATBLS_LoadUniqueItemsTxt(h);
 DATATBLS_LoadSets_SetItemsTxt(h);
 DATATBLS_LoadGemsTxt(h);
 DATATBLS_LoadBooksTxt(h);
 DATATBLS_LoadQualityItemsTxt(h);
 d2_fn_d2common_74400(h);
 DATATBLS_LoadRunesTxt(h);
 DATATBLS_LoadItemRatioTxt(h);
 d2_fn_d2common_72c50(a);
 d2_fn_d2common_69650(a);
 DATATBLS_LoadMonModeTxt(h);
 d2_fn_d2common_69190(a);
 DATATBLS_LoadCompositTxt(h);
 DATATBLS_LoadArmTypeTxt(h);
 d2_fn_d2common_60920(h);
 *(uint64_t*)DAT_6fdf0b4c = (uint64_t)(intptr_t)DATATBLS_LoadAnimDataD2(h);
 d2_fn_d2common_5d570(a);
 DATATBLS_LoadLevelsTxt(h);
 DATATBLS_LoadLevelDefsBin(h);
 DATATBLS_LoadLevelTypesTxt(h);
 if (p2 != 0) d2_fn_d2common_6c3a0(a);
 DATATBLS_LoadLvlPrestTxt(h, p2);
 DATATBLS_LoadLvlWarpTxt(h);
 DATATBLS_LoadLvlMazeTxt(h);
 d2_fn_d2common_6fba0(a, p2, p3);
 d2_fn_d2common_6ccf0(h);
 DATATBLS_LoadObjectsTxt(h);
 DATATBLS_LoadObjGroupTxt(h);
 DATATBLS_LoadShrinesTxt(h);
 DATATBLS_LoadInventoryTxt(h);
 uint32_t tmp[1]; // retail builds "DATA\\GLOBAL\\expfield.d2" via wsprintfA; the path is consumed implicitly by 9900
 d2_fn_d2common_7e4f0(d2_fn_d2common_9900(a, tmp));
 DATATBLS_LoadBeltsTxt(h);
 DATATBLS_LoadMonItemPercentTxt(h);
 DATATBLS_LoadCubeMainTxt(h);
 DATATBLS_LoadDifficultyLevelsTxt(h);
 if (*(uint32_t*)DAT_6fdf006c != 0) {
  Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\DataTbls.cpp", *(uint32_t*)DAT_6fde9e20 == 0 ? 0x8f8 : 0x8f6, 0);
  Ordinal_10212(*(uint32_t*)DAT_6fdf0070);
  *(uint32_t*)DAT_6fdf0074 = 0;
 }
}
}

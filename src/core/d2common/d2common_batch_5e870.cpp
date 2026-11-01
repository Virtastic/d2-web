// D2Common 1.13c batch: DATATBLS_UnloadAllBins driver (flat form). Globals are retail 32-bit slots
// declared as weak 4-byte arrays; each guarded step reports a leaked allocation through Ordinal_10046.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"

extern "C" {
#define W __attribute__((weak))
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
__attribute__((weak)) W void Ordinal_10043(int32_t a, int32_t b);
W void Ordinal_10212(uint32_t p);
W void d2_fn_d2common_18f90();
W void d2_fn_d2common_53bd0();
W void d2_fn_d2common_54200();
W void d2_fn_d2common_5de00();
W void d2_fn_d2common_6c7e0();
W void d2_fn_d2common_71180();
W void d2_fn_d2common_716b0();
W char DAT_6fde9e20[4];
W char DAT_6fdefa5c[8];
W char DAT_6fdefbbc[8];
W char DAT_6fdefbd0[8];
W char DAT_6fdefbd8[8];
W char DAT_6fdefbe0[8];
W char DAT_6fdefbf0[8];
W char DAT_6fdefbf4[8];
W char DAT_6fdefd8c[8];
W char DAT_6fdefd94[8];
W char DAT_6fdefd9c[8];
W char DAT_6fdefda4[8];
W char DAT_6fdeff7c[8];
W char DAT_6fdeff80[8];
W char DAT_6fdeff88[8];
W char DAT_6fdf0a90[8];
W char DAT_6fdf0a94[8];
W char DAT_6fdf0a9c[8];
W char DAT_6fdf0ac0[8];
W char DAT_6fdf0ac4[8];
W char DAT_6fdf0af8[8];
W char DAT_6fdf0afc[8];
W char DAT_6fdf0b00[8];
W char DAT_6fdf0b38[8];
W char DAT_6fdf0b4c[8];
W char DAT_6fdf0b50[8];
W char DAT_6fdf0b54[8];
W char DAT_6fdf0b60[8];
W char DAT_6fdf0b8c[8];
W char DAT_6fdf0b90[8];
W char DAT_6fdf0b94[8];
W char DAT_6fdf0b98[8];
W char DAT_6fdf0b9c[8];
W char DAT_6fdf0ba0[8];
W char DAT_6fdf13dc[8];
W char DAT_6fdf13f8[8];
W char DAT_6fdf1400[8];
W char DAT_6fdf1404[8];
W char DAT_6fdf1408[8];
W char DAT_6fdf140c[8];
W char DAT_6fdf1410[8];
W char DAT_6fdf1414[8];
W char DAT_6fdf1418[8];
W char DAT_6fdf141c[8];
W char DAT_6fdf1420[8];
W char DAT_6fdf142c[8];
W char DAT_6fdf1434[8];
W char DAT_6fdf1448[8];
W char DAT_6fdf1454[8];
W char DAT_6fdf1464[8];

#define V(g) (*reinterpret_cast<volatile uint32_t*>(g))
static const char* const DT = "..\\Source\\D2Common\\DATATBLS\\DataTbls.cpp";
static void leak(char* g) { if (V(g)) Ordinal_10046(DT, V(DAT_6fde9e20) == 0 ? 0x8f8 : 0x8f6, 0); }

// 1.13c D2Common+0x5E870
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5e870() {

  d2_fn_d2common_5de00();
  leak(DAT_6fdf0a9c);
  DATATBLS_UnloadMissilesTxt();
  Ordinal_10212(V(DAT_6fdf0a90));
  leak(DAT_6fdf0a94);
  V(DAT_6fdf0a94) = 0;
  DATATBLS_UnloadSkills_SkillDescTxt();
  DATATBLS_UnloadItemStatCostTxt();
  DATATBLS_UnloadStatesTxt();
  leak(DAT_6fdf0ac0);
  Ordinal_10212(V(DAT_6fdf0ac4));
  DATATBLS_UnloadItemsTxt();
  leak(DAT_6fdeff7c);
  Ordinal_10212(V(DAT_6fdeff80));
  if (V(DAT_6fdefbbc)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\ItemTbls.cpp",0x2b7,0);
  }
  V(DAT_6fdefbbc) = 0;
  if (V(DAT_6fdefbe0)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\ItemTbls.cpp",0x30b,0);
  }
  V(DAT_6fdefbe0) = 0;
  Ordinal_10212(V(DAT_6fdf0af8));
  leak(DAT_6fdf0afc);
  V(DAT_6fdf0b00) = 0;
  d2_fn_d2common_716b0();
  leak(DAT_6fdefd94);
  leak(DAT_6fdefd9c);
  leak(DAT_6fdefbd0);
  leak(DAT_6fdefda4);
  leak(DAT_6fdefd8c);
  if (V(DAT_6fdefbf4)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\ItemTbls.cpp",0x80d,0);
    V(DAT_6fdefbf4) = 0;
    V(DAT_6fdefbf0) = 0;
  }
  d2_fn_d2common_71180();
  d2_fn_d2common_53bd0();
  d2_fn_d2common_18f90();
  leak(DAT_6fdf0b50);
  if (V(DAT_6fdf0b4c)) {
     Ordinal_10043(0xbb,0);
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\AnimTbls.cpp",0xbc,0);
  }
  d2_fn_d2common_54200();
  d2_fn_d2common_6c7e0();
  leak(DAT_6fdf0b38);
  if (V(DAT_6fdf1420)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\LvlTbls.cpp",0x1d9,0);
  }
  leak(DAT_6fdf141c);
  DATATBLS_UnloadLvlPrestTxt();
  leak(DAT_6fdf142c);
  leak(DAT_6fdf1434);
  if (V(DAT_6fdf1448)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\LvlTbls.cpp",1099,0);
  }
  DATATBLS_UnloadLvlSubTxt();
  if (V(DAT_6fdf1454)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\LvlTbls.cpp",0x562,0);
  }
  V(DAT_6fdf1454) = 0;
  leak(DAT_6fdf0b94);
  V(DAT_6fdf0b94) = 0;
  V(DAT_6fdf0b98) = 0;
  leak(DAT_6fdf0b9c);
  V(DAT_6fdf0b9c) = 0;
  V(DAT_6fdf0ba0) = 0;
  leak(DAT_6fdf0b8c);
  V(DAT_6fdf0b8c) = 0;
  V(DAT_6fdf0b90) = 0;
  leak(DAT_6fdefa5c);
  leak(DAT_6fdefbd8);
  Ordinal_10212(V(DAT_6fdeff88));
  if (V(DAT_6fdf1404)) {
    Ordinal_10046("..\\Source\\D2Common\\DATATBLS\\FieldTbls.cpp",0x5b,0);
  }
  V(DAT_6fdf1404) = 0;
  V(DAT_6fdf140c) = 0;
  V(DAT_6fdf1408) = 0;
  V(DAT_6fdf1410) = 0;
  V(DAT_6fdf1414) = 0;
  V(DAT_6fdf1418) = 0;
  leak(DAT_6fdf13dc);
  leak(DAT_6fdf0b60);
  leak(DAT_6fdf1400);
  V(DAT_6fdf1400) = 0;
  leak(DAT_6fdf1464);
  leak(DAT_6fdf13f8);
  V(DAT_6fdf13f8) = 0;
  leak(DAT_6fdf0b54);
}
}

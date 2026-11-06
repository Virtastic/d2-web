// D2Common 1.13c 0x5E870 (DATATBLS_UnloadAllBins driver): engine entry. The body is d2common_5e870.cpp; this file binds the
// real callees and the retail module words (weak 4-byte arrays) to its seam. Fog thunk 0x9228 is a pool free whose hidden
// register arguments (ecx, edx) the link signature drops, so the engine form reports through Ordinal_10046(file, line, 0) as before.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "d2common_5e870.h"

extern "C" {
#define W __attribute__((weak))
W void Ordinal_10046(const char* file, int32_t line, int32_t z);
W void Ordinal_10043(int32_t a, int32_t b);
W void Ordinal_10212(uint32_t p);
W void d2_batch_d2common_18f90();
void d2_batch_d2common_53bd0();
W void d2_fn_d2common_54200();
W void d2_port_d2common_5de00();
W void d2_fn_d2common_6c7e0();
W void d2_fn_d2common_71180();
W void d2_fn_d2common_716b0();
#define D2C5E870_W(n, r) W char DAT_##n[4];
D2C5E870_SLOTS(D2C5E870_W)
}

static const char* strOf(uint32_t rva) {
  switch (rva) {
    case D2C5E870_STR_FIELD: return "..\\Source\\D2Common\\DATATBLS\\FieldTbls.cpp";
    case D2C5E870_STR_ITEM: return "..\\Source\\D2Common\\DATATBLS\\ItemTbls.cpp";
    case D2C5E870_STR_LVL: return "..\\Source\\D2Common\\DATATBLS\\LvlTbls.cpp";
    case D2C5E870_STR_ANIM: return "..\\Source\\D2Common\\DATATBLS\\AnimTbls.cpp";
    default: return "..\\Source\\D2Common\\DATATBLS\\DataTbls.cpp";
  }
}
static void callReal(uint32_t rva) {
  switch (rva) {
    case 0x5de00: d2_port_d2common_5de00(); break;
    case 0x7c4f0: DATATBLS_UnloadMissilesTxt(); break;
    case 0x5db60: DATATBLS_UnloadSkills_SkillDescTxt(); break;
    case 0x71330: DATATBLS_UnloadItemStatCostTxt(); break;
    case 0x5d8b0: DATATBLS_UnloadStatesTxt(); break;
    case 0x71a50: DATATBLS_UnloadItemsTxt(); break;
    case 0x716b0: d2_fn_d2common_716b0(); break;
    case 0x71180: d2_fn_d2common_71180(); break;
    case 0x53bd0: d2_batch_d2common_53bd0(); break;
    case 0x18f90: d2_batch_d2common_18f90(); break;
    case 0x54200: d2_fn_d2common_54200(); break;
    case 0x6c7e0: d2_fn_d2common_6c7e0(); break;
    case 0x6c9c0: DATATBLS_UnloadLvlPrestTxt(); break;
    case 0x6c840: DATATBLS_UnloadLvlSubTxt(); break;
  }
}
static void free9228(uint32_t, uint32_t str, uint32_t line) { Ordinal_10046(strOf(str), (int32_t)line, 0); }
static void free9246(uint32_t p) { Ordinal_10212(p); }
static void anim922e(uint32_t, uint32_t, uint32_t a, uint32_t b) { Ordinal_10043((int32_t)a, (int32_t)b); }
static uint32_t rd32(uint32_t p) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(p)), 4); return v; }

// 1.13c D2Common+0x5E870
extern "C" EMSCRIPTEN_KEEPALIVE void d2_port_d2common_5e870() {
  D2Common5E870Slots s;
#define D2C5E870_LD(n, r) memcpy(&s.n, DAT_##n, 4);
  D2C5E870_SLOTS(D2C5E870_LD)
  const D2Common5E870Env e{ callReal, free9228, free9246, anim922e, rd32 };
  d2_seam_d2common_5e870(&s, &e);
#define D2C5E870_ST(n, r) memcpy(DAT_##n, &s.n, 4);
  D2C5E870_SLOTS(D2C5E870_ST)
}

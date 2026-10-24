// Real extern "C" D2Client+0xA3D20 and +0x46640, from the decompile. Callees/globals bind weakly (pattern of d2client_batch_b70).
#include <cstdint>
#include <cstdlib>
extern "C" {
#define W __attribute__((weak))
W extern int32_t DAT_6fbcbc08, DAT_6fbaada8, DAT_6fbcbc00, DAT_6fbcbc04, DAT_6fbcc2fc, DAT_6fb90ea0;
W extern uint8_t DAT_6fb80780[], DAT_6fb8e858[], DAT_6fb8e900[], DAT_6fb8359f[];
W extern int32_t DAT_6fbc973b, DAT_6fbc973f, DAT_6fbc97c0, DAT_6fbc97c4, DAT_6fbc9757, DAT_6fbc975b,
    DAT_6fbc975f, DAT_6fbc9763, DAT_6fbc976f, DAT_6fbc9773, DAT_6fbc9767, DAT_6fbc976b;
W extern void** DAT_6fbc977b;
W int32_t Ordinal_10048(void);
W void FUN_6faeccc0(int32_t, int32_t);   // D2Client+0x3CCC0
W void FUN_6faecc60(int32_t, int32_t);   // D2Client+0x3CC60
W uint8_t FUN_6faf2090(void);            // D2Client+0x42090
W void Ordinal_10043(int32_t, int32_t);  // Fog
W void Ordinal_10065(int32_t);           // D2CMP CloseCelFile
W void Ordinal_10901(int32_t, int32_t);  // D2Common QUESTRECORD_FreeRecord
W uint32_t Ordinal_10265(int32_t);
W void Ordinal_10024(const void*, int32_t);
void d2client_FUN_6fb53d20(void);

static inline int32_t rd32(int32_t a) { return *(int32_t*)(uintptr_t)(uint32_t)a; }

// 1.13c D2Client+0xA3D20
void d2client_FUN_6fb53d20(void) {
  if (--DAT_6fbcbc08 == 0) {
    if (DAT_6fbaada8 != 0) FUN_6faeccc0(Ordinal_10048(), (int32_t)(intptr_t)DAT_6fb80780);
    FUN_6faecc60(Ordinal_10048(), (int32_t)(intptr_t)DAT_6fb8e858);
    if (FUN_6faf2090() != 0) FUN_6faecc60(Ordinal_10048(), (int32_t)(intptr_t)DAT_6fb8e900);
    int32_t iVar1 = DAT_6fbcbc00;
    int32_t iVar3 = rd32(iVar1 + 0xb08);
    DAT_6fb90ea0 = 1; DAT_6fbcc2fc = 0; DAT_6fbcbc04 = 0;
    while (iVar3 != 0) { iVar3 = rd32(iVar3 + 4); Ordinal_10043(0x430, 0); }
    if (rd32(iVar1 + 0xc) != 0 && (iVar3 = rd32(iVar1 + 8)) != 0) {
      Ordinal_10065(iVar3);
      Ordinal_10043(0x7a, 0);
    }
    int32_t uVar4 = rd32(iVar1 + 0xb0c);
    Ordinal_10043(0x43b, 0);
    DAT_6fbcbc00 = uVar4;
    return;
  }
  int32_t u = Ordinal_10265(0x441);
  Ordinal_10024(DAT_6fb8359f, u);
  _Exit(-1);
}

// 1.13c D2Client+0x46640
void d2client_FUN_6faf6640(void) {
  Ordinal_10901(0, DAT_6fbc973b);
  Ordinal_10901(0, DAT_6fbc973f);
  DAT_6fbc973b = 0; DAT_6fbc973f = 0;
  int32_t* cel[2] = {&DAT_6fbc97c0, &DAT_6fbc97c4};
  for (int32_t* c : cel) {
    if (*c != 0) { Ordinal_10065(*c); Ordinal_10043(0x7a, 0); *c = 0; }
  }
  if (DAT_6fbc977b != 0) { ((void (*)(int32_t))*(void**)DAT_6fbc977b)(1); DAT_6fbc977b = 0; }
  int32_t* h[8] = {&DAT_6fbc9757, &DAT_6fbc975b, &DAT_6fbc975f, &DAT_6fbc9763,
                   &DAT_6fbc976f, &DAT_6fbc9773, &DAT_6fbc9767, &DAT_6fbc976b};
  for (int32_t* p : h) if (*p != 0) { d2client_FUN_6fb53d20(); *p = 0; }
}
}

// Real D2Client+0x45EF0 and +0x46A20 (caller 0xC0250), from the decompile.
#include <cstdio>
extern "C" {
W extern int32_t DAT_6fbc97b8, DAT_6fbc97b4, DAT_6fba3b9c;
W extern uint8_t DAT_6fbc9660[], DAT_6fbc9663, DAT_6fbd2305, DAT_6fbd2301, DAT_6fbd2309[], DAT_6fba37b9[];
W extern int32_t DAT_6fbd22f8;
W extern uint16_t DAT_6fba23e8, DAT_6fbc9662;
W uint32_t GetTickCount(void);
W int32_t Ordinal_10939(int32_t);
W int32_t FUN_6fadb420(int32_t);   // D2Client+0x2B420
void d2client_FUN_6faf5ef0(void);

// 1.13c D2Client+0x45EF0
void d2client_FUN_6faf5ef0(void) {
  DAT_6fbc97b8 = 0; DAT_6fbc97b4 = 0;
  GetTickCount();
  for (int i = 0; i < 0x58 * 4; i++) DAT_6fbc9660[i] = 0;
  DAT_6fbc973b = Ordinal_10939(0); DAT_6fbc973f = Ordinal_10939(0);
  DAT_6fbc9663 = 0;
  char buf[256];
  snprintf(buf, sizeof buf, "%s\\ui\\panel\\buysell", "DATA\\GLOBAL"); DAT_6fbc97c0 = FUN_6fadb420(0);
  snprintf(buf, sizeof buf, "%s\\ui\\menu\\goldbtn", "DATA\\GLOBAL"); DAT_6fbc97c4 = FUN_6fadb420(0);
  DAT_6fbd2305 = 0; DAT_6fbd22f8 = 0; for (int i = 0; i < 4; i++) DAT_6fbd2309[i] = 0; DAT_6fbd2301 = 0;
}

// 1.13c D2Client+0x46A20
void d2client_FUN_6faf6a20(void) {
  DAT_6fba23e8 = 0xffff;
  d2client_FUN_6faf6640();
  d2client_FUN_6faf5ef0();
  DAT_6fbc9662 = 0;
  for (int i = 0; i < DAT_6fba3b9c; i++) { DAT_6fba37b9[i * 0x16] = 0; DAT_6fba37b9[i * 0x16 + 1] = 0; }
}
}

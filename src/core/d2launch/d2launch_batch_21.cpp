// D2Launch UI glue batch 21. Retail globals are weak externs; state lives in the screen structs.
#include <cstdint>

__attribute__((weak)) extern uint8_t* DAT_6fa65abc;  // screen state block
__attribute__((weak)) extern int32_t Ordinal_10009(void);  // D2Gfx import

// 1.13c D2Launch+0x11AA0
void FUN_6fa51aa0(char mode) {
  uint8_t* p = DAT_6fa65abc;
  p[0x1ea] = (uint8_t)mode;
  for (int i = 0x81; i <= 0x87; i++) p[i] = 0;
  static const uint8_t slot[7] = {0x81, 0x83, 0x84, 0x82, 0x85, 0x86, 0x87};
  if ((uint8_t)mode < 7) p[slot[(uint8_t)mode]] = 1;
}

// 1.13c D2Launch+0x110C0
extern int32_t DAT_6fa61e58, DAT_6fa61408, DAT_6fa61438, DAT_6fa61468, DAT_6fa614f8, DAT_6fa61528,
    DAT_6fa620f8, DAT_6fa634d8, DAT_6fa63508, DAT_6fa63298, DAT_6fa636b8, DAT_6fa632f8,
    DAT_6fa63358, DAT_6fa633b8, DAT_6fa63fb8, DAT_6fa64168;
void FUN_6fa510c0(void) {
  if (Ordinal_10009() != 9) return;
  DAT_6fa61e58 = 0x9b; DAT_6fa61408 = 0x14e; DAT_6fa61438 = 0x184; DAT_6fa61468 = 0x1ba;
  DAT_6fa614f8 = 0xf0; DAT_6fa61528 = 0x158; DAT_6fa620f8 = 0x9c; DAT_6fa634d8 = 0x1fa;
  DAT_6fa63508 = 0x22b; DAT_6fa63298 = 0x23f; DAT_6fa636b8 = 0x22b; DAT_6fa632f8 = 0x22b;
  DAT_6fa63358 = 0x23f; DAT_6fa633b8 = 0x253; DAT_6fa63fb8 = 0x46; DAT_6fa64168 = 0xee;
}

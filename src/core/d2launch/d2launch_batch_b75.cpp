// D2Launch CRT batch b75. Skipped (SEH/direct-Win32/existing): 167D 1906 4429 44AC 46DB 4714 4785 58E3 5951.
#include <cstdint>
#include <initializer_list>

__attribute__((weak)) extern void FUN_6fa43479(int32_t lockNum);  // _unlock
__attribute__((weak)) extern uint32_t DAT_6fa66f84;               // _nhandle
__attribute__((weak)) extern uint8_t* DAT_6fa66fa0[];             // ioinfo blocks, 0x24-byte entries

// 1.13c D2Launch+0x48B7
void FUN_6fa448b7(void) { FUN_6fa43479(0xd); }

// 1.13c D2Launch+0x48C3
void FUN_6fa448c3(void) { FUN_6fa43479(0xc); }

// 1.13c D2Launch+0x5927  __isatty
int32_t __isatty(int32_t fh) {
  if (DAT_6fa66f84 <= (uint32_t)fh) return 0;
  return (int32_t)(int8_t)DAT_6fa66fa0[fh >> 5][4 + (fh & 0x1f) * 0x24] & 0x40;
}

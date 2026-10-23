// Storm sub_3870: destructor of a record with owned blocks at +0x160, 16 x 0x14 elements at +0x14, +8.
#include <cstdint>
extern "C" {
void sub_1B960(int* esi, const char* name, int n);  // Storm+0x1B960, ESI=arg (SMemFree)
extern const char s__E_6fc41c74[];                  // Storm .rdata @0x6FC41C74
extern const char s__I_6fc41c80[];                  // Storm .rdata @0x6FC41C80
}

// 1.13c Storm+0x03820: element dtor, frees the block at +8.
static void sub_3820(intptr_t e) {
  int* p = *(int**)(e + 8);
  if (p) sub_1B960(p, s__I_6fc41c80, -2);
}

// 1.13c Storm+0x03870
void sub_3870(intptr_t self) {
  int* p = *(int**)(self + 0x160);
  if (p) sub_1B960(p, s__E_6fc41c74, -2);
  for (int i = 15; i >= 0; i--) sub_3820(self + 0x14 + i * 0x14);  // eh_vector_destructor: last to first
  p = *(int**)(self + 8);
  if (p) sub_1B960(p, s__I_6fc41c80, -2);
}

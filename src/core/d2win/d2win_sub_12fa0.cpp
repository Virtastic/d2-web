// 1.13c D2Win+0x12FA0 (size 53): DrawUnicodeText. thiscall(this, a, b, flag); seams 0x11FB0 and 0x12940.
#include <cstdint>
struct D2Win12FA0Env {
  int32_t (*sub_11fb0)();
  void (*sub_12940)(void* self, int32_t a, int32_t v, int32_t b, int32_t flag);
};
// 1.13c D2Win+0x12FA0
void d2win_sub_12fa0(D2Win12FA0Env& e, void* self, int32_t a, int32_t b, int32_t flag) {
  int32_t v = 0;
  if (flag != 0) v = e.sub_11fb0() + 8;
  e.sub_12940(self, a, v, b, flag);
}

// 1.13c D2Win+0x12FA0 (size 53): DrawUnicodeText. ret 0xC; ECX = this, EDX = x, stack a, b, flag.
// Seams: 0x11FB0 (EAX = this, returns the width) and 0x12940 (EAX = x; stack this, a, v, b, flag; ret 0x14).
#include <cstdint>
struct D2Win12FA0Env {
  int32_t (*sub_11fb0)(void* self);
  void (*sub_12940)(int32_t x, void* self, int32_t a, int32_t v, int32_t b, int32_t flag);
};
// 1.13c D2Win+0x12FA0
extern "C" void d2win_sub_12fa0(const void* env, void* self, int32_t x, int32_t a, int32_t b, int32_t flag) {
  const D2Win12FA0Env& e = *static_cast<const D2Win12FA0Env*>(env);
  int32_t v = 0;
  if (flag != 0) v = e.sub_11fb0(self) + 8;
  e.sub_12940(x, self, a, v, b, flag);
}

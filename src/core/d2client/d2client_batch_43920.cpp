// Batch 43920: packet drain loop. Derived from cached 1.13c code.
// Env-seam: ordinals and callees are function pointers.
#include <cstdint>
namespace d2client_43920 {
struct Env {
  int32_t (*ord10020)(uint8_t* buf, int32_t n);   // Ordinal_10020 (poll queue A)
  int32_t (*ord10008)(uint8_t* buf, int32_t n);   // Ordinal_10008 (poll queue B)
  void (*sleep0)();                               // Sleep(0)
  void (*fnACD70)(const uint8_t*);                // 0xACD70
  void (*fnACBF0)(const uint8_t*);                // 0xACBF0
  void (*fnACE20)(int32_t);                       // 0xACE20
  int32_t* g_c39c;                                // DAT_6fbcc39c
};
// 1.13c D2Client+0x43920
void sub_43920(Env& e) {
  uint8_t buf[512]; int32_t r;
  do {
    r = e.ord10020(buf, 0x200);
    if (r != -1) switch (buf[0]) {
      case 0xaf: *e.g_c39c = 1; break;
      case 0xb0: *e.g_c39c = 0; break;
      case 0xb3: e.fnACD70(buf); break;
      case 0xb4: e.fnACBF0(buf); break;
    }
    e.sleep0();
  } while (r != -1);
  do { r = e.ord10008(buf, 0x200); e.fnACE20(r); e.sleep0(); } while (r != -1);
}
}

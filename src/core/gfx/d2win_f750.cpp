// 1.13c D2Win+0xF750 (size 63): set rect fields of a type-6 control record (Fog seam: 10265(0xab)/10024 fatal).
#include <cstdint>
struct D2WinSetRectEnv {
  void (*fatal)(uint32_t id);  // Fog #10265(0xab) -> #10024("", it), _exit(-1); does not return
};
// 1.13c D2Win+0xF750
void d2win_f750(D2WinSetRectEnv& e, int* p, int a, int b, int c, int d) {
  if (*p != 6) e.fatal(0xab);
  p[3] = a; p[4] = b; p[5] = c; p[6] = d;
}

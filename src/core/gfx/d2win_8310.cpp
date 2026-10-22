// 1.13c D2Win+0x8310 (size 51): type-4 control record accessor, returns dword 0x15; null -> fatal 0x23e, wrong type -> 0x23f (Fog 10265/10024, _exit).
#include <cstdint>
struct D2WinGet8310Env {
  void (*fatal)(uint32_t id);  // Fog #10265(id) -> #10024("", it), _exit(-1); does not return
};
// 1.13c D2Win+0x8310
int32_t d2win_8310(D2WinGet8310Env& e, int32_t* p) {
  if (!p) e.fatal(0x23e);
  else if (*p != 4) e.fatal(0x23f);
  else return p[0x15];
  return 0;
}

// D2Client message pump: platform seams as function pointers, no game memory.
#include <cstdint>
namespace d2client_msgpump {
struct Env {
 int32_t (*peek)();            // PeekMessageA(no remove)
 int32_t (*getMsg)();          // GetMessageA; 0 == WM_QUIT
 void (*accel)();              // TranslateAccelerator if DAT_6fbcc0a4 != 0
 void (*dispatch)();           // TranslateMessage + DispatchMessageA
 void (*sleep10)();
 uint32_t sleepGate;           // DAT_6fba4a60
 uint32_t state;               // DAT_6fbcc394
};
// 1.13c D2Client+0x3CB00  (cb = EBX callback, null allowed)
int32_t sub_3CB00(const Env& e, int32_t (*cb)(int32_t)) {
 int32_t i = 0, got = 1;
 do {
  if (!e.peek()) {
   if (cb) { int32_t r = cb(i++); if (r) return got == 0 ? 1 : r; }
   if (e.sleepGate == 0 && e.state != 6 && e.state != 8) e.sleep10();
  } else {
   got = e.getMsg();
   if (got) e.accel();
   e.dispatch();
  }
 } while (got);
 return 1;
}
}

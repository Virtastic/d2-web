// D2Launch n57: small leaf/helper functions. Win32/Ordinal services arrive as host seams.
// Register-passed args (EAX/ESI/EBX) are explicit parameters.
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 1.13c D2LAUNCH+0x149E0
void d2launch_sub_149E0(void (*release)(uint32_t h), uint32_t* handles, int32_t* count) {
  for (int32_t i = 0; i < *count; i++) release(handles[i]);  // Ordinal_10050
  *count = 0;
}

// 1.13c D2LAUNCH+0x129D0  (progress bar: percent from callback, bar width scaled)
void d2launch_sub_129D0(void (*pump)(void), int32_t (*percent)(void), int32_t (*width)(uint32_t),
                        void (*setw)(uint32_t, int32_t), void (*settext)(const char*),
                        uint32_t bar, int32_t hastext) {
  pump();
  int32_t p = percent();
  if (p > 100) p = 100;
  setw(bar, (width(bar) * p) / 100);
  if (hastext) { char b[16]; b[0] = 0; int32_t v = p, i = 0; char t[12];
    do { t[i++] = char('0' + v % 10); v /= 10; } while (v);
    int32_t k = 0; while (i) b[k++] = t[--i]; b[k++] = '%'; b[k] = 0; settext(b); }
}

// 1.13c D2LAUNCH+0x128D0  (registry "Diablo II"/"Aux Battle.net" version quad flag)
uint8_t d2launch_sub_128D0(int32_t (*regread)(const char*, const char*, int32_t, char*, int32_t),
                           int32_t (*sscan4)(const char*, uint32_t*)) {
  char s[20] = {'d'};
  uint32_t q[4] = {0, 0, 0, 0x22};
  if (regread("Diablo II", "Aux Battle.net", 0, s, 100) != 0) {  // Ordinal_422
    sscan4(s, q);                                               // "%d.%d.%d.%d"
    if (q[3] & 0x10) return uint8_t(~uint8_t(q[3]) >> 7);
  }
  return 0;
}

// 1.13c D2LAUNCH+0x122C0
void d2launch_sub_122C0(const void* (*gettext)(void), size_t (*ulen)(const void*),
                        void (*update)(void), int32_t flag) {
  const void* a = gettext();
  const void* b = gettext();
  if (a && b) { ulen(a); ulen(b); }
  if (flag) update();
}

// 1.13c D2LAUNCH+0x121B0  (ESI = ctl; param_1 = set/clear)
int32_t d2launch_sub_121B0(void (*pump)(uint32_t), void (*settext)(uint32_t), const void* (*getstr)(void),
                           int32_t* sel, int32_t* cur, int32_t set, uint32_t ctl) {
  if (*sel == -1) {
    if (set) {
      pump(ctl); getstr(); settext(ctl); pump(ctl); getstr(); settext(ctl);
      *cur = int32_t(ctl);
      return 1;
    }
    if (*cur == int32_t(ctl)) { pump(ctl); pump(ctl); }
  }
  return 1;
}

// 1.13c D2LAUNCH+0x11CB0  (message pump; drops mouse msgs 0x204,0x205,0x207,0x208, 0x12, 0x102, 0x201-0x202 kept)
struct D2LMsg { uint32_t hwnd, message, wp, lp, time, x, y; };
void d2launch_sub_11CB0(int32_t (*peek)(D2LMsg*), void (*xlate)(D2LMsg*), void (*dispatch)(D2LMsg*)) {
  D2LMsg m;
  while (peek(&m)) {
    if (m.message > 0x202) {
      if (m.message == 0x204 || m.message == 0x205 || m.message == 0x207 || m.message == 0x208) continue;
    } else if (m.message < 0x201 && m.message != 0x12 && m.message != 0x102) {
    } else continue;
    xlate(&m); dispatch(&m);
  }
}

// 1.13c D2LAUNCH+0x11BE0  (EAX = msg*, msg->message at +4; double-click detector)
bool d2launch_sub_11BE0(const uint32_t* msg, uint32_t (*tick)(void), void (*cursor)(int32_t*),
                        int32_t* lastx, int32_t* lasty, uint32_t* lasttick) {
  uint32_t m = msg[1];
  uint32_t t = tick();
  int32_t dt = int32_t(t - *lasttick);
  int32_t pt[2]; cursor(pt);
  int32_t dx = pt[0] - *lastx, dy = pt[1] - *lasty;
  *lasty = pt[1]; *lasttick = t; *lastx = pt[0];
  return (std::abs(dx) + std::abs(dy)) < 5 && dt < 0x12d && m == 0x201;
}

// 1.13c D2LAUNCH+0x12560  (host "127.0.0.1" -> local branch, else remote branch)
void d2launch_sub_12560(const char* (*gethost)(void), void (*a)(void), void (*b)(void),
                        void (*c)(void), void (*d)(void), void (*lbl)(void)) {
  const char* h = gethost();  // Ordinal_10013
  a();                        // Ordinal_10106
  if (strncmp(h, "127.0.0.1", 10) == 0) { b(); c(); d(); d(); d(); }
  else { lbl(); d(); d(); d(); }
}

// D2Launch n54: sub_9E40 (class-name string by selector) and sub_AF00 (CharSel list paint).
// Host seams (D2Lang/D2Win ordinals, D2Launch globals) arrive as arguments.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

struct D2LCharSelEnv {
  const void* (*getstr)(int32_t id);                         // 6fa49b32 (Ordinal_10003)
  void (*ustrcpy)(void* dst, const void* src);               // Unicode::strcpy
  void (*ustrcat)(void* dst, const void* src);               // Unicode::strcat
  void (*uwin2uni)(void* dst, const char* src, int32_t n);   // Unicode::win2Unicode
  void (*usprintf)(int32_t n, void* dst, const void* fmt, int32_t v);  // Unicode::sprintf
  void (*pump)(uint32_t ctl);                                // 6fa4996a (Ordinal_10106)
  int32_t (*getkey)(void);                                   // 6fa49b2c (Ordinal_10009)
  void (*scroll)(uint32_t ctl, int32_t delta);               // 6fa498f8 (Ordinal_10094)
  void (*setflags)(uint32_t ctl, uint32_t f);                // 6fa499e8 (Ordinal_10194)
  void (*setx)(uint32_t ctl, int32_t x);                     // 6fa49a30 (Ordinal_10140)
  void (*sety)(uint32_t ctl, int32_t y);                     // 6fa499d6 (Ordinal_10179)
  void (*settext)(uint32_t ctl, const void* s, int32_t a, int32_t b, int32_t c);  // 6fa499be (Ordinal_10000)
  void (*setfont)(uint32_t ctl, const void* s, int32_t font);  // 6fa498ce (Ordinal_10022)
  const void* (*classname)(int32_t cls, int32_t b, int32_t hc, int32_t exp);  // 6fa49a18 (Ordinal_10205)
  uint32_t (*now)(void);                                     // _time32
  uint32_t (*ctl_at)(int32_t slot);                          // DAT_6fa65734[slot]
  const uint8_t* (*next)(const uint8_t* e);                  // *(e+0x34c)
  uint32_t (*ctl_of)(const uint8_t* e);                      // *(e+0x338)
  const uint32_t* bitmasks;                                  // gdwBitMasks
  const uint32_t* invbitmasks;                               // gdwInvBitMasks
  const uint8_t* head;                                       // DAT_6fa65ec8
  int32_t visible, scrollpos, basey, realm, realmflag, pumpctl;  // 64dbc 65ed4 65710 65ad4 65eb8 65edc
};

// 1.13c D2Launch+0x9E40  (EAX = selector, ESI = dst)
void d2launch_sub_9E40(const D2LCharSelEnv* h, uint32_t sel, void* dst) {
  static const int32_t ids[7] = {0xfab, 0xfaa, 0xfa9, 0xfa8, 0xfa7, 0x2771, 0x2772};
  h->ustrcpy(dst, h->getstr(sel > 6 ? 0x1507 : ids[sel]));
}

// 1.13c D2Launch+0xAF00
void d2launch_sub_AF00(const D2LCharSelEnv* h) {
  int32_t y = h->basey - 13, x = 0x25, idx = 0;
  for (int32_t i = 0; i < h->visible; i++) {
    h->pump(h->ctl_at(i));
    int32_t k = h->getkey();
    if (k == 6 || k == 9 || k == 10) h->scroll(h->ctl_at(i), -2);
    else if (k == 7) h->scroll(h->ctl_at(i), -7);
  }
  if (h->pumpctl) h->pump((uint32_t)h->pumpctl);
  if (h->realm == 1 && h->realmflag) return;
  for (const uint8_t* e = h->head; e; e = h->next(e), idx++) {
    uint32_t ctl = h->ctl_of(e);
    if (idx < h->scrollpos || idx >= h->scrollpos + h->visible) { h->setflags(ctl, 0); continue; }
    int32_t slot = idx - h->scrollpos;
    h->setx(ctl, x + 0x1e);
    h->sety(ctl, y);
    uint16_t fl; memcpy(&fl, e + 0x324, 2);
    uint32_t f = (fl & 8) ? (h->bitmasks[4] | 1) : (h->invbitmasks[4] & 1);
    f = (fl & 4) ? (f | h->bitmasks[5]) : (f & h->invbitmasks[5]);
    h->setflags(ctl, f);
    int32_t type; memcpy(&type, e + 0x334, 4);
    if (type == 2) {
      h->settext(h->ctl_at(slot), h->getstr(0x2b3b), 1, 0, 0);
    } else if (type == 0) {
      uint16_t ub[100] = {0}, uc[100] = {0}, ud[100] = {0}, ue[10] = {0};
      char a[100]; a[0] = 0;
      int32_t font = (fl & 4) ? 1 : 4;
      const void* cn = h->classname(e[0x320], (fl >> 8) & 0x1f, fl & 4, fl & 0x20);
      if (cn) h->setfont(h->ctl_at(slot), cn, font);
      strcat(a, (const char*)e);
      if (e[0x32a]) snprintf(a + strlen(a), sizeof a - strlen(a), " {%s}", (const char*)e + 0x32a);
      h->setfont(h->ctl_at(slot), a, font);
      d2launch_sub_9E40(h, e[0x320], ud);
      uint16_t lvl; memcpy(&lvl, e + 0x322, 2);
      snprintf(a, sizeof a, " %d ", lvl);
      h->uwin2uni(ue, a, 9);
      h->ustrcpy(uc, h->getstr(0xfd9));
      h->ustrcat(uc, ue);
      h->ustrcat(uc, ud);
      h->settext(h->ctl_at(slot), uc, 0, 0, 0);
      if (fl & 0x20) h->settext(h->ctl_at(slot), h->getstr(0x58cb), 2, 0, 0);
      if (e[0x32e]) h->settext(h->ctl_at(slot), h->getstr(0x2aaf), 4, 0, 0);
      uint32_t exp; memcpy(&exp, e + 0x348, 4);
      uint32_t t = h->now(), days = exp > t ? (exp - t) / 86400u : 0;
      if (h->realm == 1 && days <= 0x3c) {
        int32_t flag = 1, id = days == 0 ? 0x2b7e : days < 2 ? 0x2b7c : 0x2b7d;
        if (days >= 15) flag = 0;
        h->usprintf(100, ub, h->getstr(id), (int32_t)days);
        h->settext(h->ctl_at(slot), ub, flag, 0, 0);
      }
    }
    if (x == 0x25) x = 0x135; else { x = 0x25; y += 0x5d; }
  }
}

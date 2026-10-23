// 1.13c Storm+0x13170 -- SNET periodic pump: per-peer resend/ack timers. Session is a byte block with
// retail's 32-bit field offsets; list nodes (32-bit pointers in retail) are resolved via hooks.
#include <cstdint>
#include <cstring>
extern "C" uint16_t Storm_SubFF40(uint32_t, uint32_t, const uint8_t*);
struct Snet13170Hooks {
  uint8_t* (*node)(uint32_t handle);                       // handle -> node bytes
  uint8_t* (*sessionGet)();                                // sub_12690
  void (*expire)();                                        // sub_11630
  void (*enqueue)(uint32_t, uint8_t, const uint32_t*, uint32_t);  // sub_12F40
  void (*sendPkt)(uint8_t* sess, const uint8_t* pkt, uint32_t n); // sub_10D20 via sess+0x108
  void (*proc)(uint8_t* sess, uint8_t* pktNode);           // sub_11D90
  int32_t gap, gapA, gapB, gapC; uint32_t rate, base;       // DAT_6fc41b48/38/44/..., NET+0x21c/0x228
};
template <class T> static T& at(uint8_t* b, uint32_t o) { return *reinterpret_cast<T*>(b + o); }
static void pkt(const Snet13170Hooks& h, uint8_t* s, uint8_t owner, uint8_t type, uint8_t idx, uint16_t seq, uint16_t tab) {
  uint8_t p[12] = {0, 0, 0x0c, 0, (uint8_t)seq, (uint8_t)(seq >> 8), (uint8_t)tab, (uint8_t)(tab >> 8), idx, 0, owner, type};
  uint16_t r = Storm_SubFF40(0, 10, p + 2); uint32_t lo = r & 0xff, hi = r >> 8;
  uint8_t b = (uint8_t)(-(int)((lo + hi) % 0xff) - 1);
  p[0] = (uint8_t)(-1 - (int)((b + hi) % 0xff)); p[1] = b;
  h.sendPkt(s, p, 12);
}
extern "C" uint32_t Storm_Sub13170(uint32_t now, uint8_t* s, const Snet13170Hooks& h) {  // 1.13c Storm+0x13170
  uint32_t best = 0xffffffffu;
  uint8_t* me = h.sessionGet();
  if (!s || !me) return best;
  for (;;) {
    uint32_t nextSess = at<int32_t>(s, 4) < 1 ? 0 : at<uint32_t>(s, 4);
    bool live = true;
    if (at<int8_t>(s, 0x212) == -1) {
      if (now - at<uint32_t>(s, 0x11c) >= 50000) { h.expire(); live = false; }
      else live = at<int32_t>(s, 0x204) != 0;
    }
    if (live) {
      if (now - at<uint32_t>(s, 0x124) > 19999 && at<int32_t>(s, 0x204) == 0) {
        at<uint32_t>(s, 0x124) = now; h.enqueue(0, 4, nullptr, 0);
      }
      for (int i = 0; i < 4; i++) {
        if (i == 3) continue;
        int32_t n = at<int32_t>(s, 0x168 + 12 * i);
        uint16_t lastAck = at<uint16_t>(s, 0x1de + 2 * i) - 1;
        if (n <= 0) continue;
        uint8_t* nd = h.node(n); bool done = false;
        for (;;) {
          uint16_t v = at<uint16_t>(h.node(at<uint32_t>(nd, 0xc)), 4);
          if ((int)v - (int)lastAck >= 2) break;
          int32_t nn = at<int32_t>(nd, 4);
          if (nn < 1) { done = true; break; }
          lastAck = v; nd = h.node(nn);
        }
        if (done) continue;
        uint32_t t = at<uint32_t>(nd, 0x1c); int32_t left;
        if (t == 0 || (left = (int32_t)(t - now + h.gap)) < 1) {
          uint16_t tab = at<uint16_t>(s, 0x1de + 2 * i);
          at<uint32_t>(nd, 0x1c) = now;
          pkt(h, s, at<uint8_t>(me, 0x212), 2, (uint8_t)i, lastAck + 1, tab);
          at<uint16_t>(s, 0x1e6 + 2 * i) = tab; at<uint32_t>(s, 0x1f0 + 4 * i) = 0;
        } else if ((uint32_t)left <= best) best = left;
      }
      for (int i = 0; i < 4; i++) {
        int32_t n = at<int32_t>(s, 0x138 + 12 * i);
        if (n <= 0 || i == 2 || i == 3) continue;
        uint8_t* nd = h.node(n); uint32_t t = at<uint32_t>(nd, 0x1c); int32_t left;
        if (t == 0 || (left = (int32_t)(t - now + (h.base ? (uint32_t)((h.gapC + 0x80 + at<int32_t>(nd, 0x10)) * 1000) / h.rate : 0) + 200 + h.gap * 2)) < 1) {
          at<uint32_t>(nd, 0x1c) = now; h.proc(s, h.node(at<uint32_t>(nd, 0xc)));
        } else if ((uint32_t)left <= best) best = left;
      }
      for (int i = 0; i < 4; i++) {
        if (i == 3) continue;
        int32_t w = i == 2 ? h.gapA : h.gapB;
        uint32_t t = at<uint32_t>(s, 0x1f0 + 4 * i);
        if (at<uint16_t>(s, 0x1e6 + 2 * i) == at<uint16_t>(s, 0x1de + 2 * i) || !t) continue;
        int32_t left = (int32_t)(t - now + w);
        if (left < 1) {
          uint16_t tab = at<uint16_t>(s, 0x1de + 2 * i);
          pkt(h, s, at<uint8_t>(me, 0x212), 1, (uint8_t)i, tab, tab);
          at<uint16_t>(s, 0x1e6 + 2 * i) = tab; at<uint32_t>(s, 0x1f0 + 4 * i) = 0;
        } else if ((uint32_t)left <= best) best = left;
      }
    }
    if (!nextSess) return best;
    s = h.node(nextSess);
  }
}

// 1.13c D2Win+0x12FE0 (size 301): SetUnicodeTextFont(fontIdx) -> previous index.
#include <cstdint>
struct D2Win12FE0Slot { void* font; int32_t a, b, c; uint32_t lastUse; };  // 20 bytes, table @0x6F901510 x14
struct D2Win12FE0Env {
  uint32_t (*tickCount)();                       // GetTickCount
  D2Win12FE0Slot* slots;                         // DAT_6f901510[14]
  int32_t* curIdx;                               // DAT_6f8ffd34
  uint32_t* lastSweep;                           // DAT_6f9a9d0c
  D2Win12FE0Slot** curSlot;                      // DAT_6f9014f0
  const uint8_t* flag;                           // DAT_6f9a9d12
  void (**getter)();                             // DAT_6f901508
  void (*getterA)(); void (*getterB)();          // LAB_6f8f2150 / LAB_6f8f21d0
  void (*prep)(int32_t);                         // 0x12350(0x4f)
  void* (*load)(void* buf, int32_t);             // 0x0A7A0(local_50[80], 0)
  void (*post)();                                // 0x12B40
  void (*release)(void*);                        // Ordinal_10065
  void (*notify)(int32_t, int32_t);              // Ordinal_10043(0x5c,0)
};
// 1.13c D2Win+0x12FE0
int32_t d2win_sub_12fe0(D2Win12FE0Env& e, int32_t idx) {
  int32_t prev = *e.curIdx;
  uint32_t now = e.tickCount() >> 10;
  if (prev != idx) {
    *e.curIdx = idx;
    if (!e.slots[idx].font) {
      char buf[80];
      e.prep(0x4f);
      e.slots[idx].font = e.load(buf, 0);
      e.post();
      e.slots[idx].lastUse = now;
    }
    if (now - *e.lastSweep > 600) {
      *e.lastSweep = now;
      for (int32_t i = 0; i < 14; ++i) {
        D2Win12FE0Slot& s = e.slots[i];
        if (i != idx && now - s.lastUse > 0xf0 && s.font) {
          e.release(s.font);
          e.notify(0x5c, 0);
          s.font = nullptr;
        }
      }
    }
    *e.curSlot = &e.slots[idx];
    if (*e.flag) *e.getter = (idx != 0xd) ? e.getterB : e.getterA;  // decomp: getter=A, then B unless idx==0xd
    return prev;
  }
  e.slots[idx].lastUse = now;
  return prev;
}

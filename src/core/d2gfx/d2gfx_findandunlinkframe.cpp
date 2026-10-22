// 1.13c D2Gfx+0xBEF4 _FindAndUnlinkFrame: unlink a FrameInfo from the per-thread chain headed by
// ptd->_curexception. Frame layout: next link (32-bit guest pointer) at +4. `head` is &ptd->_curexception;
// `at(p)` maps a guest pointer to the host word holding its next link. Returns 0 ok, 1 = _inconsistency().
#include <cstdint>
int d2gfx_FindAndUnlinkFrame(uint32_t* head, uint32_t frame, uint32_t* (*nextOf)(uint32_t)) {
  if (frame == *head) { *head = *nextOf(frame); return 0; }
  uint32_t cur = *head;
  for (;;) {
    uint32_t* n = nextOf(cur);
    if (*n == 0) return 1;
    if (*n == frame) { *n = *nextOf(frame); return 0; }
    cur = *n;
  }
}

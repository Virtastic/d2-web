// 1.13c D2Win+0x12200 (size 127, caller 0x18060): release the cached cel files and Fog blocks.
// Table at 0x6F901510: 14 entries of 5 dwords; [0]=cel file (D2CMP #10065 then Fog #10043(0x5c,0)), [1]=Fog block
// (#10043(0x135,0)). Then the tail cel file at 0x6F901628. Seams: D2CMP #10065 and Fog #10043 (0x768A/0x777A thunks).
#include <cstdint>
struct D2Win12200Env {
  uint32_t (*tbl)[5];                       // 0x6F901510, 14 rows
  uint32_t* tail;                           // 0x6F901628
  void (*cmpFree)(uint32_t cel);            // D2CMP #10065
  void (*fogFree)(uint32_t line, uint32_t); // Fog #10043
};
// 1.13c D2Win+0x12200
void d2win_sub_12200(D2Win12200Env& e) {
  for (int i = 0; i < 14; i++) {
    uint32_t* r = e.tbl[i];
    if (r[0]) { e.cmpFree(r[0]); e.fogFree(0x5c, 0); r[0] = 0; }
    if (r[1]) { e.fogFree(0x135, 0); r[1] = 0; }
  }
  uint32_t t = *e.tail;
  e.cmpFree(t);
  if (t) e.fogFree(0x5c, 0);
  *e.tail = 0;
}

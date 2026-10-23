// D2Client+0x4F770: pick one of two float globals by a unit-state test. Seam: Ordinals 10331/10826/10215/10499 passed in; returns the choice, not the float.
#include <cstdint>
namespace d2client_4f770 {
typedef uint32_t (*Ord1)(uint32_t a); // D2Common/Game ordinal taking one arg
// 1.13c D2Client+0x4F770 (g_cbf78=DAT_6fbcbf78, player=DAT_6fbcbbfc). Returns 1 -> read DAT_6fbc38f4, 0 -> read DAT_6fb87578.
int sub_4F770(Ord1 o10331,Ord1 o10826,Ord1 o10215,Ord1 o10499,uint32_t g_cbf78,uint32_t player){
 if(g_cbf78==0&&player!=0){
  uint32_t u=o10826(o10331(player));
  uint8_t c=(uint8_t)o10215(u);
  o10499(u);
  if(c!=0) return 1;
 }
 return 0;
}
}

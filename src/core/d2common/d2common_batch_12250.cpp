// D2Common 1.13c implementation batch. Inputs carried in x86 registers in the cached
// decompiles are explicit parameters here; unresolved retail callees are callback seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
int32_t s(uint32_t a, int o) { return static_cast<int32_t>(w(a,o)); }
}

extern "C" {
// 1.13c D2Common+0x12250: a radius box must fit strictly inside the room box.
EMSCRIPTEN_KEEPALIVE uint32_t d2_batch_d2common_12250(uint32_t room, int32_t x, int32_t y, int32_t radius, void (*roomBox)(uint32_t,int32_t*)) {
 if (!roomBox) return 1;
 int32_t b[4]{}; roomBox(room,b);
 return !(b[0] < x-radius && b[1] < y-radius && x+radius < b[0]+b[2] && y+radius < b[1]+b[3]);
}
// 1.13c D2Common+0x4E480: select a skill node by id, passive flag, and highest level.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_4E480_FindSkillNode(uint32_t unit, int32_t skillId) {
 if (!unit || !w(unit,0xA8)) return 0;
 uint32_t best=0;
 for (uint32_t n=w(w(unit,0xA8),4); n; n=w(n,4)) {
  const uint32_t row=w(n,0);
  if (row && static_cast<int16_t>(w(row,0))==skillId && !w(n,0x3C) &&
      (!best || s(n,0x34)==-1 || (s(best,0x34)!=-1 && s(best,0x28)<s(n,0x28)))) best=n;
 }
 return best;
}
// 1.13c D2Common+0x32CA0: write owner identity and SOURCEUNIT stats.
// countGate: sgptDataTables+0xC4 > 0x62. Retail runs the bit block (toggle) and UNITROOM_RefreshUnit (refresh) only then,
// and only when the unit has a stat list ex; both happen before the overlay-list lookup.
void D2Common_32CA0_StoreOwnerInfo(uint32_t unit, int32_t type, int32_t id, bool countGate,
 void (*toggle)(uint32_t,int32_t,int32_t), void (*refresh)(uint32_t), uint32_t (*find)(uint32_t,int32_t),
 uint32_t (*alloc)(uint32_t,int32_t,int32_t,int32_t,int32_t), void (*post)(uint32_t,uint32_t,int32_t),
 void (*set)(uint32_t,int32_t,int32_t)) {
 if (!unit) return;
 w(unit,0x94)=static_cast<uint32_t>(type); w(unit,0x98)=static_cast<uint32_t>(id);
 if (w(unit,0x5C)) {
  if (countGate) { if (toggle) toggle(unit,0x62,1); if (refresh) refresh(unit); }
  // retail walks the overlay lists only when the list ex is extended (flags bit 31); otherwise it allocates directly
  uint32_t list=(find && static_cast<int32_t>(w(w(unit,0x5C),0x10))<0) ? find(unit,0x62) : 0;
  if (!list && alloc) { list=alloc(w(unit,8),0,0,s(unit,0),s(unit,4)); if (!list) return;
   w(list,0x14)=0x62; if (post) post(unit,list,1); }
  if (set) { set(list,0x161,type); set(list,0x162,id); }
 }
 w(unit,0xC8)|=0x400;
}
// 0x24F00 is ITEMS_IsRepairableStack in d2moo/src/Items/Items.cpp.
// 1.13c D2Common+0x4F870: stat gate for category 1,2,4,5,12.
int32_t D2Common_4F870_StatGate(uint32_t unit, int32_t category, int32_t ratio,
 uint32_t tableCount, uint32_t tableRow, uint32_t (*hasStat)(uint32_t,int32_t),
 uint32_t (*hasTableStat)(uint32_t), int32_t (*applyRatio)(int32_t,int32_t)) {
 // retail: EAX = stat value from the getter (0x38B70 / 0x38A80), ESI = base; the result is
 // FUN_6fd511e0(100, base) with EAX as multiplier, i.e. base * statValue / 100 (applyRatio(base, value)).
 uint32_t value=0;
 if (category==4 || category==12) {
  if (!unit || !w(unit,0x5C) || tableCount<0x14C || tableRow+0x1A2EC==0 || !hasTableStat) return 0;
  value=hasTableStat(tableRow+0x1A2EC);
 } else {
  int32_t stat=category==1 ? 0x149 : category==2 ? 0x14A : category==5 ? 0x14C : -1;
  if (stat<0 || !hasStat) return 0;
  value=hasStat(unit,stat);
 }
 if (!value) return 0;
 return applyRatio ? applyRatio(ratio,(int32_t)value) : 0;
}
// 1.13c D2Common+0x3CF30: stable insertion sort of 12-byte records; retail calls compare(ecx=earlier record, edx=moving record) and shifts while it is positive.
EMSCRIPTEN_KEEPALIVE void d2c_3cf30_legacy(uint32_t records, uint32_t count,
 int32_t (*compare)(uintptr_t,uintptr_t)) {
 if (!records || !compare) return;
 uint8_t saved[12];
 for (uint32_t i=1;i<count;i++) {
  std::memcpy(saved,p(records+i*12),12);
  uint32_t j=i;
  while (j && compare(records+(j-1)*12,reinterpret_cast<uintptr_t>(saved))>0) {
   std::memmove(p(records+j*12),p(records+(j-1)*12),12); --j;
  }
  std::memcpy(p(records+j*12),saved,12);
 }
}
// 1.13c D2Common+0x241A0: item data signature and unit type gate.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_241A0_ItemDataGate(uint32_t unit) {
 if (!unit) return 0;
 uint32_t data=w(unit,0x60); return data && w(data,0)==0x1020304 && w(data,0xC) && w(w(data,0xC),0)==4;
}
}

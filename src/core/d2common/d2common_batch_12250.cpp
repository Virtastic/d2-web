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
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_12250(uint32_t room, int32_t x, int32_t y, int32_t radius, void (*roomBox)(uint32_t,int32_t*)) {
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
// 1.13c D2Common+0x41DD0: uppercase the key, sum its bytes, scan that bucket.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_41dd0(uint32_t key, uint32_t buckets, uint32_t (*equal)(uint32_t,uint32_t)) {
 if (!key || !buckets || !equal) return 0;
 uint8_t hash=0;
 for (uint8_t* c=p(key); *c; ++c) { if (*c>='a' && *c<='z') *c-=32; hash=static_cast<uint8_t>(hash+*c); }
 uint32_t bucket=w(buckets,4+hash*4); if (!bucket) return 0;
 int32_t count=s(bucket,0);
 for (uint32_t e=bucket+4; count-->0; e+=0xA0) if (equal(key,e)) return e;
 return 0;
}
// 1.13c D2Common+0x32CA0: write owner identity and SOURCEUNIT stats.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_32ca0(uint32_t unit, int32_t type, int32_t id,
 void (*toggle)(uint32_t,int32_t,int32_t), uint32_t (*find)(uint32_t,int32_t),
 uint32_t (*alloc)(uint32_t,int32_t,int32_t,int32_t,int32_t), void (*post)(uint32_t,uint32_t,int32_t),
 void (*set)(uint32_t,int32_t,int32_t)) {
 if (!unit) return;
 w(unit,0x94)=static_cast<uint32_t>(type); w(unit,0x98)=static_cast<uint32_t>(id);
 if (w(unit,0x5C)) {
  if (toggle) toggle(unit,0x62,1);
  uint32_t list=find ? find(unit,0x62) : 0;
  if (!list && alloc) { list=alloc(w(unit,8),0,0,s(unit,0),s(unit,4)); if (!list) return;
   w(list,0x14)=0x62; if (post) post(unit,list,1); }
  if (set) { set(list,0x161,type); set(list,0x162,id); }
 }
 w(unit,0xC8)|=0x400;
}
// 1.13c D2Common+0x24F00: item record gate, durability, type, and ethereal flag.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_24f00(uint32_t item, int32_t rowCount, uint32_t rows,
 int32_t (*itemRow)(uint32_t), uint32_t (*hasDurability)(uint32_t),
 uint32_t (*validType)(uint32_t), uint32_t (*isEthereal)(uint32_t)) {
 if (!item) return 0;
 int32_t row=s(item,0)==4 && itemRow ? itemRow(item) : 0;
 if (row<0 || row>=rowCount || !rows || !p(rows+row*0xE4)[8]) return 0;
 return hasDurability && hasDurability(item) && validType && validType(item) &&
        (!isEthereal || !isEthereal(item));
}
// 1.13c D2Common+0x455B0: resolve a unit and return its item data field +0x1D0.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_455b0(uint32_t first, uint32_t second, uint32_t third,
 uint32_t (*resolve)(uint32_t,uint32_t,uint32_t*,uint32_t*)) {
 if (!resolve) return 0;
 uint32_t a=first,b=third; uint32_t unit=resolve(third,second,&a,&b);
 if (!unit || !w(unit,0x10) || !w(w(unit,0x10),0x58)) return 0;
 return w(w(w(unit,0x10),0x58),0x1D0);
}
// 1.13c D2Common+0x2F100: update sequence mode, frame and event; report a mode change.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_2f100(uint32_t unit, uint32_t target,
 void (*compute)(uint32_t,uint32_t,uint32_t,uint32_t*,uint32_t*,int32_t*,uint32_t*)) {
 if (!unit || !compute) return 0;
 uint32_t old=w(unit,0x40),mode=0,frame=0,event=0; int32_t dir=0;
 compute(w(unit,0x30),w(unit,0x38),target,&mode,&frame,&dir,&event);
 w(unit,0x44)=frame<<8; w(unit,0x40)=mode; p(unit)[0x4E]=static_cast<uint8_t>(event);
 return mode!=old;
}
// 1.13c D2Common+0x4F870: stat gate for category 1,2,4,5,12.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4f870(uint32_t unit, int32_t category, int32_t ratio,
 uint32_t tableCount, uint32_t tableRow, uint32_t (*hasStat)(uint32_t,int32_t),
 uint32_t (*hasTableStat)(uint32_t), int32_t (*applyRatio)(int32_t,int32_t)) {
 if (category==4 || category==12) {
  if (!unit || !w(unit,0x5C) || tableCount<0x14C || !tableRow || !hasTableStat ||
      !hasTableStat(tableRow+0x1A2EC)) return 0;
 } else {
  int32_t stat=category==1 ? 0x149 : category==2 ? 0x14A : category==5 ? 0x14C : -1;
  if (stat<0 || !hasStat || !hasStat(unit,stat)) return 0;
 }
 return applyRatio ? applyRatio(100,ratio) : 0;
}
// 1.13c D2Common+0x3B3A0: validate the room's counted unit-id array.
extern "C" uint32_t D2Common_3B3A0_ValidateUnitIds(uint32_t room) {
 if (!room) return 0;
 uint32_t count=w(room,0x78), a=w(room,0x48);
 if (count && !a) return 0;
 for (uint32_t i=0;i<count;i++) if (!w(a,i*4)) return 0;
 return 1;
}
// 1.13c D2Common+0x3CF30: stable insertion sort of 12-byte records; compare 6-byte keys.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3cf30(uint32_t records, uint32_t count,
 int32_t (*compare)(uintptr_t,uintptr_t)) {
 if (!records || !compare) return;
 uint8_t saved[12];
 for (uint32_t i=1;i<count;i++) {
  std::memcpy(saved,p(records+i*12),12);
  uint32_t j=i;
  while (j && compare(reinterpret_cast<uintptr_t>(saved),records+(j-1)*12)>0) {
   std::memmove(p(records+j*12),p(records+(j-1)*12),12); --j;
  }
  std::memcpy(p(records+j*12),saved,12);
 }
}
// 1.13c D2Common+0x7BA00: reject inactive/short text chains and flagged links.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7ba00(uint32_t text,
 uint32_t (*resolve)(uint32_t,uint32_t*,uint32_t*)) {
 if (!text || (w(text,0x28)&0x400000) || !w(text,0x58)) return 0;
 uint32_t chain=w(w(text,0x58),0x1B4); if (!chain || (p(chain)[0x8C]&1) || p(text)[0x44]<2) return 0;
 uint32_t owner=0,kind=0; if (resolve && resolve(text,&owner,&kind) && owner && kind==0x78)
  for (uint32_t n=w(owner,0x10);n;n=w(n,0x24)) if (p(n)[0x44]<2) return 0;
 return 1;
}
// 1.13c D2Common+0x241A0: item data signature and unit type gate.
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_241A0_ItemDataGate(uint32_t unit) {
 if (!unit) return 0;
 uint32_t data=w(unit,0x60); return data && w(data,0)==0x1020304 && w(data,0xC) && w(w(data,0xC),0)==4;
}
}

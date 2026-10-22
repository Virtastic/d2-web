// D2Common 1.13c implementation batch. Retail x86 addresses in table-backed records are
// explicit 32-bit values; unresolved DLL calls and image globals are supplied by the caller.
#include <cstdint>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t u32(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b+o); }
int32_t i32(const uint8_t* b, int o) { return *reinterpret_cast<const int32_t*>(b+o); }
uint16_t u16(const uint8_t* b, int o) { return *reinterpret_cast<const uint16_t*>(b+o); }
int16_t i16(const uint8_t* b, int o) { return *reinterpret_cast<const int16_t*>(b+o); }
int32_t sar(int32_t a, uint8_t n) { return a >> (n & 31); }
}

struct D2Common39380Env {
 uint32_t tables;
 int32_t (*total_stat)(uint32_t);
 int32_t (*base_stat)(uint32_t);
};
// 1.13c D2Common+0x39380: difference of a stat's total and base values.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_39380(uint32_t gate, int32_t stat,
 const D2Common39380Env* e) {
 if (!gate || !e || !e->tables || stat < 0) return 0;
 auto t=p(e->tables); if (stat >= i32(t,0xbd4)) return 0;
 uint32_t rec=u32(t,0xbcc)+static_cast<uint32_t>(stat)*0x144;
 if (!rec) return 0;
 return (e->total_stat ? e->total_stat(rec) : 0) - (e->base_stat ? e->base_stat(rec) : 0);
}

struct D2Common39530Env {
 uint32_t tables;
 int32_t (*initial)(uint32_t,uint32_t); // +0x38CD0
 int32_t (*monster_op)(uint32_t,uint32_t); // +0x38C20
 int32_t (*unit_stat)(uint32_t,uint32_t); // +0x38B70
 uint32_t (*op_record)(uint16_t); // +0x17A0
 uint32_t (*player_record)(); // +0x12D0
 int32_t (*stat_index)(uint16_t); // +0x382B0
 uint32_t (*seed)(uint32_t,int32_t*); // +0x37470
 int32_t (*roll)(uint32_t,uint32_t,int32_t,uint32_t*,int32_t*,int32_t*); // +0x46330
 int32_t (*ratio)(int32_t,int32_t); // DATATBLS_ApplyRatio(100,value)
};
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_39530(uint32_t, uint32_t, const D2Common39530Env*);
namespace {
int32_t stat_value(uint32_t list, uint16_t id, const D2Common39530Env* e) {
 if (!list || !e->stat_index) return 0;
 int32_t n=e->stat_index(id); if (n<0) return 0;
 uint32_t entry=list+static_cast<uint32_t>(n)*8;
 return entry ? i32(p(entry),4) : 0;
}
int32_t ratio(int32_t v,const D2Common39530Env* e) { return e->ratio ? e->ratio(100,v) : v/100; }
}
// 1.13c D2Common+0x39530: apply up to sixteen itemstatcost op records to a base stat.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_39530(uint32_t ctx, uint32_t packed,
 const D2Common39530Env* e) {
 if (!ctx || !e || !e->tables) return 0;
 auto t=p(e->tables); uint32_t id=(packed>>16)&0xffff;
 if (id>=u32(t,0xbd4)) return 0;
 uint32_t addr=u32(t,0xbcc)+id*0x144; if (!addr) return 0;
 auto rec=p(addr); auto c=p(ctx);
 int32_t result=e->initial ? e->initial(ctx,addr) : 0;
 if (!rec[0x52]) return result;
 int32_t base=result;
 for (int j=0;j<16;j++) {
  auto op=rec+0xe0+j*6; uint8_t type=op[2]; if (!type) break;
  uint16_t source=u16(op,0); uint32_t next=static_cast<uint32_t>(source)<<16;
  uint32_t actor=u32(c,0x44); int32_t amount=0;
  switch(type) {
   case 1: case 11: case 13: {
    if (!actor) break;
    auto a=p(actor); bool allowed=type==1 || (type==13 ? i32(a,0)==4 : i32(a,0)<=1);
    if (!allowed) break;
    int32_t opbase=base;
    if ((type==1 || type==13) && i32(a,0)==4)
     opbase=e->monster_op ? e->monster_op(actor,id) : 0;
    if (opbase && d2_fn_d2common_39530(ctx,next,e)) amount=ratio(opbase,e);
    break;
   }
   case 2: case 3: case 4: case 5: {
    uint32_t unit= type<=3 ? ctx : u32(c,4);
    if (type<=3 ? (i32(c,8)>1) : (!unit || i32(p(unit),0)>1)) break;
    if (i16(op,0)==-1 || !e->op_record) break;
    uint32_t meta=e->op_record(source); if (!meta) break;
    int32_t value=0;
    if (type<=3) value=stat_value(u32(c,i32(c,0x10)>=0?0x24:0x48),source,e);
    else value=e->unit_stat ? e->unit_stat(unit,source) : 0;
    value=sar(value,p(meta)[0x18]);
    if (value<=0) break;
    int32_t derived=d2_fn_d2common_39530(ctx,next,e);
    if ((type&1)==0) amount=sar(static_cast<int32_t>(static_cast<uint32_t>(derived)*value),op[3]);
    else if (derived) amount=ratio(base,e);
    break;
   }
   case 6: case 7: {
    if (!actor) break;
    uint32_t state=u32(p(actor),0x1c); if (!state) break;
    int32_t derived=d2_fn_d2common_39530(ctx,next,e); if (!derived) break;
    int32_t aux=0; uint32_t seed=e->seed ? e->seed(state,&aux) : 0;
    int32_t rolled=e->roll ? e->roll(static_cast<uint32_t>(derived),seed,aux,nullptr,nullptr,nullptr) : 0;
    amount=(type==7 ? ratio(result,e) : rolled);
    break;
   }
   case 8: case 9: {
    if (!actor || i32(p(actor),0)!=0 || !e->player_record) break;
    uint32_t player=e->player_record(); if (!player) break;
    int32_t diff=d2_fn_d2common_39530(ctx,next,e)-stat_value(u32(c,0x24),source,e);
    uint8_t mult= type==8 ? p(player)[0x48] : p(player)[id==11?0x47:0x46];
    amount=static_cast<int32_t>(static_cast<uint32_t>(mult)*diff*0x40u);
    break;
   }
  }
  result=static_cast<int32_t>(static_cast<uint32_t>(result)+static_cast<uint32_t>(amount));
 }
 return result;
}

struct D2Common1FB50Env { void (*prepare)(); const int32_t* pairs; };
// 1.13c D2Common+0x1FB50: first equal composite id in slots 1..254.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_1fb50(int32_t a,int32_t b,const D2Common1FB50Env* e) {
 if (!e) return 0; if (e->prepare) e->prepare(); if (!e->pairs) return 0;
 for (uint32_t i=1;i<=0xfe;i++) { int32_t v=e->pairs[i*2]; if (v==a || v==b) return i; }
 return 0;
}

struct D2Common21300Env {
 uint32_t item_count, item_table;
 uint32_t (*component)(uint32_t,int32_t); // +0x20830
 uint8_t (*slot)(uint32_t); // +0x231E0
 uint32_t (*composite)(int32_t,int32_t); // +0x1FB50
 void (*update)(uint32_t,uint32_t,uint32_t); // +0x20E80
 int32_t (*color)(uint32_t,uint32_t,uint8_t*,int32_t); // +0x251C0
};
// 1.13c D2Common+0x21300: map one item component into its 16-byte color tables.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_21300(uint32_t unit,uint32_t out_a,uint32_t out_b,
 uint32_t item,const D2Common21300Env* e) {
 if (!unit || !out_a || !out_b || !e || !item) return;
 uint32_t inv=u32(p(unit),0x60); if (!inv) return;
 uint32_t c5=e->component?e->component(inv,5):0, c6=e->component?e->component(inv,6):0;
 uint32_t id=u32(p(item),4); if (id>=e->item_count || !e->item_table) return;
 uint32_t record=e->item_table+id*0x1a8; if (!record) return;
 uint8_t slot=e->slot?e->slot(item):0xff;
 uint32_t composite=e->composite?e->composite(i32(p(item),4),i32(p(unit),4)):0;
 if (!composite) { if (slot<16) { p(out_a)[slot]=0xff; p(out_b)[slot]=0xff; } return; }
 if (item==c5) slot=5; else if (item==c6) slot=6; else if (slot>=16) return;
 p(out_a)[slot]=static_cast<uint8_t>(composite);
 if (e->update) e->update(unit,out_a,out_b);
 uint8_t* dest=p(out_b)+slot;
 if (!e->color || !e->color(unit,item,dest,0)) *dest=0xff; else ++*dest;
}

struct D2Common21D70Env { int32_t (*place)(uint32_t,uint32_t,int32_t,int32_t,int32_t,int32_t,int32_t); };
// 1.13c D2Common+0x21D70: place an item in belt slot 0..15.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_21d70(uint32_t inventory,uint32_t item,
 int32_t slot,const D2Common21D70Env* e) {
 if (!inventory || !item || i32(p(item),0)!=4 || slot<0 || slot>=16 || !e || !e->place) return 0;
 return e->place(inventory,item,slot,0,1,0,0);
}

// 1.13c D2Common+0x38160: lower-bound insertion index, -1 for an existing key.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_38160(uint32_t list,int32_t key) {
 if (!list) return 0;
 auto l=p(list); int32_t lo=0, hi=i16(l,4); uint32_t data=u32(l,0);
 while (lo<hi) { int32_t mid=lo+(hi-lo)/2; int32_t v=i32(p(data),mid*4);
  if (v<key) lo=mid+1; else if (v==key) return -1; else hi=mid; }
 return lo;
}

struct D2Common4FDC0Env { uint32_t tables; int32_t (*total_stat)(uint32_t); uint32_t (*state)(int32_t); };
// 1.13c D2Common+0x4FDC0: gate a player by stat nine and state twelve.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4fdc0(uint32_t unit,uint32_t record,
 const D2Common4FDC0Env* e) {
 if (!unit || i32(p(unit),0)!=0 || !record || !u32(p(record),0)) return 1;
 int16_t threshold=i16(p(u32(p(record),0)),0x184); if (threshold<=0) return 1;
 int32_t value=0;
 if (u32(p(unit),0x5c) && e && e->tables) {
  auto t=p(e->tables); if (i32(t,0xbd4)>=9) {
   uint32_t stat=u32(t,0xbcc)+0xa20; if (stat && e->total_stat) value=e->total_stat(stat);
  }
 }
 if (value<static_cast<int32_t>(threshold)*0x100 &&
  (!e || !e->tables || i32(p(e->tables),0xc4)<13 || !e->state || !e->state(12))) return 0;
 return 1;
}

struct D2Common20E80Env {
 int32_t (*weapon_class)(uint32_t,uint32_t,uint32_t*,int32_t,int32_t); // +0x441E0
 uint8_t (*slot)(uint32_t); // +0x231E0
 uint32_t (*component)(uint32_t,int32_t); // +0x20830
 uint32_t (*items_txt)(uint32_t); // +0x719A0
 uint32_t (*item_by_code)(uint32_t); // +0x71960
 uint32_t (*composite)(uint32_t,uint32_t,uint32_t); // +0x1FB50, args unknown: unit,item,record
};
// 1.13c D2Common+0x20E80: bow/crossbow items set composite byte 5 or 6 of the weapon-held tables.
// Null unit/item is fatal in retail (_exit); here it is a no-op.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_20e80(uint32_t unit,uint32_t out_a,uint32_t out_b,
 uint32_t item,const D2Common20E80Env* e) {
 if (!unit || !item || !e) return;
 uint32_t data=u32(p(item),0x14);
 if (i32(p(item),0)!=4 || !data) return;
 uint8_t t=p(data)[0x44]; if (t!=4 && t!=5) return;
 uint32_t inv=u32(p(unit),0x60), tmp=out_a;
 int32_t code=e->weapon_class(unit,inv,&tmp,-1,1);
 uint8_t slot=e->slot(item);
 if (code==0x20776278) {
  if ((slot!=5 && slot!=6) || !inv) return;
  uint32_t c=e->component(inv,5); if (!c) return;
  uint32_t rec=e->items_txt(u32(p(c),4)); if (!rec) return; // retail: fatal
  int32_t n=static_cast<int32_t>(e->composite(unit,item,rec));
  if (n>0) p(out_a)[6]=static_cast<uint8_t>(n); else p(out_a)[6]=p(out_b)[6]=0xff;
 } else if (code==0x20776f62 && slot==6 && e->item_by_code(0x2074696c)) {
  uint32_t n=e->composite(unit,item,0);
  if (n==0) p(out_a)[5]=p(out_b)[5]=0xff; else p(out_a)[5]=static_cast<uint8_t>(n);
 }
}

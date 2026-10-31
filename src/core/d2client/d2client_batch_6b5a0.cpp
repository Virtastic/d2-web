// Implementation batch (RE-DONE lane, no vector yet). Derived from cached 1.13c decompile;
// register-passed arguments are explicit parameters, Fog/D2 ordinals and globals come through Host.
#include <cstdint>
#include <cstring>
namespace d2client_batch_6b5a0 {
using U=uint32_t; using I=int32_t;

struct Host {
 // Fog/D2Win ordinals and globals used by the group; unset members are never called by a caller that does not need them.
 I (*ord)(U ordinal, I a, I b, I c, I d);
 U (*tick)();                       // GetTickCount
 I (*lookup)(U rva, I a, I b, I c); // FUN_ callees by RVA (0x6AC60, 0x6B300, 0x69180, 0x689F0, 0x1AF0, 0x1B50, 0x6AEE0, 0x6E2C0)
 uint8_t* gfxA;  // DAT_6fbc4068 (stride 0x28 dwords per cell group)
 uint8_t* gfxB;  // DAT_6fbc40b8 (= gfxA+0x50)
 uint8_t* tbl;   // DAT_6fbc54e4
 U count0;       // DAT_6fb928ac
 U count1;       // DAT_6fb928ec
 U* list1;       // DAT_6fb928b0
 const uint8_t* bitMasks; // gdwBitMasks, stride 4 bytes
 I clipR, clipB; // DAT_6fb8bc48 / DAT_6fb8bc4c
 const uint8_t* dataTables; // *sgptDataTables (D2Client+0xCE474 import slot); +0xA80 = entry count
 I (*unitInfo)(U unit, I* type, I* id, I* sub); // D2Common#10350
 U flagB9B0;     // DAT_6fbcb9b0
 const uint8_t* (*dtTable)(U off); // the table pointer stored at *sgptDataTables+off (0xA78 MonStats, 0xA90 MonStats2)
 void (*cell6C020)(U cell,U unit,U arg); // 0x6C020 (esi=cell, edi=unit, stack=arg)
};

static U& dw(uint8_t* base,U idx){ return *reinterpret_cast<U*>(base+idx*4); }

// 1.13c D2Client+0x6D220 (eax=from, ecx=to, esi=value*)
void sub_6D220(Host& h,I from,I to,U* v){
 if(from==8){ if(to==4){ U r=U(h.ord(10057,I((*v&0x1fffffff)>>3),8,4,0)); *v=(r&0x3ffffff)<<3; } }
 else if(from==0x10 && to==8){ U r=U(h.ord(10057,I((*v&0xfffffff)>>2),0x10,8,0)); *v=(r&0x3ffffff)<<2; }
}

// 1.13c D2Client+0x6D540 (eax=dst[4], edx=src) : left-justified 3-char copy, space padded to 4
void sub_6D540(char* dst,const char* src){
 std::memcpy(dst,"    ",4);
 for(int i=0;i<3&&src[i];++i) dst[i]=src[i];
}

// 1.13c D2Client+0x6D280 : on-screen test. w=ord10000, h2=ord10069, a=ord10002, d=ord10060
bool sub_6D280(const Host& h,I w,I h2,I a,I d,I ebp,I ebx,I retaddr){
 I x=a+ebp, y=d;
 if(ebx){ x-=w; y+=w; }
 if(x<=h.clipR && -1<y+x && (h2+retaddr)-w<=h.clipB) return -1<h2+retaddr+w;
 return false;
}

// 1.13c D2Client+0x6D590 : null-guarded pair of ordinal calls (10175, 10350)
void sub_6D590(Host& h,I eax){ if(eax){ h.ord(10175,0,0,0,0); h.ord(10350,0,0,0,0); } }

// 1.13c D2Client+0x6BF30 (esi=group index ptr/value, edi=cell)
void sub_6BF30(Host& h,U grp,U cell){
 h.ord(10175,0,0,0,0); h.ord(10350,0,0,0,0);
 U i=cell+grp*0x28;
 if(dw(h.gfxA+0x4068-0x4068,i)==0){
  if(dw(h.gfxB,i)==0){
   if(h.lookup(0x6AC60,I(cell),0,0)==1) dw(h.gfxB,i)=1;
   else { dw(h.gfxB,i)=0; dw(h.gfxA,i)=0; }
  } else if(h.ord(10055,0,0,0,0)==0) dw(h.gfxA,i)=1;
 }
}

// 1.13c D2Client+0x6C3B0
void sub_6C3B0(Host& h){ for(U i=0;i<h.count0;++i) sub_6BF30(h,i,0); }

// 1.13c D2Client+0x6C020 (esi=cell, edi=group, groupCount=*(dataTables+0xa80))
void sub_6C020(Host& h,U cell,I grp,I groupCount){
 h.ord(10175,0,0,0,0); h.ord(10350,0,0,0,0);
 if(grp<groupCount){
  U i=cell+U(grp)*0x28;
  if(dw(h.tbl,i)==0){
   if(dw(h.tbl,i+0x14)==0){
    if(h.lookup(0x6AC60,I(cell),0,0)==1) dw(h.tbl,i+0x14)=1;
    else { dw(h.tbl,i+0x14)=0; dw(h.tbl,i)=0; }
   } else if(h.ord(10055,0,0,0,0)==0) dw(h.tbl,i)=1;
  }
 }
}

// 1.13c D2Client+0x6C3F0 (eax = unit, stack = arg forwarded to every 6C020; ret 4). Class = 0x1B50(unit);
// MonStats row (dataTables+0xA78, stride 0x1A8, count +0xA80) -> short +0x18 -> MonStats2 row (+0xA90, stride 0x134,
// count +0xA98); for each entry b of the static list (count DAT 0x6FB928EC), if gdwBitMasks[b&7] & row[0xF0+(b>>3)]
// then 6C020(esi=b, edi=unit, stack=arg). The count is re-read after every call, as retail does.
void sub_6C3F0(Host& h,U unit,U arg){
 I cls=h.lookup(0x1B50,I(unit),0,0);
 if(cls<0) return;
 if(cls>=*reinterpret_cast<const I*>(h.dataTables+0xa80)) return;
 I m2=*reinterpret_cast<const int16_t*>(h.dtTable(0xa78)+U(cls)*0x1a8+0x18);
 if(m2<0) return;
 if(m2>=*reinterpret_cast<const I*>(h.dataTables+0xa98)) return;
 const uint8_t* row=h.dtTable(0xa90)+U(m2)*0x134;
 if(!row) return;
 for(U i=0;i<h.count1;++i){
  U b=h.list1[i];
  if(h.bitMasks[(b&7)*4] & row[(b>>3)+0xf0]) h.cell6C020(b,unit,arg);
 }
}

// 1.13c D2Client+0x6CB30 (esi = unit, may be null). Retail: type = unit?unit[0]:6, id = unit?unit[1]:-1, sub = D2Common#10175(unit);
// D2Common#10350(unit,&type,&id,&sub,...) rewrites all three; for type==1 (monster) and id < *(sgptDataTables+0xA80) both
// parallel cells tbl[(id*0x28+sub)] and +0x50 are cleared; then edi = D2Common#10331(unit) && !D2gfx#10043() && !DAT_6fbcb9b0, and
// 0x6C3F0(eax=unit, stack=edi) runs. The entry count is read from the table here, as retail does.
void sub_6CB30(Host& h,const U* unit,U unitAddr){
 I type = unit ? I(unit[0]) : 6;
 I id   = unit ? I(unit[1]) : -1;
 I sub  = h.ord(10175,I(unitAddr),0,0,0);
 h.unitInfo(unitAddr,&type,&id,&sub);
 if(type!=1) return;
 if(id >= *reinterpret_cast<const I*>(h.dataTables+0xa80)) return;
 dw(h.tbl,U(id)*0x28+U(sub))=0;
 dw(h.tbl,U(id)*0x28+U(sub)+0x14)=0;
 U edi=1;
 if(!h.ord(10331,I(unitAddr),0,0,0)) edi=0;
 else if(h.ord(10043,0,0,0,0) || h.flagB9B0) edi=0;
 sub_6C3F0(h,unitAddr,edi);
}

// 1.13c D2Client+0x6B5A0 : size/alloc-tier select (ord 11088==4, ord 10973 value), returns 0/1
I sub_6B5A0(Host& h,const U* obj,I param2){
 if(!obj||*obj!=4) return 0;
 I tier=0;
 if(h.ord(11088,0,0,0,0)==4){
  U n=U(h.ord(10973,0,0,0,0));
  if(n>99) tier = n<500 ? 1 : (n<5000 ? 2 : 3);
 }
 (void)tier; (void)param2;
 // remaining body: draw via 0x6E2C0/0x6AEE0 (host-provided)
 return h.lookup(0x6B5A0,I(tier),param2,0)?1:0;
}

// 1.13c D2Client+0x6C130 : per-tick animation state machine (record kinds 0..9). Host-driven step.
// Returns number of records advanced.
struct Anim { U kind,a,b,c,d,step,pos,limit,u8,u9,delay,stamp,f; };
void sub_6C130(Host& h,Anim* a){
 U now=h.tick();
 if(!a||a->stamp==now) return;
 a->stamp=now;
 a->pos+=a->step;
 if(I(a->limit)<=I(a->pos)){
  switch(a->kind){
   case 1: case 3: case 8: a->pos-=a->limit; break;
   case 2: a->a=0; break;
   case 5: a->step=0; a->pos=a->limit-1; a->a=1; break;
   case 6: a->pos-=a->limit; a->a=0; a->stamp=h.tick(); break;
   default: h.lookup(0x69180,0,0,0); break;
  }
 }
}

// 1.13c D2Client+0x6D010 : per-frame group pump
void sub_6D010(Host& h,I n,U unit,U arg){
 for(I i=0;i<n;++i){ sub_6C3B0(h); sub_6C3F0(h,unit,arg); }
}

} // namespace

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

// 1.13c D2Client+0x6C3F0 : walk the per-class bit table. rec = class record (+0xf0 bitmap) or null
void sub_6C3F0(Host& h,const uint8_t* rec,I grp,I groupCount){
 if(!rec) return;
 for(U i=0;i<h.count1;++i){
  U b=h.list1[i];
  if(h.bitMasks[(b&7)*4] & rec[(b>>3)+0xf0]) sub_6C020(h,0,grp,groupCount);
 }
}

// 1.13c D2Client+0x6CB30 : ebx-style mode==0x11 gate
void sub_6CB30(Host& h,I mode,U grp,I groupCount,const uint8_t* rec){
 h.ord(10175,0,0,0,0); h.ord(10350,0,0,0,0);
 if(mode==0x11 && I(grp)<groupCount){
  dw(h.tbl,grp*0x28+9)=0; dw(h.tbl,grp*0x28+9+0x14)=0;
  if(h.ord(10331,0,0,0,0)) h.ord(10043,0,0,0,0);
  sub_6C3F0(h,rec,I(grp),groupCount);
 }
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
void sub_6D010(Host& h,I n,const uint8_t* rec,I grp,I groupCount){
 for(I i=0;i<n;++i){ sub_6C3B0(h); sub_6C3F0(h,rec,grp,groupCount); }
}

} // namespace

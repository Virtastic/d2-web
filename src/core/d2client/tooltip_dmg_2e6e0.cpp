// Item-tooltip damage/stat line builders, D2Client 0x2E6E0..0x2F0A0 (batch rows s3474-s3503).
// Derived from cached 1.13c decompile; ordinals and sibling D2Client routines are injected via Host.
// Register/stack-carried args (EAX/ESI/stack slots) are named parameters.
#include <cstdint>
#include <cstring>
#include <cstdio>
namespace d2client_tipdmg {
using I=int32_t; using U=uint32_t; using P=void*;
struct Range { I lo, hi, a, b, c; };   // Ordinal_11089 result: lo/hi damage plus the scale words 2E900 reads
struct Host {
 void (*lineStr)(I id,I val,I z);                 // 0x2DBF0
 const char16_t* (*sep)();                        // Ordinal_10003
 void (*strcat)(char16_t* dst,const char16_t* s); // Unicode::strcat
 I (*lineOk)();                                   // 0x2C870 (low 16 bits tested)
 void (*toUni)(char16_t* dst,const char* s,I n);  // Unicode::toUnicode
 I (*dataClass)(I idx,P* rec);                    // data-table row (0x1A8 stride) or 0
 void (*range)(Range* out,I id,I mode,I cls,I val,I n); // Ordinal_11089
 I (*scaled)(I a,I b,I c);                        // Ordinal_10786
 void (*emit)(char16_t* dst,I lo,I hi);           // 0x2D220
 void (*emit2)(I lo,I hi);                        // 0x2CF20
 void (*emit3)(char16_t* dst,I id,I val);         // 0x2E060
 I (*dmgMin)(P u); I (*dmgMax)(P u);              // Ordinal_10121 / 11091
 void (*lineA)(I id1,I id2,I v,I f);              // 0x2D9B0
 void (*lineB)(I id,I v,I z);                     // 0x2DCA0
 I (*ord10653)(I a,I b);
 I (*ord10205)(I z,P u); I (*ord10532)(I z,P u);
 I (*mode)(); I (*tableRowBytes)(I tbl,I idx,P* rec);
 I (*rowsByte)(P a);
 const I* classIdx; const I* gsA; const I* gsB; const I* gsC; // DAT_6fbcc390 / 6fbcc394 / 6fbc9858
};
static I mulpct(I v,I x){ // overflow-safe v*x/100 exactly as the three-way branch in 2E900
 if(x<=0x100000){
  if(v<=0x10000) return I(U(v)*U(x))/100;
  if((v&~15)>=0x640) return (v/100)*x;
 } else if((x&~15)>=0x640) return (x/100)*v;
 return I((int64_t)v*(int64_t)x/100);
}
static void fmtRange(Host&h,char16_t* dst,I lo,I hi){
 char16_t u[64]; std::memset(u,0,sizeof u); char s[32];
 if(!h.lineOk()) return; // (short) test
 bool same=lo==0?hi==0:lo==hi;
 if(same) std::snprintf(s,sizeof s,"%d",lo); else std::snprintf(s,sizeof s,"%d-%d",lo,hi);
 h.toUni(u,s,0x20); h.strcat(dst,h.sep()); h.strcat(dst,u); h.strcat(dst,h.sep());
}
// 1.13c D2Client+0x2E6E0
I sub_2E6E0(Host&h,I level,char16_t* dst,I cls){
 if(level>1){
  P rec=nullptr; I v=0;
  if(h.dataClass(cls,&rec)&&level-1>=1) v=(level-2)*((I*)rec)[0x14c/4]+((I*)rec)[0x148/4];
  h.lineStr(0x10d5,v,0);
  h.strcat(dst,h.sep()); h.strcat(dst,h.sep());
 } return 1;
}
// 1.13c D2Client+0x2E770
I sub_2E770(Host&h,P item,char16_t* dst,I recIdx,I id){
 P rec=nullptr; if(!h.dataClass(recIdx,&rec)) return 0;
 Range r{}; I cls=*h.classIdx; I g=(*h.gsA<2||*h.gsA>3)?1:*h.gsB;
 h.range(&r,id,g,cls,((int16_t*)((char*)rec+0xaa))[cls],8);
 (void)item; fmtRange(h,dst,r.b+r.lo,r.hi+r.a); return 1;
}
// 1.13c D2Client+0x2E900
I sub_2E900(Host&h,P item,char16_t* dst,I recIdx){
 P rec=nullptr; if(!h.dataClass(recIdx,&rec)) return 0;
 Range r{}; I cls=*h.classIdx; I g=(*h.gsA<2||*h.gsA>3)?1:*h.gsB;
 h.range(&r,recIdx,g,cls,((int16_t*)((char*)rec+0xaa))[cls],8);
 U lo=U(r.lo),hi=U(r.hi); I sum=0; char* b=(char*)item;
 for(int i=0;i<6;i++) if(((int16_t*)(b+0x54))[i]==0x19) sum+=h.scaled(r.a,((I*)(b+0x68))[i],r.b);
 for(int i=0;i<5;i++) if(((int16_t*)(b+0x98))[i]==0x19) sum+=h.scaled(r.a,((I*)(b+0xa4))[i],r.b);
 if(sum){
  if(sum<-0x59) sum=-0x5a;
  hi+=U(mulpct(sum,I(hi))); lo+=U(mulpct(sum,I(lo)));
 }
 h.emit(dst,I(hi),I(lo)); return 1;
}
// 1.13c D2Client+0x2EB80  (item type byte at +0x1dc: every case calls the same emitter)
void sub_2EB80(Host&h,P u,P item){ (void)item; h.emit2(h.dmgMin(u)*0x19>>8,h.dmgMax(u)*0x19>>8); }
// 1.13c D2Client+0x2EC60
I sub_2EC60(Host&h,P u,char16_t* dst,I ord10510){
 I lo=h.dmgMin(u)*ord10510>>8, hi=h.dmgMax(u)*ord10510>>8; (void)lo;
 h.emit3(dst,0x2b0e,ord10510);
 fmtRange(h,dst,hi,lo); return 1; // lo/hi roles as decompiled: first printed value is emit3's out word
}
// 1.13c D2Client+0x2ED80
void sub_2ED80(Host&h,P u){ h.emit2(h.dmgMin(u)*0x4b>>8,h.dmgMax(u)*0x4b>>8); }
// 1.13c D2Client+0x2EDD0 (all six type cases call the same emitter)
void sub_2EDD0(Host&h,P u,I esiLo){ I hi=h.ord10532(0,u); h.ord10205(0,u); h.emit2(esiLo>>8,hi*0x4b>>8); }
// 1.13c D2Client+0x2EEF0
void sub_2EEF0(Host&h,I a,I b){ I r=h.ord10653(b,a); if(r) h.lineA(0x10a6,0x10b3,r,1); }
// 1.13c D2Client+0x2EF20
I sub_2EF20(Host&h,P u,char16_t* dst){
 I a=h.ord10205(0,u), b=h.ord10532(0,u);
 if((a>>8)==0&&(b>>8)==0) return 0;
 h.emit(dst,a>>8,b>>8); return 1;
}
// 1.13c D2Client+0x2EF80
void sub_2EF80(Host&h,char16_t* dst,P u){
 I a=h.dmgMin(u), b=h.dmgMax(u);
 if(a==0&&b==0) return;
 h.emit(dst,a>>8,b>>8);
}
// 1.13c D2Client+0x2EFD0
void sub_2EFD0(Host&h,I recIdx,I id){
 P rec=nullptr; if(!h.dataClass(recIdx,&rec)) return;
 Range r{}; I cls=*h.classIdx; I v=((int16_t*)((char*)rec+0xaa))[cls];
 h.range(&r,id,0,cls,v,0);
 I half=(v+1)/2; half=half+(half*r.b)/100+r.a;
 if(half) h.lineB(0x10c6,half,0);
}
// 1.13c D2Client+0x2F0A0 -- range formatter ("%d", "%d-%d", "%s%d-%dK", "%s%dK-%dK")
void sub_2F0A0(char* out,I lo,I hi,const char* prefix){
 if(hi<=lo) hi=lo+1; // decompile: EAX replaced by this+1 when EAX<=this
 if(lo==hi){ std::snprintf(out,32,"%d",lo); return; }
 if(lo<10000){
  if(hi<10000){ std::snprintf(out,32,"%d-%d",lo,hi); return; }
  std::snprintf(out,32,"%s%d-%dK",prefix,lo,(hi+500)/1000);
 } else std::snprintf(out,32,"%s%dK-%dK",prefix,(lo+500)/1000,(hi+500)/1000);
}
}

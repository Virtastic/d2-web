// Batch 43710: derived from cached 1.13c D2Client decompile. Ordinals are host callbacks (Calls);
// REG? = register/stack input not confirmed against x86. Unnamed ordinals are marked ORD.
#include <cstdint>
#include <cstring>
namespace d2client_batch_43710 {
using U=uint32_t; using I=int32_t; using P=void*; using W=char16_t;
static size_t wlen(const W*s){size_t n=0;while(s[n])++n;return n;}
static void wcat(W*d,const W*s){d+=wlen(d);while((*d++=*s++)){}}
struct Calls {
 // globals (RVA in comment is the 1.13c data address)
 I g_b8bc48; I g_c9854; P g_cbbbfc; U g_cc390; const U* tbl_82e50; const U* tbl_85d20; // 8bc48 c9854 cbbbfc cc390 82e50 85d20
 void (*drawText)(I strId,I x);              // D2Client+0x42C10
 void (*ord10003)(I,I,I); void (*ord10150)(); // text/draw ORD
 P (*ord10331)(P); U (*ord10826)(P); P (*ord10014)(U); U (*ord10265)(I); void (*ord10024)(const void*,U);
 void (*ord10936)(); void (*ord10614)(); void (*fb60450)(U);
 I (*ord10106)(); I (*ord10620)(); P (*ord10871)(); I (*ord10680)(P,I,I);
 P (*ord10218)(); U (*ord11161)(); I (*ord10550)();
 void (*fb05b20)(P ths,P unit,W* out,I cap,I flag,U col); // 55B20
 void (*ord10996)(I,I,P,U); I (*ord10872)(P,I,I); void (*ord11111)(P,I); void (*ord11108)(I);
 I (*ord10744)(P,I); const W* (*ord10003s)(I); I (*ord10695)(I); I (*ord10806)(U); void (*ord10569)(P,I*,I*);
 const void* dataTables; // sgptDataTables
};
// 1.13c D2Client+0x43710
void sub_43710(Calls&c){
 c.drawText(0x13ea,c.g_b8bc48/2); c.ord10003(c.g_c9854?100:0x3e,4,0); c.ord10150();
 if(!c.g_c9854){c.drawText(0x13eb,c.g_b8bc48/2); c.ord10003(0x6b,4,0); c.ord10150();}
 c.drawText(0x13ec,c.g_b8bc48/2); c.ord10003(c.g_c9854?0x110:0xe8,4,0); c.ord10150();
}
// 1.13c D2Client+0x437E0  (returns *(+0x218) of record; _exit(-1) path reported via ord10024)
U sub_437E0(Calls&c,bool*fatal){
 *fatal=false; if(!c.g_cbbbfc) return 0;
 P a=c.ord10331(c.g_cbbbfc); if(!a) return 0;
 P r=c.ord10014(c.ord10826(a));
 if(!r){c.ord10024(nullptr,c.ord10265(0x5d7));*fatal=true;return 0;}
 U v; std::memcpy(&v,static_cast<uint8_t*>(r)+0x218,4); return v;
}
// 1.13c D2Client+0x4C7C0  (REG? in_EAX = unit; param_1 stack)
void sub_4C7C0(Calls&c,const I*unit,U param_1,const uint8_t*dt_a78,I dt_a80){
 if(!unit||unit[0]!=1) return;
 I i=unit[1],k=i; const uint8_t*rec=nullptr;
 if(i>=0&&i<dt_a80){rec=dt_a78+i*0x1a8;int16_t s;std::memcpy(&s,rec+2,2);k=s;}
 if(k>=0&&k<dt_a80&&k==0x3f&&rec){
  c.ord10936();c.ord10614();int16_t n;std::memcpy(&n,rec+0x62+(c.g_cc390&0xffff)*2,2);
  for(I j=n;j>0;--j)c.fb60450(param_1);
 }
}
// 1.13c D2Client+0x55B20 : tooltip line builder, 1004 bytes. Not transcribed beyond the entry
// contract (ORD-heavy, 6 args, this=ECX): see fb05b20 callback; NAMED only.
// 1.13c D2Client+0x55F10  (REG? unaff_EBX/ESI/retaddr passed through to 55B20)
void sub_55F10(Calls&c,const uint8_t*dt_c18,I dt_c1c){
 if(c.ord10106()!=5) return; I q=c.ord10620();
 if(q<0||q>=dt_c1c) return; const uint8_t*rec=dt_c18+q*0x1b8; int16_t w; std::memcpy(&w,rec+0x2c,2);
 for(I i=0;i<=5;++i){P it=c.ord10871(); if(it&&c.ord10680(it,0x47,0)==w){c.fb05b20(nullptr,nullptr,nullptr,0,0,c.tbl_82e50[i]);return;}}
}
// 1.13c D2Client+0x55FB0  (REG? unaff_EDI = unit/mask word, EBX)
void sub_55FB0(Calls&c,const I*edi,P unitMask){
 if(!edi||edi[0]!=4||c.ord10106()!=5) return; const uint8_t*p=(const uint8_t*)c.ord10218(); if(!p) return;
 if(p[0x87]==1){
  c.ord11161(); I me=c.ord10550();
  for(I i=0;i<6;++i){ if(i==me) continue; I o=i*4; if(me<i)o-=4;
   if(((uintptr_t)unitMask>>i)&1) c.fb05b20(nullptr,nullptr,nullptr,0,0,*(const U*)((const uint8_t*)c.tbl_82e50+o)); }
 } else if(p[0x87]==2){
  U v=c.ord11161(); I n=v<0x40?(I)c.tbl_85d20[v]:0;
  for(I i=0;i<n-1;++i)c.fb05b20(nullptr,nullptr,nullptr,0,0,c.tbl_82e50[i]);
 }
}
// 1.13c D2Client+0x560B0  (REG? unaff_EDI = destination W*)
void sub_560B0(Calls&c,P unit,W*dst,const W*param_3){
 static thread_local W buf[1024]; buf[0]=0; c.fb05b20(nullptr,unit,buf,0x400,1,0);
 I len=param_3?(I)wlen(buf):0;
 if(!param_3||len==0){wcat(dst,buf);return;}
 I k=len-2;
 if(k>0){ while(buf[k-1]!=10){--k; if(k<1){wcat(dst,param_3);wcat(dst,buf);return;}}
  const W*a=c.ord10003s(0),*b=c.ord10003s(0); buf[k-1]=0;
  wcat(dst,buf);wcat(dst,a);wcat(dst,param_3);
  if(buf[len]==10)buf[len]=0;   // mirrors decompile: index relative to local_804
  wcat(dst,buf+k);wcat(dst,b);wcat(dst,a);return; }
 wcat(dst,param_3);wcat(dst,buf);
}
static void trimNl(W*s){size_t n=wlen(s); if(n&&s[n-1]==10)s[n-1]=0;}
// 1.13c D2Client+0x56210  (fastcall ECX=param_1)
void sub_56210(Calls&c,U param_1,W*out,P unit){
 c.ord10996(2,0,unit,param_1); sub_560B0(c,unit,out,nullptr);
 I it=c.ord10872(unit,0,0x40); if(it){c.ord11111(unit,it);c.ord11108(it);} trimNl(out);
}
// 1.13c D2Client+0x56280  (REG? EBX = unit, stack = tooltip arg)
void sub_56280(Calls&c,W*out,P unit,const W*arg){
 c.ord10996(5,0,nullptr,0); static thread_local W buf[1024]; buf[0]=0;
 sub_560B0(c,unit,buf,arg); wcat(out,buf);
 I it=c.ord10872(unit,0,0x40); if(it){c.ord11111(unit,it);c.ord11108(it);} trimNl(out);
}
// 1.13c D2Client+0x56330 : item tooltip assembler (931 bytes), caller 0x933E0. Fatal strings 0x21a/0x21c/0x223/0x245.
// NAMED only; body is 4x(strcat x3 + 56210|56280) repeated per quality branch, see decompile.
// 1.13c D2Client+0x56700  (fastcall ECX=obj)
U sub_56700(Calls&c,uint8_t*obj){ I a,b; c.ord10569(obj,&a,&b); obj[0x4e]=0; if(a<1){U z=0;std::memcpy(obj+0x3c,&z,4);} return 1; }
// 1.13c D2Client+0x56740  (byte-identical twin of 56700)
U sub_56740(Calls&c,uint8_t*obj){ return sub_56700(c,obj); }
}

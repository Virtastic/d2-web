// Implementations derived from cached 1.13c D2Client decompiles. Register/caller-frame inputs are explicit
// parameters; Win32/CRT/ordinal callees are function-pointer seams. No game data in this file.
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace d2client_n92 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(const void*p,U o){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,4);return v;}
static void wr(void*p,U o,U v){std::memcpy(static_cast<uint8_t*>(p)+o,&v,4);}
// 1.13c D2CLIENT+0x184D0
struct Fn184D0{ char (*type)(P); P (*slotItem)(P,I); I (*busy)(P); P (*owner)(P); U (*fallback)(P);};
U sub_184D0(P ctx,P unit,const Fn184D0&f){
 char t=f.type(unit);
 if(t==5||t==4){P o=reinterpret_cast<P>(static_cast<uintptr_t>(rd(ctx,0x60)));
  P a=f.slotItem(o,4),b=f.slotItem(o,5);
  if(a&&b&&!f.busy(a)&&!f.busy(b)&&unit!=f.owner(o))return 6;}
 return f.fallback(unit);}
// 1.13c D2CLIENT+0x18880
struct Fn18880{ P (*cur)(); I (*q306)(P,I,I); I (*q786)(P,U,I,I); I (*q522)(P,I);};
bool sub_18880(P obj,I p1,I p2,I out[2],const Fn18880&f){
 if(!obj||!p1||!p2)return false;
 P rec=f.cur(); if(!rec)return false;
 I a=f.q306(obj,p1,1);
 out[0]=f.q786(obj,rd(rec,0x64),int16_t(rd(rec,0)&0xffff),a);
 out[1]=f.q522(obj,p1);
 return out[1]<=out[0];}
// 1.13c D2CLIENT+0x18B20
struct Fn18B20{ void (*o10084)(); I (*o10494)(); void (*sub18e30)(); void (*o10887)(); void (*fatal)(U); U *g5f58,*g5f5c;};
void sub_18B20(P u,const Fn18B20&f){
 *f.g5f58=0;*f.g5f5c=0; f.o10084();
 if(u)wr(u,0xc4,(rd(u,0xc4)&~4u)|0x10000);
 if(!f.o10494()&&u)wr(u,0xc4,rd(u,0xc4)&~2u);
 f.sub18e30();
 if(u&&rd(u,0x54)){uint8_t*b=reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rd(u,0x54)));for(int i=0;i<0x14;++i)b[i*2]=0;f.o10887();return;}
 f.fatal(0xbba);}
// 1.13c D2CLIENT+0x19030
struct Fn19030{ I (*o10708)(I); I (*sub18f90)(P); I (*o10601)(I,const char*,I);};
U sub_19030(I*self,P arg,const Fn19030&f){
 I h=f.o10708(self[1]);
 if(h){ if(!f.sub18f90(arg))return 0;
  I old=*self; I n=f.o10601(h,"..\\Source\\D2Client\\UNIT\\Player.cpp",0x32c); *self=n; if(n!=old)self[1]=-1;}
 return 1;}
struct Fn1AB20{
 I (*dataFlag)(I); void (*o10159)(P); U (*s1660)(P); U (*s1630)(P); void (*s48a0)(I,I,U,U); void (*s6c0)();
 I (*o10407)(P,P); I (*o10324)(P,P,I); I (*o10594)(P,P); I (*o10494)(U,I); void (*o10688)(I);
 I (*s9f30)(); P (*s55b40)(I,I); void (*s750)(I,P); void (*s7c0)(U); void (*s2d50)(I,I,I);
 void (*s230)(); I (*s8ac0)(I,I); void (*s010)(P,U,P); P (*unitPtr)(U); U now,*g97ec,*g9828,*g3e4,dhandle;};
static void clr15(P u){ if(u&&rd(u,0)==0&&rd(u,0x14)){P x=reinterpret_cast<P>(static_cast<uintptr_t>(rd(u,0x14)));for(U o=0x150;o<=0x15c;o+=4)wr(x,o,0);}}
static bool tail9f(P pl,I a,I b,const Fn1AB20&f){ // shared "reset hover" tail
 I ok=f.s9f30(); if(!ok)return false;
 P p=(a==4)?f.s55b40(b,4):nullptr; if(p&&rd(p,0)==4&&!((rd(p,0xc4)>>1)&1))return false;
 f.s750(2,pl); clr15(pl); return true;}
// 1.13c D2CLIENT+0x1AB20
void sub_1AB20(const uint8_t*fr,I a,I b,const Fn1AB20&f){
 P tgt=reinterpret_cast<P>(static_cast<uintptr_t>(rd(fr,8)));
 P pl=reinterpret_cast<P>(static_cast<uintptr_t>(rd(fr,4)));
 U kind=tgt?rd(tgt,0):99;
 auto common=[&](){
  if(tgt&&rd(tgt,0)==1){I i=I(rd(tgt,4));int v=f.dataFlag(i);
   if(v>=0&&(v&1)&&(v&2)){ if(rd(tgt,0x2c))f.o10159(reinterpret_cast<P>(static_cast<uintptr_t>(rd(tgt,0x2c))));
    P t2=reinterpret_cast<P>(static_cast<uintptr_t>(rd(fr,8)));
    f.s48a0(t2?I(rd(t2,0)):6,t2?I(rd(t2,0xc)):-1,f.s1630(t2),f.s1660(t2));
    t2=reinterpret_cast<P>(static_cast<uintptr_t>(rd(fr,8)));
    if(t2&&rd(t2,0)==1&&rd(t2,0x14)){uint8_t*q=reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rd(t2,0x14)))+0x28;*q|=1;}
    f.s6c0();}}
  if(f.o10407(tgt,pl)<=b){ tail9f(pl,a,b,f); return;}
  f.s7c0(rd(fr,0x18)); f.s2d50(0x13,a,b);};
 switch(kind){
 case 0:
  if(f.o10407(tgt,pl)<5&&!f.o10324(pl,tgt,0x804)){tail9f(pl,a,b,f);return;}
  f.s7c0(rd(fr,0x18)); f.s2d50(0x13,a,b); return;
 case 1: f.s230(); common(); return;
 case 2: case 4:{
  if(fr[0]&8)return;
  bool ok=true;
  if(kind==2){ f.o10688(tgt?I(rd(tgt,4)):-1);
   if(tgt&&rd(tgt,4)==0x3b&&f.o10494(f.dhandle,0x66))ok=false;
   if(f.o10594(pl,tgt)&&!f.o10324(pl,tgt,0x804)){
    if(!ok)return; I g=f.s9f30(); if(!g){*f.g3e4=1;return;}
    P p=(a==4)?f.s55b40(b,4):nullptr; if(p&&rd(p,0)==4&&!((rd(p,0xc4)>>1)&1)){*f.g3e4=1;return;}
    f.s750(2,pl); clr15(pl); *f.g3e4=1; return;}
  } else if(f.o10407(tgt,pl)<5&&!f.o10324(pl,tgt,0x804)){
   I g=f.s9f30(); if(!g){*f.g3e4=1;return;}
   P p=(a==4)?f.s55b40(b,4):nullptr; if(p&&rd(p,0)==4&&!((rd(p,0xc4)>>1)&1)){*f.g3e4=1;return;}
   f.s750(2,pl); clr15(pl); *f.g3e4=1; return;}
  if(f.s8ac0(a,b))return;
  if(kind==2){f.s1660(tgt);f.s010(reinterpret_cast<P>(static_cast<uintptr_t>(f.s1630(tgt))),rd(fr,0x14),pl);if(!ok)return;}
  else f.s7c0(rd(fr,0x18));
  f.s2d50(0x13,a,b); *f.g3e4=1; return;}
 case 5:
  if(*f.g97ec<*f.g9828)return;
  if(f.o10407(tgt,pl)>4){f.s7c0(rd(fr,0x18));f.s2d50(0x13,a,b);*f.g9828=*f.g97ec+500;return;}
  tail9f(pl,a,b,f); *f.g9828=*f.g97ec+500; return;
 default: common(); return;}}
// 1.13c D2CLIENT+0x2B380
struct Fn2B380{ const char*(*o10070)(); U (*o10016)(); I (*s_d810)(U,P); void (*o10006)(I,U*,const char*,I,I,const char*);};
const char* sub_2B380(const char*name,P arg,const Fn2B380&f){
 char buf[260]; std::strncpy(buf,name,259); buf[259]=0;
 std::strncat(buf,f.o10070(),259-std::strlen(buf));
 I h=f.s_d810(f.o10016(),arg); U z=0; const char*src="..\\Source\\D2Client\\CORE\\ARCHIVE.CPP";
 f.o10006(h,&z,src,0x6b,-1,buf); return src;}
static void wcat(char16_t*d,const char16_t*s){while(*d)++d;while((*d++=*s++));}
// 1.13c D2CLIENT+0x2CE10
struct Fn2CE10{ const char16_t*(*str)(I);};
U sub_2CE10(int16_t id,I val,I plus,I paren,char16_t*dst,const Fn2CE10&f){
 if(!val)return 0;
 char a[32]; std::snprintf(a,32,(plus&&val>=0)?"+%d":"%d",val);
 char16_t w[32]{}; for(int i=0;a[i]&&i<31;++i)w[i]=uint8_t(a[i]);
 if(id!=0x1506){wcat(dst,f.str(0));wcat(dst,f.str(1));wcat(dst,f.str(2));}
 wcat(dst,w); if(paren)wcat(dst,f.str(3));
 wcat(dst,f.str(4));wcat(dst,f.str(5));wcat(dst,f.str(6)); return 1;}
}

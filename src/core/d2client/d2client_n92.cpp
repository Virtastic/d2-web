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
// D2Client 2B380: moved to d2client_2b380.cpp (EAX path, stdcall outSize/rootArg).
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

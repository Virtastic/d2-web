// Prior art used: UI/input entries; D2MOO has no D2Client.
// Derived from cached 1.13c code, with register/stack arguments checked in x86.
// Native handles stay pointers; serialized unit/path fields use a resolver.
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "belt_26060.h"
namespace d2client_belt {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
static I add(I a,I b){return I(U(a)+U(b));}
// 1.13c D2Client+0x26060
void sub_26060(P source,P unit,Calls& c){
 for(I a=0;a<4;++a)for(I b=0;b<4;++b){
  Effect e{}; e.words[0]=3;e.unit=unit;e.words[4]=0x3e;
  I type=I(rd(source,0));P path=c.resolve(rd(source,0x2c));
  e.words[5]=(type==2||(type>3&&type<6))?rd(path,12):(path?U(c.pathX(path)):0);
  type=I(rd(source,0));path=c.resolve(rd(source,0x2c));
  e.words[6]=(type==2||(type>3&&type<6))?rd(path,16):(path?U(c.pathY(path)):0);
  e.words[7]=U((a%2?-1:1)*(a+1)*5);e.words[8]=U((b%2?-1:1)*(b+1)*5);
  e.words[12]=U(std::clamp(c.stat(unit,12,0)/5,1,8));c.effect(&e);
 }
}
// EAX upper bytes survive retail's AL=1 return; carry incoming EAX explicitly.
// 1.13c D2Client+0x261F0
U sub_261F0(State& s,Calls& c,P actor,U eax){
 P target=s.player;
 if((!actor||rd(actor,16)==0)&&target){
  eax=U(c.questFlag(s.quest,0x27,0));
  if(eax){eax=U(c.questFlag(s.quest,0x27,4));if(!eax){
   eax=U(c.distance(actor,s.player));if(I(eax)<25){
    P path=c.resolve(rd(target,0x2c));if(path)c.stopPath(path);
    U id=rd(actor,12);eax=U(c.canAct(target));if(eax){U args[7]={2,id,0,0,0,0,0};c.action(2,target,args,0);
     if(rd(target,0)==0&&rd(target,20))for(U off=0x150;off<=0x15c;off+=4)wr(c.resolve(rd(target,20)),off,0);
    }c.setMode(actor,2);eax=c.finish(actor,0);
   }
  }}
 }return (eax&0xffffff00u)|1;
}
// 1.13c D2Client+0x26410
void sub_26410(State& s){s.selected=s.x=s.y=-1;s.active=0;s.current=s.previous=nullptr;}
// EDI is the x input, stack argument is y. Info byte 4 is slot count.
// 1.13c D2Client+0x26460
I sub_26460(State& s,Calls& c,I x,I y){uint8_t info[264];c.beltInfo(s.belt,s.inventory,info);
 for(I n=0;n<I(info[4]);++n){I r[4];c.rect(s.belt,s.inventory,r,n);if(r[0]<=x&&x<=r[1]&&r[2]<=y&&y<=r[3])return n;}return -1;}
// 1.13c D2Client+0x265E0
void sub_265E0(State& s,Calls& c){s.colors[0]=uint8_t(c.color(128,0,0));s.colors[1]=uint8_t(c.color(0,128,0));s.colors[2]=uint8_t(c.color(0,0,128));s.colors[3]=uint8_t(c.color(128,128,0));}
// ECX unit, EDX x, EAX y; rect index is itemIndex, not selected slot.
// 1.13c D2Client+0x266C0
void sub_266C0(State& s,Calls& c,P unit,I x,I y){I n=sub_26460(s,c,x,y);if(n<0)return;
 I special=c.special(unit);if(s.mode==6||s.mode==8)return;
 if(special){s.selected=n;s.current=s.previous=nullptr;s.active=1;return;}
 P item=c.item(unit,n);s.selected=n;if(!item){s.active=0;s.current=s.previous=nullptr;return;}
 s.active=1;if(item!=s.previous){I index=c.itemIndex(item),r[4];c.rect(s.belt,s.inventory,r,index);s.current=s.previous=item;s.x=add(r[0],14);s.y=r[2];}}
}

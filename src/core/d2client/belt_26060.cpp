// Prior art used: UI/input entries; D2MOO has no D2Client.
// Derived from cached 1.13c code, with register/stack arguments checked in x86.
// Native handles stay pointers; serialized unit/path fields use a resolver.
#include <cstdint>
#include <cstring>
#include <algorithm>
namespace d2client_belt {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
static I add(I a,I b){return I(U(a)+U(b));}
struct Draw { U words[13]{}; P cell=nullptr; U tail[4]{}; };
struct Effect { U words[23]{}; P unit=nullptr; };
struct State {
 I selected=-1,x=-1,y=-1,active=0,mode=0,belt=0,width=0,height=0,mx=0,my=0,expanded=0,enabled=0;
 P current=nullptr,previous=nullptr,inventory=nullptr,player=nullptr,quest=nullptr;
 uint8_t colors[4]{}; P cells[30]{};
};
struct Calls {
 P (*resolve)(U);
 I (*pathX)(P); I (*pathY)(P); I (*stat)(P,I,I); void (*effect)(Effect*);
 I (*questFlag)(P,I,I); I (*distance)(P,P); void (*stopPath)(P); I (*canAct)(P); // 29F30 reads the player in ESI.
 void (*action)(I,P,const U*,I); void (*setMode)(P,I); U (*finish)(P,I);
 void (*beltInfo)(I,P,uint8_t*); void (*rect)(I,P,I*,I);
 I (*special)(P); P (*item)(P,I); I (*itemIndex)(P);
 U (*color)(I,I,I); const char16_t* (*text)(U); void (*tooltip)(const char16_t*,I,I,I,I);
 void (*fill)(I,I,I,I,U,I); void (*draw)(Draw*,I,I,U,I,I);
 void (*unload)(P); void (*release)(P,I,I); P (*load)(const char*);
};
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
static bool hover(const State& s){return add(s.width/2,-8)<=s.mx&&s.mx<=add(s.width/2,5)&&add(s.height,-39)<=s.my&&s.my<=add(s.height,-13);}
// 1.13c D2Client+0x267B0
void sub_267B0(State& s,Calls& c){if(hover(s)){const char16_t* text=c.text(s.expanded?0x1048:0x1047);c.tooltip(text,add(s.width/2,-1),add(s.height,-39),0,1);}}
// ESI color index, stack slot index. Rect x/y are words zero/two.
// 1.13c D2Client+0x26820
void sub_26820(State& s,Calls& c,I color,I slot){sub_265E0(s,c);if(color!=2){I r[4];c.rect(s.belt,s.inventory,r,slot);c.fill(r[0],r[2],add(r[0],29),add(r[2],29),s.colors[color],0);}}
// 1.13c D2Client+0x26880
void sub_26880(State& s,Calls& c){Draw d{};d.cell=s.cells[(0x7d4-0x770)/4];d.words[0]=U(s.expanded?2:0)+U(s.enabled&&hover(s));c.draw(&d,add(s.width/2,-8),add(s.height,-16),0xffffffff,5,0);}
// Release receives the saved allocation in ECX after unloading it.
// 1.13c D2Client+0x26CD0
void sub_26CD0(State& s,Calls& c){constexpr U offsets[]={0x7e0,0x7c8,0x7dc,0x788,0x7c4,0x778,0x774,0x7d8,0x77c,0x780,0x7cc,0x7e4,0x7d4,0x790,0x784,0x770,0x78c};
 for(U off:offsets){P& cell=s.cells[(off-0x770)/4];P old=cell;if(old){c.unload(old);c.release(old,0x7a,0);cell=nullptr;}}
 for(U n=0;n<12;++n)s.cells[(0x794-0x770)/4+n]=nullptr;
}
static void panel(State& s,Calls& c,bool right){P& cell=s.cells[3];if(!cell)cell=c.load("Panel\\800BorderFrame");Draw d{};d.cell=cell;
 constexpr I left[5][2]={{0,253},{256,63},{0,484},{0,553},{256,553}};
 constexpr I rhs[5][2]={{400,63},{544,253},{713,484},{544,553},{400,553}};
 for(U i=0;i<5;++i){d.words[0]=i+(right?5:0);c.draw(&d,right?rhs[i][0]:left[i][0],right?rhs[i][1]:left[i][1],0xffffffff,5,0);}}
// 1.13c D2Client+0x270D0
void sub_270D0(State& s,Calls& c){panel(s,c,true);}
// 1.13c D2Client+0x271C0
void sub_271C0(State& s,Calls& c){panel(s,c,false);}
}

// Prior art used: D2Client has no D2MOO counterpart; community address tables
// are naming aids only. Rules below derive from cached 1.13c bodies and x86
// disassembly (especially EAX/ESI parameters absent from the decompiler).
#include <cstdint>
#include <cstring>
#include <string>
#include <emscripten.h>
namespace client27290 {
using I=int32_t; using U=uint32_t; using P=void*;
// Native objects are resolved by the embedding client. Retail scalar offsets
// remain byte offsets, never offsets into a widened native pointer structure.
struct Cell { I frame{}; U reserved[12]{}; P file{}; U tail[4]{}; };
struct Key { U action; int16_t key; U side; };
struct Threshold { U type,frame,high,medium,low; };
struct State {
 I width,height; P panel{},widePanel{},oldPanel{},button{},warning{},durability{},bars{},barFrame{};
 P statusPanel{},player{},hover{}; I selected{},hoverX{},hoverY{},mouseX{},mouseY{};
 I inventoryOpen{},hoverEnabled{},hoverA{},hoverB{},buttonPressed{},buttonEnabled{};
 I expansion{},blocked[5]{},panelOpen{},panelMode{}; uint8_t occupied[6]{};
 Key* keys; U keyCount; I* slots; I* ammoSlots; U ammoCount; Threshold* thresholds; U thresholdCount;
 I selectionSource{},selectionPage{};
};
struct Env {
 U (*word)(P,U); P (*pointer)(P,U); U (*retailAddress)(P); P (*resolveRetail)(U);
 I (*resolution)(); I (*renderer)(); P (*load)(U); void (*unload)(P); void (*freeFile)(P,U,U,U);
 void (*draw)(const Cell*,I,I,I,I,U); void (*clip)(const Cell*,I,I,I,I,I);
 I (*current)(I); I (*maximumLife)(P); I (*maximumMana)(P); I (*state)(P,I); I (*stat)(P,I,I);
 I (*isExpansion)(); P (*cursor)(P); void (*selectionInfo)(I,I,uint8_t*);
 I (*selectionBlocked)(I,I); I (*cursorValid)(P); I (*selectionTest)(I,P,I,U*); void (*select)(I,I);
 I (*buttonTest)(I);
 P (*equipped)(P,I); I (*durable)(P); I (*excluded)(P); I (*maxDurability)(P); I (*itemFlag)(P,I); I (*special)(P);
 I (*ammoStat)(P); uint8_t (*ammoType)(P); I (*singleStack)(P);
 P (*itemRecord)(U); P (*first)(P); P (*next)(P); P (*item)(P); uint8_t (*skip)(P);
 const char16_t* (*localized)(U); U (*keyName)(U,U);
 void (*formatKey)(char16_t*,U,const char16_t*,U);
 void (*text)(const char16_t*,I,I,I,I);
 void (*name)(P,char16_t*,U); I (*quality)(P); void (*description)(P,I,P,char16_t*);
 const char16_t* (*colorText)(uint8_t,const char16_t*); I (*extraEnabled)(); I (*extra)(P,I,char16_t*,U,I*);
 void (*fatal)(U,U,I);
};
static I mul(I a,I b){return I(U(a)*U(b));}
static I add(I a,I b){return I(U(a)+U(b));}
static void draw(Env& e,P file,I frame,I x,I y,I mode=5){Cell c{};c.file=file;c.frame=frame;e.draw(&c,x,y,-1,mode,0);}
// 1.13c D2Client+0x27290
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_27290(State* s,Env* ep,I ebx){
 auto& e=*ep; bool wide=e.resolution()==2;
 auto release=[&](P& p){if(p){e.unload(p);e.freeFile(p,0x6fb86ab0,0x7a,0);p=nullptr;}};
 if(!wide){if(!s->panel)s->panel=e.load(0x6fb86b14);release(s->widePanel);release(s->oldPanel);}
 else {release(s->panel);if(!s->widePanel)s->widePanel=e.load(0x6fb86b24);}
 P file=wide?s->widePanel:s->panel;
 draw(e,file,0,0,add(s->height,-ebx));
 I n=wide?4:3,offset=wide?-0xeb:-0x9b;
 for(I i=0;i<n;++i)draw(e,file,i+1,add(s->width/2,offset+i*128),s->height);
 draw(e,file,n+1,add(s->width,-0x75),add(s->height,-ebx));
}
// 1.13c D2Client+0x277E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_277e0(State* s,Env* ep,P inventory,I edi){
 auto& e=*ep;P cursor=e.cursor(inventory);if(!cursor||!s->hoverEnabled)return;
 uint8_t info[0x10c];e.selectionInfo(s->selectionSource,s->selectionPage,info);
 if(s->selected<0||s->selected>=info[4])return;
 if(!e.selectionBlocked(edi,s->selected)&&e.cursorValid(cursor)){e.select(s->selected,1);return;}
 U out;I mode=e.selectionTest(edi,cursor,s->selected,&out)?3:0;e.select(s->selected,mode);
}
static void bar(State& s,Env& e,bool mana){
 I current=e.current(mana?8:6),maximum=mana?e.maximumMana(s.player):e.maximumLife(s.player);
 if(!maximum)return;if(mana&&current>maximum)current=maximum;
 I fill=mul(current,80)/maximum;
 if(!mana&&fill>0&&fill<3&&s.player&&e.word(s.player,0)==0&&e.word(s.player,0x10)!=0x11)fill=2;
 Cell c{};c.file=s.bars;c.frame=mana?1:0;
 if(e.state(s.player,mana?0x6a:100)){
 I total=mul(e.stat(s.player,mana?0x1a:0x4a,0),80)/100;if(total>80)total=80;
 if(fill<total){I renderer=e.renderer();I x=mana?add(s.width,renderer==6?-0x70:-0x6f):(renderer==6?0x1c:0x1d);
 e.clip(&c,x,add(s.height,-13),fill,add(total,-fill),0);}}
 if(!mana)c.frame=e.state(s.player,2)?2:0;
 if(mana||fill){e.renderer();e.clip(&c,mana?add(s.width,-0x6f):0x1d,add(s.height,-13),0,fill,5);}
 draw(e,s.barFrame,mana?1:0,mana?add(s.width,-0x6e):0x1c,add(s.height,mana?-9:-5));
}
// 1.13c D2Client+0x27A90
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_27a90(State* s,Env* e){bar(*s,*e,true);}
// 1.13c D2Client+0x27BD0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_27bd0(State* s,Env* e){bar(*s,*e,false);}
// 1.13c D2Client+0x27EF0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_27ef0(State* s,Env* ep){
 auto& e=*ep;std::u16string text=e.localized(0x1053);
 for(U side: {1u,0u})for(U i=0;i<s->keyCount;++i){auto& k=s->keys[i];if(k.action!=0x23||k.side!=side)continue;
 if(k.key!=-1){char16_t part[30]{};U name=e.keyName(0x23,side);e.formatKey(part,30,e.localized(0x1052),name);text+=part;}break;}
 e.text(text.c_str(),add(s->width/2,-0x91),add(s->height,-0x17),0,1);
}
// 1.13c D2Client+0x28320
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_28320(State* s,Env* ep){
 auto& e=*ep;I frame=s->buttonPressed?2:0;
 if(s->buttonEnabled&&e.buttonTest(s->mouseX))++frame;
 I x=add(s->width/2,-0x91);draw(e,s->button,frame,x,add(s->height,-10));
 if(x<=s->mouseX&&s->mouseX<=add(s->width/2,-0x80)&&add(s->height,-28)<=s->mouseY&&s->mouseY<=add(s->height,-8))d2_client_27ef0(s,ep);
}
// EAX severity, ECX special, EDX item flag, stack slot.
// 1.13c D2Client+0x28420
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_28420(State* s,Env* ep,I severity,I special,I flag,I slot){
 I frame=mul(severity,11),offset=0;
 switch(slot){case 1:offset=1;break;case 3:offset=4;break;case 4:offset=special?10:(flag?7:6);break;
 case 5:offset=special?9:(flag?5:8);break;case 8:offset=2;break;case 9:offset=3;break;}
 I y=0x6e;if(s->expansion)y=0x8c+(ep->isExpansion()?0x2b:0);
 draw(*ep,s->durability,add(frame,offset),add(s->width,-0x3c),y,1);
}
// 1.13c D2Client+0x28530
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_28530(State* s,Env* ep,I frame){
 I x=add(s->width,-0x37),y=0x9b;
 for(I i=0;i<6;++i)if(!s->occupied[i]){s->occupied[i]=1;x=add(x,-40*i);break;}
 if(s->expansion)y=(ep->isExpansion()?0xc6:0x9b)+30;
 draw(*ep,s->warning,frame,x,y);
}
// 1.13c D2Client+0x285C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_285c0(State* s,Env* e){
 if(!e->isExpansion())return;for(I b:s->blocked)if(b)return;
 if(s->panelOpen&&s->panelMode!=1&&s->panelMode!=2)return;
 draw(*e,s->statusPanel,0,add(s->width,-0x3a),0x2d+(s->expansion?0x46:0),1);
}
// 1.13c D2Client+0x28680
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_28680(State* s,Env* ep){
 auto& e=*ep;P inv=e.pointer(s->player,0x60);bool warning=false;
 for(I i=0;i<7;++i){P item=e.equipped(inv,s->slots[i]);if(!item||!e.durable(item)||e.excluded(item))continue;
 I max=e.maxDurability(item);if(max&&mul(e.stat(item,0x48,0),100)/max<=20){warning=true;break;}}
 if(!warning)return;
 for(I i=0;i<7;++i){I slot=s->slots[i],severity=0,flag=0,special=0;P item=e.equipped(inv,slot);
 if(!item&&(slot==4||slot==5))continue;
 if(item&&e.durable(item)&&!e.excluded(item)){I max=e.maxDurability(item);flag=e.itemFlag(item,0x33);special=e.special(item);
 if(max){I percent=mul(e.stat(item,0x48,0),100)/max;severity=percent<=0?2:(percent<=20?1:0);}}
 d2_client_28420(s,ep,severity,special,flag,slot);}
}
// 1.13c D2Client+0x287C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_287c0(State* s,Env* ep){
 auto& e=*ep;std::memset(s->occupied,0,6);P inv=e.pointer(s->player,0x60);
 P left=e.equipped(inv,4),right=e.equipped(inv,5);bool first=false;
 if(left&&e.ammoStat(left)){if(!right||!e.itemFlag(right,e.ammoStat(left)))first=true;}
 if(!first&&right&&e.ammoStat(right)){if(!left||!e.itemFlag(left,e.ammoStat(right)))first=true;}
 if(first)d2_client_28530(s,ep,2);
 // x86 EBX starts as inventory, becomes the quantity after a matching threshold,
 // and is restored at the end of each outer slot. Preserve that unusual ABI flow.
 for(U i=0;i<s->ammoCount;++i){P item=e.equipped(inv,s->ammoSlots[i]);if(!item)continue;
 U discriminator=e.ammoType(item);if(!discriminator)continue;P firstArg=inv;
 for(U j=0;j<s->thresholdCount;++j){auto& t=s->thresholds[j];if(discriminator!=t.type)continue;
 P record=e.itemRecord(e.word(item,4));U count=U(e.stat(item,0x46,0));
 if(!e.singleStack(item))for(P n=e.first(firstArg);n;n=e.next(n)){P other=e.item(n);if(other==item||e.skip(other))continue;
 P otherRecord=e.itemRecord(other?e.word(other,4):0xffffffff);
 if(e.word(otherRecord,0x80)==e.word(record,0x80))count+=U(e.stat(other,0x46,0));}
 discriminator=e.retailAddress(record); // Retail reuses the discriminator word as a record address.
 firstArg=e.resolveRetail(count); // EBX is a retail scalar until resolved at the call boundary.
 if(count<=t.low)d2_client_28530(s,ep,I(t.frame+2));else if(count<=t.medium)d2_client_28530(s,ep,I(t.frame+1));else if(count<=t.high)d2_client_28530(s,ep,I(t.frame));
 }
 }
}
// 1.13c D2Client+0x289A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_289a0(State* s,Env* ep){
 auto& e=*ep;if(!s->hoverEnabled||!s->hover||e.cursor(e.pointer(s->player,0x60)))return;
 if(s->selected>=4&&!s->hoverA&&!s->hoverB)return;
 if(!e.itemRecord(e.word(s->hover,4))){e.fatal(0x61d,0x6fb8359f,-1);return;}
 char16_t name[128]{},desc[256]{},extra[64]{};
 e.name(s->hover,name,128);if(e.quality(s->hover)!=3)e.description(s->hover,1,nullptr,desc);
 std::u16string text=e.colorText(3,desc);text+=e.colorText(0,name);
 if(s->hoverX<0||s->hoverY<0)return;
 I extraValue{}; // +0x46230 writes through EAX, independently of its text output.
 if(e.extraEnabled()&&e.extra(s->hover,1,extra,64,&extraValue)&&extra[0]){text+=e.localized(0xf9e);text+=extra;}
 e.text(text.c_str(),s->hoverX,s->hoverY,0,1);
}
}

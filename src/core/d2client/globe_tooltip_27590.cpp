// Env-seam: globals and callees come in through G/C. Derived from cached 1.13c decompile.
#include <cstdint>
namespace d2client_globe {
using I=int32_t; using P=void*;
struct G { I mouseY,mouseX,screenW,screenH,forceLife,forceMana; P player; };
struct C {
 I (*f6B20)(I stat); I (*ord10913)(P); I (*ord10186)(P);
 // ord10003 is __fastcall-like: ECX = string index (0x1045 life, 0x1046 mana), stack = (a,b).
 const char16_t* (*ord10003)(I idx,I a,I b); void (*format)(char16_t*,const char16_t*);
 // cat: ECX = dest, EDX = src. ord10028: ECX = buffer, returns its pixel width (EAX).
 // ord10150: ECX = buffer, EDX = x, stack = (y,0,0).
 void (*cat)(char16_t*,const char16_t*); I (*ord10028)(char16_t*); void (*ord10150)(char16_t*,I x,I y,I,I);
};
// 1.13c D2Client+0x27590
void sub_27590(const G& g,const C& c){
 char16_t life[100]{},mana[100]{},tmp[100];
 // Life globe: force flag, or cursor in x (0x1d,0x6f) and y [H-0x4b,H-0xf] (g.screenH = DAT_6fb8bc4c).
 if(g.forceLife||(g.mouseX>0x1d&&g.mouseX<0x6f&&g.screenH-0x4b<=g.mouseY&&g.mouseY<=g.screenH-0xf)){
  I a=c.f6B20(6)>>8,b=c.ord10913(g.player)>>8;
  if(g.player&&*static_cast<I*>(g.player)==0&&static_cast<I*>(g.player)[4]!=0x11&&a>=0&&a<2)a=1;
  c.format(tmp,c.ord10003(0x1045,a,b));c.cat(life,tmp);I w=c.ord10028(life);c.ord10150(life,0x41-((w-(w>>31))>>1),g.screenH-0x5f,0,0);
 }
 // Mana globe: x in [W-0x6f, W-0x1f] (g.screenW = DAT_6fb8bc48).
 if(g.forceMana||(g.screenW-0x6f<=g.mouseX&&g.mouseX<=g.screenW-0x1f&&g.screenH-0x4b<=g.mouseY&&g.mouseY<=g.screenH-0xf)){
  I a=c.f6B20(8)>>8,b=c.ord10186(g.player)>>8;
  c.format(tmp,c.ord10003(0x1046,a,b));c.cat(mana,tmp);I w=c.ord10028(mana);c.ord10150(mana,g.screenW-((w-(w>>31))>>1)-0x50,g.screenH-0x5f,0,0);
 }
}
}

// D2Client implementation batch 49440 (row s3559): the 10-slot list dialog builder.
// Derived from cached 1.13c decompile. Callees outside this function are seams in H; string keys the
// decompiler dropped (Ordinal_10003 arguments) are passed as the call's sequence number S() (UNVERIFIED).
#include <cstdint>
#include <cstring>
#include <cstddef>
namespace d2client_b49440 {
using U=uint32_t; using I=int32_t; using P=uint8_t*; using W=char16_t;
struct Ctl{U w[0x18];};
struct Slot{uint16_t key,_1,_2,_3; U on; U _a;};   // 16-byte records at DAT_6fbc967d
static_assert(sizeof(Slot)==16&&offsetof(Slot,on)==8,"slot");
struct Q{U _0,_1,a,b,_4,_5,d,_7,c;};                    // Ordinal_10458 out (auStack_650): +8,+0xc,+0x20,+0x18
struct G{ P player=nullptr; Ctl*ctl9763=nullptr,*ctl976f=nullptr; void**v977b=nullptr; 
 Slot*slots=nullptr; I scrW=0,scrH=0; U c390=0; };
struct H{
 void (*f3d20)(); void (*f9090)(); void (*f8b20)(); Ctl*(*newWnd)(I,I,U,U,U,U,U,I,I);   // 53d20, 48B20, 53e40
 void (*fatal)(U);                                      // 10265+10024+_exit; 49090
 I (*o10973)(P,I,I); const W*(*str)(U seq); void (*sprintfW)(W*,const W*);
 void (*addLine)(Ctl*,const W*,U,U,U,U,U); void (*f53400)();
 void*(*newList)(I,I,I,I,uint8_t); const W*(*lang)(U);                  // 17f20
 U (*o10988)(U); I (*o10458)(P,U,U,Q*); const W*(*o10091)(I);
 void (*fmt)(W*,const W*,const W*);                                     // 40cb0
 void (*listAdd)(void*,const W*,const W*,U,U,U,U,I,U,U,U);            // vtbl+0x30
};
static G g; static H h;
static void cpy(W*d,const W*s){while((*d++=*s++));}
static void cat(W*d,const W*s){while(*d)++d;cpy(d,s);}
static void ncpy(W*d,const W*s,I n){I i=0;for(;i<n&&s[i];++i)d[i]=s[i];for(;i<n;++i)d[i]=0;}
static void num2(W*d,U v){char t[12];int n=0;do t[n++]=char('0'+v%10),v/=10;while(v);int k=0;
 if(n<2)d[k++]=' ';while(n)d[k++]=W(t[--n]);d[k]=0;}   // wsprintfA "%2u" + win2Unicode
#define S() h.str(seq++)

// 1.13c D2Client+0x49440
U sub_49440(){
 U seq=0;
 if(g.ctl9763){h.f3d20();g.ctl9763=nullptr;}
 if(g.ctl976f){
  h.f3d20(); if(g.v977b)((void(*)())*g.v977b)(); g.v977b=nullptr; g.ctl976f=nullptr; h.f8b20(); return 0;
 }
 g.ctl976f=h.newWnd((g.scrW-0x1ea)/2,g.scrH/2-0xc3,0x6faf9090,0,0,0,0,0x1ea,0x15e);
 if(!g.ctl976f){h.fatal(0);return 0;}
 g.ctl976f->w[0x17]=1;
 W title[300]; I a=h.o10973(g.player,0,0), b=h.o10973(g.player,0xe,0);
 (void)a;(void)b; h.sprintfW(title,S());                       // Unicode::sprintf(300,buf,fmt(a+b))
 h.addLine(g.ctl976f,title,0x15,0,1,0,0);
 h.addLine(g.ctl976f,S(),0x13b,0,1,0x6faf9090,1);
 g.v977b=reinterpret_cast<void**>(h.newList(0,(g.scrW-0x1ea)/2,g.scrH/2-0xa0,0x118,6));
 if(!g.v977b){h.f9090();return 0;}
 h.f53400();
 I used=0;
 for(I i=0;i<10;++i){
  Slot&s=g.slots[i]; if(!s.on)continue;
  Q q; if(h.o10458(g.player,h.o10988(s.key),g.c390,&q)==0)return 0;
  W t[300]={0}, u[300]={0}, f[20]={0}, nb[10]={0};
  cpy(t,S()); cat(t,S()); cat(t,S()); cat(t,S());
  U v[4]={q.a,q.b,q.c,q.d};
  for(int k=0;k<4;++k){
   ncpy(f,S(),0x14); cat(t,f); cat(t,S()); cat(t,S());
   num2(nb,v[k]); cat(t,nb);
   if(k<3){cat(t,S());cat(t,S());}
  }
  cat(u,S()); cat(u,S());
  if(h.o10091(0)){ W x[150]={0}; const W fm[6]={'a','0',':','%','0',0}; h.fmt(x,fm,h.o10091(0)); cat(u,x); }
  h.listAdd(g.v977b,t,u,0,0,0,0,i,0,1,0xe);
  ++used;
 }
 if(!used) h.listAdd(g.v977b,S(),nullptr,0,0,0,0,10,0,1,0xe);
 if(g.ctl976f->w[0x14]>=0x80000000u){h.fatal(0x4f8);return 0;}
 g.ctl976f->w[0x11]=0xffffffffu; return 1;
}
} // namespace d2client_b49440

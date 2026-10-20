// D2Client implementation batch 47D60: dialog/menu state machine (rows s3504-s3516).
// Derived from cached 1.13c decompile. Globals are named by their 1.13c address (G::aXXXX = 0x6fbaXXXX..);
// every callee not in this batch is a seam in H (UNVERIFIED: arguments the decompiler dropped are marked).
#include <cstdint>
#include <cstring>
#include <cstddef>
namespace d2client_b47d60 {
using U=uint32_t; using I=int32_t; using P=uint8_t*;
static U rd(const P p,U o){U v;std::memcpy(&v,p+o,4);return v;}
#pragma pack(push,1)
struct Menu{I id,count;int16_t s[5];U f[5];uint8_t pad;};
#pragma pack(pop)
static_assert(sizeof(Menu)==0x27,"menu stride");
static_assert(offsetof(Menu,s)==8&&offsetof(Menu,f)==0x12,"menu layout");
enum:U{H6D10=0x6faf6d10,H9440=0x6faf9440,H8380=0x6faf8380,H8350=0x6faf8350,H8290=0x6faf8290,H82F0=0x6faf82f0,H81C0=0x6faf81c0,H9250=0x6faf9250,H8650=0x6faf8650};
struct Ctl{U w[0x18];};
struct G{
 P player=nullptr;                 // DAT_6fbcbbfc
 I bc90=0,bc34=0,a97a8=0,c334=0,v23e8=0,b834=0,c854=0,a3b10=0,a91590=0,a91c3c=0;
 U a9777=0,v973b=0,v971d=0,c390=0,v9747=0,v9743=0,d22fd=0,d22f9=0,t2309=0,t2301=0,d2305=0;
 U v9725=0,c2f4=0,c2f8=0,d22f8=0,v97b4=0,v97b8=0,v97bc=0,v974f=0,v974b=0,c394=0;
 U a9675=0,a9679=0,cbf94=0,cbf98=0,c404=0,c3a4=0,a8c9c=0;
 uint8_t v9729=0,v9663=0,v9661=0,v9662=0,v9648=0;
 Ctl*ctl9763=nullptr,*ctl9767=nullptr,*ctl976b=nullptr,*ctl976f=nullptr,*ctl9773=nullptr;
 U v975b=0,v975f=0,v9757=0; void**v977b=nullptr;
 Menu*tbl=nullptr; I menuCount=0;  // 6fba3ba0, 6fba42f0
 P tab37a8=nullptr;                // 6fba37a8 stride 0x16
 P tab9675rec=nullptr;             // *DAT_6fbc9675 record (+5 ptr,+9 cnt,+0xd,+0x11)
 P tab91400=nullptr;
};
struct H{
 I* (*cur)();                                      // 6150
 U (*o10071)(I,I,I,U,U,I); U (*o10107)(P,P,U,U,I,I); I (*o10705)(P); I (*o11088)(P); I (*o11017)(U);
 U (*tick)(); I (*f5b40)(); I (*f5bd0)(); void (*f6ee0)(I,uint8_t); void (*f38a70)(I,I,I,I); void (*f7500)();
 void (*f67c0)(); void (*f8150)(); void (*f2ad0)(); void (*f3840)(); void (*f42850)(); void (*f3d20)(); void (*f4660)();
 void (*f7920)(); U (*r1380)(P); void (*f6880)(); void (*f7b20)(); void (*f6e00)(); void (*f2d3c0)(I*,int16_t);
 Ctl* (*newCtl)(U,U); void (*addLine)(Ctl*,const char16_t*,I,I,I,U,I); void (*ctlOpen)(Ctl*);
 const char16_t* (*str)(U key);                    // Ordinal_10003; key UNVERIFIED (dropped by decompiler)
 const char16_t* (*title)(); void (*fatal)(U);     // 55d90; Ordinal_10265+10024+_exit
 void (*send)(const uint8_t*,I);                   // 43e0
 I (*o10174)(U,I,I); I (*o10973)(P,I,I); I (*o10913)(P); I (*o10504)(P); I (*o10640)(P);
 I (*o10720)(); U (*o10109)(I,I); I (*rowOf)(U,I,I); I (*o10460)(U); I (*o10464)(I); I (*o11147)(I); U (*o10802)(I);
 U (*lower)(U); I (*f5c00)(); void (*f72790)(I,I,I); void (*f3ca20)(); void (*f4fd70)(); I (*o10227)();
 void (*unitFlag)(P,I);                            // 7b20 side
 P (*unitLookup)(U);                               // slot (&DAT_6fbba808)[id&0x7f]
};
static G g; static H h;
static void cpy(char16_t*d,const char16_t*s){while((*d++=*s++));}
static void cat(char16_t*d,const char16_t*s){while(*d)++d;cpy(d,s);}
static const char16_t* num(char16_t*b,I v){char t[12];int n=0;U u=v<0?-U(v):U(v);do t[n++]=char('0'+u%10),u/=10;while(u);int k=0;if(v<0)b[k++]='-';while(n)b[k++]=t[--n];b[k]=0;return b;}

// 1.13c D2Client+0x47D60
U sub_47D60(P self,I p1,U p3,I p4,I p6,U eax,uint8_t bp){
 I*c=h.cur(); if(!c)return 0; I id=c[1];
 if(p6){
  bool ok=id<0xfe?(id==0xfd||id==0x9a||id==0xb2):(id==0x101||id==0x1ff);
  if(!ok)return 0;
  if(!g.bc90)return 1;
  g.a9777=h.o10071(0,p1,id,g.c390,g.v973b,0); g.v9729=3; g.d22f8=1; g.d22fd=0; g.c2f8=1; g.c2f4=0;
  g.v9747=p3; g.v9743=eax; h.f6ee0(1,bp); return 1;
 }
 if(h.o11088(self)==4)return 0;
 if(p4&&h.tick()-g.t2309<500)return 0;
 g.v9747=p3; U mode=0; bool toFinish=false, step=false, via9a=false, viaF4=false, via93=false;
 if(p1==1){
  if(id<0xfe){
   if(id==0xfd)via9a=true; else switch(id){
    case 0x93:case 0x94:case 0xb1:case 199:case 0xca:case 0xfc:via93=true;break;
    case 0x9a:case 0xb2:via9a=true;break;
    case 0xf4:case 0xf5:case 0xf6:viaF4=true;break;
    default:g.v9743=eax;return 0;}
  }else if(id>0x195){
   switch(id){case 0x1ff:via9a=true;break;case 0x200:case 0x201:case 0x202:via93=true;break;case 0x208:viaF4=true;break;default:g.v9743=eax;return 0;}
  }else if(id!=0x195){
   switch(id){case 0xfe:case 0xff:via93=true;break;case 0x101:via9a=true;break;case 0x109:viaF4=true;break;default:g.v9743=eax;return 0;}
  }else via93=true;
  if(viaF4){g.v9743=eax;if(h.f5bd0()==1)return 0;g.v9729=4;step=true;}
  else if(via9a&&g.bc34==4){g.v9743=eax;if(!h.f5b40())return 0;g.v9729=3;mode=3;toFinish=true;}
  else{g.v9743=eax;g.a9777=h.o10107(g.player,self,g.c390,g.v973b,id,1);if(!h.o10705(self))return 0;g.v9729=2;step=true;}
 }else{
  g.v9743=eax; if(h.o11017(rd(g.player,0x60)))return 0;
  g.v9729=1; mode=g.a97a8?2:0; toFinish=true;
 }
 if(toFinish)g.a9777=h.o10107(g.player,self,g.c390,g.v973b,id,mode);
 (void)step;
 g.d22f8=1;
 if(!self){g.d22fd=0xffffffff;g.d22f9=0xffffffff;}else{g.d22fd=rd(self,0xc);g.d22f9=rd(self,4);}
 g.c2f8=1; g.c2f4=0;
 if((g.v9729==2||g.v9729==1)&&p4){h.f6ee0(1,uint8_t(p3));return 1;}
 h.f38a70(0,0,0,0); g.v9663=4; h.f7500(); return 1;
}

// 1.13c D2Client+0x483B0
void sub_483B0(){
 I*c;
 if(!g.player||rd(g.player,0x10)==0x11||((c=h.cur())&&(c=h.cur())&&c[4]==0xc)||!h.cur()){h.f67c0();h.f8150();}
 if(!g.ctl9767&&!g.ctl976b){
  if(g.ctl9763){h.f2ad0();h.f3840();return;}
  if(g.v975b)goto end;
  if(!g.v975f){
   if(g.v9757){
    if(!h.o11017(rd(g.player,0x60))&&!g.b834){h.f3840();return;}
    g.v9663=3;h.f42850();g.t2301=h.tick();g.d2305++;h.f3d20();g.v9757=0;return;
   }
   if(g.ctl976f||g.ctl9773)goto end;
  }else{
   if((c=h.cur())&&(c=h.cur())&&c[1]==0x202&&!g.v97b4){
    if(!g.v97b8){g.v97b8=1;g.v97bc=(h.r1380(g.player+0x20)+0x78)*1000+h.tick();}
    if(h.tick()<g.v97bc){g.v97b4=1;h.f38a70(0,0,0,0);}
   }
   if(g.c394&&g.v974f&&g.v974f<h.tick()){h.f7920();return;}
   if(g.v974b<h.tick()){h.f4660();g.a8c9c=0;g.c404=1;g.c3a4=1;}
  }
  return;
 }
end: h.f3840();
}

// 1.13c D2Client+0x48590
void sub_48590(uint8_t flag,I eax){
 if(!h.cur()){h.f8150();return;}
 g.v9661=1;g.cbf94=0x6faf6e60;g.cbf98=0;h.f3d20();g.ctl9767=nullptr;
 g.tab9675rec[0x11]=1;
 if(!g.v9662){h.f6e00(); if(flag)eax=I(rd(g.tab9675rec,0xd));}
 int16_t s; U base=rd(g.tab9675rec,5); I cnt=I(rd(g.tab9675rec,9));
 std::memcpy(&s,reinterpret_cast<P>(uintptr_t(base))+(eax%cnt)*0xf,2);
 h.f2d3c0(h.cur(),s);
}

// 1.13c D2Client+0x486A0  (id arrives in EAX)
void sub_486A0(U id,void (*node)(U,P&)){
 h.f7b20();
 P p=h.unitLookup(id&0x7f);
 while(p){
  if(rd(p,0xc)==id){
   if(rd(p,0)!=1){h.fatal(0x5a);return;}
   U f=rd(p,0xc4)|2;std::memcpy(p+0xc4,&f,4);return;
  }
  p=reinterpret_cast<P>(uintptr_t(rd(p,0xe4)));
 }
 (void)node;
}

static void clearBuf(char16_t*b,int n){std::memset(b,0,n*2);}

// 1.13c D2Client+0x48930
void sub_48930(){
 if(!h.cur()){h.f8150();return;}
 if(g.ctl9773){h.fatal(0xd69);return;}
 h.f6880();
 g.ctl9773=h.newCtl(H82F0,H8290);
 if(!g.ctl9773){h.fatal(0x529);return;}
 g.ctl9773->w[0x17]=1;
 char16_t b[160];clearBuf(b,160);
 h.cur(); cpy(b,h.title());
 int n=0;while(b[n])++n;
 for(int i=0;i<n;++i)if(b[i]<0x100)b[i]=char16_t(h.lower(b[i]));
 h.addLine(g.ctl9773,b,0x15,0,1,0,0);
 g.v9663=1;
 // 64-bit seed update at player+0x20: lo*0x6ac690c5 + hi
 U lo=rd(g.player,0x20),hi=rd(g.player,0x24);uint64_t s=uint64_t(lo)*0x6ac690c5u+hi;std::memcpy(g.player+0x20,&s,8);
 h.addLine(g.ctl9773,h.str(0),0xf,0,1,H81C0,1);
 h.addLine(g.ctl9773,h.str(1),0xf,0,1,H82F0,1);
 h.ctlOpen(g.ctl9773);
}

// 1.13c D2Client+0x48B20
void sub_48B20(){
 if(g.ctl9763){h.fatal(0xc74);return;}
 I*c=h.cur(); if(!c){h.f8150();return;}
 if(g.v9725==0xfc||g.v9725==0xc6||g.v9725==0x203||g.v9725==0x96){
  uint8_t pk[13]={0x38,3,0,0,0};U pid=g.player?rd(g.player,0xc):0xffffffff;
  std::memcpy(pk+5,&g.v971d,4);std::memcpy(pk+9,&pid,4);h.send(pk,13);
 }
 h.f6880();
 g.ctl9763=h.newCtl(H8380,H8350);
 if(!g.ctl9763){h.fatal(0x529);return;}
 g.ctl9763->w[0x17]=1;
 char16_t b[160];clearBuf(b,160);h.cur();cpy(b,h.title());
 h.addLine(g.ctl9763,b,0x15,0,1,0,0);
 c=h.cur(); if(!c)return;
 I idx=0;for(I i=0;i<g.menuCount;++i,++idx)if(g.tbl[i].id==c[1])break;
 if(idx>=g.menuCount){ if(g.menuCount>0){}else return; }
 if(idx>=g.menuCount)idx=0;
 if(!(idx<g.menuCount))return;
 Menu&m=g.tbl[idx];
 g.v9663=1;
 if(m.count!=1&&m.count-1>=0){
  for(I i=0;i<m.count-1;++i){
   char16_t t[1024];clearBuf(t,1024);I show=1;int16_t id=m.s[i];bool emit=false;
   if(id==0x1507){
    const char16_t*k=h.str(0); if(int16_t(g.v23e8)!=0x421)k=h.str(1);
    (void)k; cpy(t,h.str(2)); emit=true; // sprintf(t,fmt(k,c334)): format args UNVERIFIED
   }else if(id==0xd09){
    I a=h.o10973(g.player,6,0),b2=h.o10913(g.player);
    if(b2<=a)continue;
    I n=h.o10504(g.player);if(!n)n=1;
    char16_t nb[10];cpy(t,h.str(3));cat(t,num(nb,n));if(!n)show=0;emit=true;
   }else if(id==0xfb4){
    bool none=h.o10174(g.v973b,4,0)==0&&h.o10174(g.v973b,4,1)==0;
    I v=h.o10640(g.player);
    if(v){m.count=3;char16_t nb[10];cpy(t,h.str(4));if(none)cat(t,num(nb,v*100));emit=true;}
    else m.count=2;
   }else if(id!=0x2ba0||(h.o10174(g.v973b,0x29,0)==0&&(h.o10174(g.v973b,0x29,1)==1||(g.c390==2&&h.o10174(g.v973b,0x29,1)==0)))){
    cpy(t,h.str(5));emit=true;
   }
   if(emit)h.addLine(g.ctl9763,t,0xf,0,1,m.f[i],show);
  }
 }
 h.addLine(g.ctl9763,h.str(6),0xf,0,1,H8380,1);
}

// 1.13c D2Client+0x49090
U sub_49090(){
 if(g.ctl976f){h.f3d20();if(g.v977b)((void(*)(I))*g.v977b)(1);g.v977b=nullptr;g.ctl976f=nullptr;}
 sub_48B20();return 1;
}

// 1.13c D2Client+0x492F0
void sub_492F0(){
 uint8_t s=g.v9663;
 if(s!=1){
  if(s>2&&s<7){h.f72790(0xc,1,0);h.f8150();h.f3ca20();h.f67c0();g.v9663=0;return;}
  if(s==7){h.f4fd70();h.f67c0();g.v9663=0;return;}
  if(!g.v9661){h.f67c0();g.v9663=0;return;}
 }
 g.v9661=0;h.f8150();h.f67c0();g.v9663=0;
}

// 1.13c D2Client+0x49D80  (menu index in EAX)
void sub_49D80(I idx){
 if(g.ctl9767){h.fatal(0x76f);return;}
 h.f6880();
 g.ctl9767=h.newCtl(H9250,H8650);
 if(!g.ctl9767){h.fatal(0x529);return;}
 g.ctl9767->w[0x17]=1;
 h.addLine(g.ctl9767,h.str(0),0x15,0,1,0,0);
 I inv=g.a3b10; if(inv)
 {
  I n=h.o10720(); g.a9679=U(idx);
  if(g.tab37a8[idx*0x16+0x14]==0)h.addLine(g.ctl9767,h.str(1),0xf,0,1,0x6faf9130,1);
  h.addLine(g.ctl9767,h.str(2),0xf,0,1,0x6faf91e0,inv);
  for(I i=0;i<n;++i){
   U s=h.o10109(inv,i); I row=h.rowOf(s,s<U(uint16_t(g.a91590))?0:100,s<U(uint16_t(g.a91590))?100:g.a91c3c);
   h.addLine(g.ctl9767,h.str(U(row)),0xf,0,1,0x6faf73b0,1);
  }
  if(g.v9725==0xc9)h.addLine(g.ctl9767,h.str(3),0xf,0,1,0x6faf7360,1);
  if((g.v9725==0x109||g.v9725==0xf4||g.v9725==0xf5||g.v9725==0x208||g.v9725==0xf6)&&g.player&&rd(g.player,0x60)){
   for(I u=h.o10460(rd(g.player,0x60));u;u=h.o10464(u)){
    I it=h.o11147(u);
    if(it&&h.o10802(it)==0x20786f62){h.addLine(g.ctl9767,h.str(4),0xf,0,1,0x6faf7310,u);break;}
   }
  }
 }
 h.addLine(g.ctl9767,h.str(5),0xf,0,1,H9250,1);
}

// 1.13c D2Client+0x4A060  (menu index in EAX)
U sub_4A060(I e,I add){
 if(!h.o10227()||!g.c854)return 0;
 Menu&m=g.tbl[e]; I i=0;
 if(!add){
  for(;i<m.count;++i)if(m.f[i]==H6D10){
   for(;i<m.count-1;++i){m.f[i]=m.f[i+1];m.s[i]=m.s[i+1];}
   m.count--;return 1;
  }
  return 1;
 }
 I n=m.count;
 for(;i<n;++i)if(m.f[i]==H6D10)return 1;
 for(i=0;i<n;++i)if(m.f[i]==H9440){
  I k=n-1;
  for(;k>=i;--k){m.f[k+1]=m.f[k];m.s[k+1]=m.s[k];}  // ponytail: fixed 5-slot table, as retail
  m.count++;m.f[i]=H6D10;m.s[i]=0x1507;return 1;
 }
 I k=n-1;m.f[k+1]=m.f[k];m.s[k+1]=m.s[k];m.count++;m.f[k]=H6D10;m.s[k]=0x1507;return 1;
}

// 1.13c D2Client+0x4A210  (writes menu entry 11, slot 1)
U sub_4A210(){Menu&m=g.tbl[11];m.count=3;m.f[1]=H9440;m.s[1]=int16_t(0xd45);return 1;}

// 1.13c D2Client+0x4A270
U sub_4A270(){
 g.v9663=1; I i=h.f5c00(); g.a9675=U(i); sub_49D80(i); return 1;
}
}

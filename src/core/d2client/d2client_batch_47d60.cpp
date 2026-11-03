// D2Client implementation batch 47D60: dialog/menu state machine (rows s3504-s3516); structs, g and h are in d2client_b47d60.h.
#include "d2client_b47d60.h"
namespace d2client_b47d60 {
static void cpy(char16_t*d,const char16_t*s){while((*d++=*s++));}
static void cat(char16_t*d,const char16_t*s){while(*d)++d;cpy(d,s);}
static const char16_t* num(char16_t*b,I v){char t[12];int n=0;U u=v<0?-U(v):U(v);do t[n++]=char('0'+u%10),u/=10;while(u);int k=0;if(v<0)b[k++]='-';while(n)b[k++]=t[--n];b[k]=0;return b;}

// 1.13c D2Client+0x47D60  (thiscall ECX=self, EAX=eax, stack s0 p1 p3 bp p4 p6; ret 0x18; s0 is the 2nd argument of Ordinal_10071, bp the 2nd of 0x46EE0)
U sub_47D60(P self,U s0,I p1,U p3,I p4,I p6,U eax,U bp){
 I*c=h.cur(); if(!c)return 0; I id=c[1];
 if(p6){
  bool ok=id<0xfe?(id==0xfd||id==0x9a||id==0xb2):(id==0x101||id==0x1ff);
  if(!ok)return 0;
  if(!g.bc90)return 1;
  g.a9777=h.o10071(0,s0,id,g.c390,g.v973b,0); g.v9729=3; g.d22f8=1; g.d22fd=0; g.c2f8=1; g.c2f4=0;
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
  if(viaF4){g.v9743=eax;if(h.f5bd0(self)==1)return 0;g.v9729=4;step=true;}
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
 if((g.v9729==2||g.v9729==1)&&p4){h.f6ee0(1,bp);return 1;}
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

// 1.13c D2Client+0x48930 (from the disassembly: ctor 11 words, 536F0 EAX 4 for the title, key 0xD3A+(seed%5), 0x102E)
void sub_48930(){
 if(!h.cur()){h.f8150();return;}
 if(g.ctl9773){h.fatal(0xd69);return;}
 U xy[2]={0,0}; h.f6880o(xy);
 const U w[11]={xy[0],xy[1],H82F0,H8290,0,0,1,0,0,0,1};
 g.ctl9773=h.ctor(w);
 if(!g.ctl9773){h.fatal(0x529);return;}
 g.ctl9773->w[0x17]=1;
 char16_t b[80];clearBuf(b,80);
 h.cur(); cpy(b,h.title());
 int n=0;while(b[n])++n;
 for(int i=0;i<n;++i)if(b[i]<0x100)b[i]=char16_t(h.lower(b[i]));
 h.line(4,g.ctl9773,b,0x15,0,1,0,0);
 g.v9663=1;
 // 64-bit seed update at player+0x20: lo*0x6ac690c5 + hi
 U lo=rd(g.player,0x20),hi=rd(g.player,0x24);uint64_t s=uint64_t(lo)*0x6ac690c5u+hi;std::memcpy(g.player+0x20,&s,8);
 static const uint16_t K[5]={0xd3a,0xd3b,0xd3c,0xd3d,0xd3e};
 h.line(0,g.ctl9773,h.str(K[U(s)%5]),0xf,0,1,H81C0,1);
 h.line(0,g.ctl9773,h.str(0x102e),0xf,0,1,H82F0,1);
 h.ctlOpen(g.ctl9773);
}

// 1.13c D2Client+0x48B20 (from the disassembly)
static void toUni9(char16_t*t,I v){char nb[12];int n=0;U u=v<0?-U(v):U(v);char r[12];do r[n++]=char('0'+u%10),u/=10;while(u);int k=0;if(v<0)nb[k++]='-';while(n)nb[k++]=r[--n];nb[k]=0;
 char16_t*d=t;while(*d)++d;for(int i=0;i<9&&nb[i];++i)*d++=char16_t(uint8_t(nb[i]));*d=0;}
void sub_48B20(){
 if(g.ctl9763){h.fatal(0xc74);return;}
 I*c=h.cur(); if(!c){h.f8150();return;}
 if(g.v9725==0xfc||g.v9725==0xc6||g.v9725==0x203||g.v9725==0x96){
  uint8_t pk[13]={0x38,3,0,0,0};U pid=g.player?rd(g.player,0xc):0xffffffff;
  std::memcpy(pk+5,&g.v971d,4);std::memcpy(pk+9,&pid,4);h.send(pk,13);
 }
 U xy[2]={0,0}; h.f6880o(xy);
 const U w[11]={xy[0],xy[1],H8380,H8350,0,0,1,0,0,0,1};
 g.ctl9763=h.ctor(w);
 if(!g.ctl9763){h.fatal(0x529);return;}
 g.ctl9763->w[0x17]=1;
 char16_t b[80];clearBuf(b,80);h.cur();cpy(b,h.title());
 h.line(4,g.ctl9763,b,0x15,0,1,0,0);
 c=h.cur(); if(!c)return;
 I cnt=g.menuCount,idx=0;
 for(I i=0;i<cnt;++i)if(g.tbl[i].id==c[1]){idx=i;break;}
 if(idx>=cnt)return;
 Menu&m=g.tbl[idx];
 g.v9663=1;
 I i=0;
 if(m.count-1>0){
  do{
   char16_t t[0x200];clearBuf(t,0x200);I show=1;uint16_t id=uint16_t(m.s[i]);bool emit=true;
   if(id==0x1507){
    const char16_t*k=h.str(0x2b0d); if(uint16_t(g.v23e8)!=0x421)k=h.str(uint16_t(g.v23e8));
    h.usnprintf(t,0x200,h.str(0x58a8),k,U(g.c334));
   }else if(id==0xd09){
    I a=h.o10973(g.player,6,0),b2=h.o10913(g.player);
    if(a>=b2)emit=false;
    else{
     I n=h.o10504(g.player);if(!n)n=1;
     cpy(t,h.str(id));toUni9(t,n);
    }
   }else if(id==0xfb4){
    bool none=h.o10174(g.v973b,4,0)==0&&h.o10174(g.v973b,4,1)==0;
    I v=h.o10640(g.player);
    if(!v){m.count=2;emit=false;}
    else{
     m.count=3;
     if(!none)cpy(t,h.str(0xfb4));
     else{cpy(t,h.str(0xfb5));toUni9(t,v*100);}
    }
   }else if(id==0x2ba0){
    if(h.o10174(g.v973b,0x29,0)!=0)emit=false;
    else if(h.o10174(g.v973b,0x29,1)==1)cpy(t,h.str(id));
    else if((g.c390&0xff)!=2)emit=false;
    else if(h.o10174(g.v973b,0x29,1)!=0)emit=false;
    else cpy(t,h.str(id));
   }else cpy(t,h.str(id));
   if(emit)h.line(0,g.ctl9763,t,0xf,0,1,m.f[i],U(show));
   ++i;
  }while(i<m.count-1);
 }
 h.line(0,g.ctl9763,h.str(0x102e),0xf,0,1,H8380,1);
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

// 1.13c D2Client+0x49D80  (menu index in EAX; stack arg 0xd35 is the title string key, its only caller 4A270 passes it)
// h.rowOf(s,lo,hi) must return the string key: the u16 at 6fb91402+4*k for the first k in [lo,hi) with u16 6fb91400+4*k == s, else 0xe8c.
void sub_49D80(I idx){
 if(g.ctl9767){h.fatal(0x76f);return;}
 h.f6880();
 g.ctl9767=h.newCtl(H9250,H8650);
 if(!g.ctl9767){h.fatal(0x529);return;}
 g.ctl9767->w[0x17]=1;
 h.addLine(g.ctl9767,h.str(0xd35),0x15,0,1,0,0);
 I inv=g.a3b10; if(inv)
 {
  I n=h.o10720(); g.a9679=U(idx);
  if(g.tab37a8[idx*0x16+0x14]==0)h.addLine(g.ctl9767,h.str(0xd47),0xf,0,1,0x6faf9130,1);
  h.addLine(g.ctl9767,h.str(0xd43),0xf,0,1,0x6faf91e0,inv);
  for(I i=0;i<n;++i){
   U s=h.o10109(inv,i); I row=h.rowOf(s,s<U(uint16_t(g.a91590))?0:100,s<U(uint16_t(g.a91590))?100:g.a91c3c);
   h.addLine(g.ctl9767,h.str(U(row)),0xf,0,1,0x6faf73b0,1);
  }
  if(g.v9725==0xc9)h.addLine(g.ctl9767,h.str(0xd40),0xf,0,1,0x6faf7360,1);
  if((g.v9725==0x109||g.v9725==0xf4||g.v9725==0xf5||g.v9725==0x208||g.v9725==0xf6)&&g.player&&rd(g.player,0x60)){
   for(I u=h.o10460(rd(g.player,0x60));u;u=h.o10464(u)){
    I it=h.o11147(u);
    if(it&&h.o10802(it)==0x20786f62){h.addLine(g.ctl9767,h.str(0x8b7),0xf,0,1,0x6faf7310,u);break;}
   }
  }
 }
 h.addLine(g.ctl9767,h.str(0xd48),0xf,0,1,H9250,1);
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
 g.v9663=1; I i=h.f5c00(); g.a9675=0x6fba37a8u+U(i)*0x16u; if(g.tab37a8)g.tab9675rec=g.tab37a8+i*0x16;  // retail: DAT_6fbc9675 = &DAT_6fba37a8 + idx*0x16 (a record pointer, not the index)
 sub_49D80(i); return 1;
}
}

// Batch 41690: small D2Client helpers, derived from cached 1.13c decompile (D2MOO has no D2Client).
// Globals live in G (RVA noted); external calls are hooks. Uncertain register inputs marked REG?.
#include <cstdint>
namespace d2client_b41690 {
using U=uint32_t; using I=int32_t; using P=void*;
struct G {
 U tick;            // 6fbc97ec
 U lastTick;        // 6fbc9818
 U cntA,cntB;       // 6fbc980c, 6fbc9814
 U avgA,avgB;       // 6fbc9808, 6fbc9810
 I a8c9c,cc404,cc3a4,cc3a0,cc39c,cc3d0,cc388,cc3b0; // misc flags
 I cc3cc,v9898,v9890,v988c,v46d4;
 U cc390; uint8_t* player; // 6fbcc390, 6fbcb980
 I cc394,cc3a8,cc3f0,cc3d4,cc3d8; U v9884,cc37c; I v9938;
 I (*sysMetrics)(I); I (*isKeyDef)(I,I); I (*asyncKey)(U); // GetSystemMetrics, FUN_6fae98c0, GetAsyncKeyState
 struct Bind{I key,a;I b;} ; // unused layout note
 uint8_t* keyTab;   // 6fbca2e4, 0x5a? entries of 10 bytes: u16 vk@0, i32 id@+2, i32 on@+6... id@-6 relative to +2 slot
 void (*cmd)(I,I); void (*hook4660)(U); void (*e350)(I,I); U (*tickNow)(); I (*ord10004)();
};
static U rdu(const uint8_t*p){U v=0;for(int i=3;i>=0;--i)v=v<<8|p[i];return v;}

// 1.13c D2Client+0x41690 (REG?: param_1=ecx unit, param_2=edx; uStack_8/unaff_retaddr are stack garbage in decompile)
struct C41690{ I (*f40550)(P); void (*fa0b0)(); P (*o10392)(U); I (*f40450)(); I (*o10786)(P,U,I,U);
 U (*f1630)(I*); U (*f1660)(I*); void (*f41000)(P,U,U,U,U,I); };
bool sub_41690(P unit,I idx,const uint8_t* recs,I nrec,C41690& c,U r1,U r2){
 if(idx<0||idx>=nrec) return false;
 const uint8_t* rec=recs+idx*0x23c; // sgptDataTables+0xb98 / count +0xba0
 if(c.f40550(unit)<0) return false;
 c.fa0b0();
 U h=rdu(static_cast<uint8_t*>(unit)+0x2c);
 if(!h) return false; // retail: fatal error + _exit(-1)
 I* o=static_cast<I*>(c.o10392(h)); if(!o) return false;
 I m=c.f40450(); if(m<1) m=c.o10786(unit,rdu(rec+100),idx,r1);
 c.f41000(unit,r2,c.f1630(o),c.f1660(o),r1,m); return true;
}
// 1.13c D2Client+0x420E0
bool sub_420E0(G&g){ return g.sysMetrics(0x4b)!=0 && g.cc3cc!=0 && g.v9898==1 && g.v9890==0; }
// 1.13c D2Client+0x423A0
void sub_423A0(G&g){ if(U(g.tick-g.lastTick)>3000){ g.avgA=g.cntA/3; g.avgB=g.cntB/3; g.lastTick=g.tick; g.cntA=0; g.cntB=0; } }
// 1.13c D2Client+0x42850 (REG?: ecx forwarded to 14660)
void sub_42850(G&g,U ecx){ g.a8c9c=0; g.cc404=1; g.hook4660(ecx); g.cc3a4=1; }
// 1.13c D2Client+0x429C0 ; emit(k) = Ordinal_10048 + FUN_6faecc60 with string k (0..3 = 4748,4754,476c,4784)
void sub_429C0(G&g,void(*emit)(I)){
 bool m=g.sysMetrics(0x4b)!=0;
 if(m){
  if(!g.cc3cc) goto tail;
  if(g.v9898==1){ if(g.v9890) emit(0); }
  else if(g.v9898==2&&(g.v988c==4||g.v988c==5)) emit(0);
 }
 if(g.cc3cc){
  if(g.v9898==1||(g.v9898==2&&(g.v988c==4||g.v988c==5))) emit(1);
  if(g.cc3cc&&g.v9898!=1&&g.v9898==2&&g.v988c!=4&&g.v988c==5) emit(2);
 }
 tail:
 if(g.sysMetrics(0x4b)!=0&&g.cc3cc&&g.v9898==1&&g.v9890==0&&g.v46d4!=0) emit(3);
}
// 1.13c D2Client+0x42BD0
void sub_42BD0(G&g){ g.cc388=1; uint16_t*p=reinterpret_cast<uint16_t*>(g.player+0x1eb);
 *p=uint16_t(((g.cc390+1)*0x400)|(*p&0xe0ff)); }
// 1.13c D2Client+0x42C10 (REG?: param_2 in edx/stack; param_1 unused). textW = width of Ordinal_10003 string via Ordinal_10028
I sub_42C10(I param_2,I textWidth){ return param_2-textWidth/2; }
// 1.13c D2Client+0x42C70 : builds name+title strings and calls Ordinal_10150 twice; pure UI text, ordinals as hooks.
struct C42C70{ const char* (*o10205)(uint8_t,I,U,U); void (*show)(const char*,U,I,I); };
void sub_42C70(G&g,const uint8_t* uidata,uint8_t b991c,I v9854,C42C70& c){
 const uint8_t* p=g.player; U f=*reinterpret_cast<const uint16_t*>(p+0x1eb);
 U a=f&4, fl=f&0x20; I k;
 if(g.cc390==0){k=4+(fl!=0); if(!a)a=0;}
 else if(g.cc390==1){k=8+(fl!=0)*2; if(!a)a=0;}
 else if(g.cc390==2){k=12+(fl?3:0); if(!a)a=0;}
 else return;
 const char* s=c.o10205(b991c,k,a,fl); if(!s) return;
 c.show(s,0,v9854?0x165:0x115,4); (void)uidata; // first call (0xbb) shows name at p+0xb9
}
// 1.13c D2Client+0x431B0 : server-thread loop; REG? none. Threading/critsec left to platform; not ported.
// 1.13c D2Client+0x43310 (REG?: ecx = key id passed to fae98c0)
void sub_43310(G&g,U ecx){
 // scan 10-byte binding table [0x6fbca2e4, 0x6fbca75a): {u16 vk; i32 id@+2 (0x24); i32 mode@+6}
 const I N=(0x6fbca75a-0x6fbca2e4)/10;
 if(g.isKeyDef(ecx,0x24)){
  for(I i=0;i<N;++i){ const uint8_t*e=g.keyTab+i*10; if(I(rdu(e+2))==0x24&&I(rdu(e+6))==1){
   U vk=e[0]|e[1]<<8; if(vk!=0xffff&&g.asyncKey(vk)<0){g.cc3f0=1;return;} break; } }
 }
 if(g.isKeyDef(ecx,0x24)){
  for(I i=0;i<N;++i){ const uint8_t*e=g.keyTab+i*10; if(I(rdu(e+2))==0x24&&I(rdu(e+6))==0){
   U vk=e[0]|e[1]<<8; if(vk!=0xffff&&g.asyncKey(vk)<0){g.cc3f0=1;return;} break; }
   if(i==N-1){g.cc3f0=0;return;} }
 }
 g.cc3f0=0;
}
// 1.13c D2Client+0x43550 (REG?: eax = value; clamped to 9 if >0x1c)
void sub_43550(G&g,I eax){
 if(g.cc3d0==0&&g.cc388==0&&g.cc3b0==0){
  g.v9938=eax>0x1c?9:eax; g.e350(1,0);
  g.cc3d0=600; g.cc3a4=1; g.cc3a0=0; g.cc39c=0; g.a8c9c=0;
 }
}
// 1.13c D2Client+0x435D0 (REG?: eax passed through to 43550 is whatever caller left; pass as eax)
void sub_435D0(G&g,I eax){
 if(g.cc394!=0&&g.cc394!=1){
  U now=g.tickNow();
  if(g.ord10004()==0&&g.cc3a4==0&&g.cc3a0!=0) sub_43550(g,eax);
  if(g.cc3a8!=0&&now-g.v9884>180000) sub_43550(g,eax);
  if(now-g.cc37c>30000&&g.cc3a0==0&&g.cc3a4==0&&g.cc3a8==0) sub_43550(g,eax);
  if(g.cc39c==0&&g.cc3a0==0&&g.cc3a4==0&&g.cc3a8==0&&(g.cc394==9||g.cc394==7)) sub_43550(g,eax);
 }
}
}

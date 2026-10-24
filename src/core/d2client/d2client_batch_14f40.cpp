// Implementation batch (alpha lane): twelve D2Client functions from the cached 1.13c decompile.
// Globals and ordinals are reached through the Host table; pointer fields via H.ptr (wasm64 handles).
// Unproved: no spec/vector yet. Stack-artifact operands in 6A3D0/6A550/6A860/6A6B0 are marked ponytail.
#include <cstdint>
#include <cstring>
namespace d2client_b14f40 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(const void* p,U n){U v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,4);return v;}
static I ri(const void* p,U n){return I(rd(p,n));}
static uint16_t rw(const void* p,U n){uint16_t v;std::memcpy(&v,static_cast<const uint8_t*>(p)+n,2);return v;}
static void wr(void* p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
struct Host {
 P (*ptr)(const void* base,U off);          // pointer field read
 const uint8_t* (*dataTables)(void);        // *sgptDataTables
 void (*fatal)(U msg);                      // Ordinal_10265 + 10024 + _exit(-1)
 void (*op)(U id,P a,P b,P c);              // generic callee by RVA (see each function)
 I (*opi)(U id,P a,P b,P c);                // generic callee returning int
};
// 1.13c D2Client+0x14F40
I sub_14F40(const Host& H,P unit,I idx){
 if(*((uint8_t*)unit+0x4e)!=3) return 1;
 const uint8_t* dt=H.dataTables();
 if(idx<0||ri(dt,0xba0)<=idx) return 0;
 const uint8_t* e=(const uint8_t*)H.ptr(dt,0xb98)+(size_t)idx*0x23c;
 if(!e) return 0;
 U t=rd(unit,0);
 if(t==0&&int16_t(rw(e,0x102))>0) H.op(0x88A70,unit,0,0);
 if(t==1&&int16_t(rw(e,0x104))>0) H.op(0x88A70,unit,0,0);
 return 1;
}
// 1.13c D2Client+0x16250
struct S16250{I g858,g854,g61c,g864; struct{U w[7];}tbl[7];};
// 1.13c D2Client+0x16250
void sub_16250(const Host& H,S16250& s){
 s.g858-=0x40; if(s.g858>=0) return;
 if(s.g61c==7){
  if(s.g854<7){
   if(s.tbl[s.g854].w[0]==0){ s.g854=5; s.g858=0; s.g864=1; return; }
   s.g858-=H.opi(0x160E0,0,0,0); return;
  }
  H.fatal(0x90); return;
 }
 H.fatal(0x8f);
}
// 1.13c D2Client+0x43830
struct S43830{P g3b8,gbbfc; I g400;};
// 1.13c D2Client+0x43830
void sub_43830(const Host& H,S43830& s){
 if(s.g3b8){ P u=s.gbbfc?(P)(intptr_t)H.opi(10331,s.gbbfc,0,0):nullptr; H.op(10358,s.g3b8,u,(P)5); return; }
 s.g400=1;
}
// 1.13c D2Client+0x440C0
// 1.13c D2Client+0x440C0
void sub_440C0(const Host& H,P g3b8,I g2e0){
 for(P n=(P)(intptr_t)H.opi(10717,g3b8,0,0);n;n=H.ptr(n,0x7c)){
  uint8_t* f=(uint8_t*)n+0x34;
  if(*f&1) continue;
  H.op(0x18370,n,0,0); H.op(0xA7D00,0,0,0);
  if(H.opi(10826,n,0,0)==8&&g2e0) H.op(0x53550,n,0,0);
  *f|=1;
 }
}
// 1.13c D2Client+0x44110 (teardown; each step is a callee, order preserved)
// 1.13c D2Client+0x44110
void sub_44110(const Host& H,P* g3b8,P* g54e4,P* g3c4,U* g97e0 /*32 words*/,P g97e8){
 H.op(0x30220,0,0,0); H.op(0x037D0+0x00000,0,0,0); H.op(0x57000,0,0,0);
 if(*g3b8) H.op(10868,*g3b8,0,0);
 *g3b8=nullptr;
 H.op(10076,0,0,0); H.op(10078,0,0,0); H.op(10089,0,0,0); H.op(10104,0,0,0);
 H.op(0x18C30,0,0,0); H.op(0x18BE0,0,0,0);
 if(*g54e4){ H.op(10046,(P)"..\\Source\\D2Client\\ENGINE\\Gfx.cpp",(P)0x7f9,0); *g54e4=nullptr; }
 P c=*g3c4;
 if(ri(c,0xeaa8)) H.op(10043,(P)0x55,0,0);
 wr(c,0xeaa8,0); wr(c,0xeab0,0); wr(c,0xea9c,0); wr(c,0x38,0);
 H.op(10043,(P)0x141,0,0);
 *g3c4=nullptr;
 H.op(0x72D10,0,0,0); H.op(0x582B0,0,0,0);
 H.op(10061,0,0,0); H.op(10061,0,0,0); H.op(10061,0,0,0);
 H.op(10143,g97e8,0,0);
 std::memset(g97e0,0,32*4);
 H.op(10043,(P)0x592,0,0);
}
// 1.13c D2Client+0x526E0 / +0x527A0 (message id 0x161 / 0x15b)
// 1.13c D2Client+0x526E0
// 1.13c D2Client+0x527A0
static void sendMsg(const Host& H,P e,P gbbfc,U id){
 U t=rd(e,0); P u=H.ptr(e,0x2c);
 U a,b;
 if(t==2||(t>3&&t<6)){ a=rd(u,0x10); b=rd(u,0xc); }
 else { a=u?U(H.opi(10750,u,0,0)):0; b=u?U(H.opi(10867,u,0,0)):0; }
 if(!gbbfc) return;
 U m[23]={}; m[0]=1; m[1]=U((uintptr_t)gbbfc); m[11]=0; m[12]=1; m[4]=id; m[5]=b; m[6]=a; // m[6] (uStack_44) = a: kept for the record layout
 H.op(0xB30F0,m,0,0);
}
// 1.13c D2Client+0x526E0
void sub_526E0(const Host& H,P e,P gbbfc){ sendMsg(H,e,gbbfc,0x161); }
// 1.13c D2Client+0x527A0
void sub_527A0(const Host& H,P e,P gbbfc){ sendMsg(H,e,gbbfc,0x15b); }
// ---- shared by 6A3D0/6A550/6A6B0/6AC60: find-or-load the unit's cel block list node
struct Gfx{ I key; I mode; P (*findList)(void); };
static P findNode(P head,I a,I b,bool two){ for(P n=head;n;n=*(P*)((uint8_t*)n+16)){ if(rd(n,0)==U(a)&&(!two||rd(n,4)==U(b))) return n; } return nullptr; }
static I keyOf(const Host& H,P unit){
 if(unit&&((rd(unit,0xc8)>>3)&1)&&H.opi(10866,0,0,0)){ P o=(P)(intptr_t)H.opi(0x560D0,0,0,0); if(o) return ri(o,0x60); }
 return unit?ri(unit,0x60):0;
}
static P ensureNode(const Host& H,P unit,I mode,P* listHead){
 P d=H.ptr(unit,0x54); if(!d){H.fatal(0x3ac);return nullptr;}
 I k=I(H.opi(0x1AF0,0,0,0));
 for(P n=H.ptr(d,0x28);n;n=H.ptr(n,16)) if(ri(n,0)==k&&ri(n,4)==mode) {*listHead=n;return n;}
 H.op(0x6A2F0,(P)(intptr_t)mode,unit,0);
 return H.ptr(d,0x28);
}
// 1.13c D2Client+0x6A3D0 : is the frame's box on screen at offset dx
// ponytail: decompile shows stack-address operands (local_4); modelled as 0.
// 1.13c D2Client+0x6A3D0
bool sub_6A3D0(const Host& H,P unit,I dx,I scrW,I scrH){
 I key=keyOf(H,unit);
 I mode=I(H.opi(10431,0,0,0));
 P hd; ensureNode(H,unit,mode,&hd);
 H.op(10175,0,0,0); H.op(10350,0,0,0);
 P d=unit?H.ptr(unit,0x54):nullptr; if(!d) return false;
 P n=H.ptr(d,0x28); while(n&&ri(n,0)!=key) n=H.ptr(n,16);
 if(!n) return false;
 P f=H.ptr(H.ptr(n,8),0x14); if(!f) return false;
 if(!((ri(f,8)+dx)-(ri(f,0x14)-ri(f,0x10))/2<scrW-1)) return false;
 if(!(ri(f,0xc)+dx>=0)) return false;
 if(!(ri(f,0x14)>=0)) return false;
 return ri(f,0x10)<scrH-1;
}
// 1.13c D2Client+0x6A550 : point (x=param_3 ->px, y=unit[1]) inside the -1 node frame box
// ponytail: stack0xffffffec / &param_3 operands modelled as 0 offsets.
// 1.13c D2Client+0x6A550
I sub_6A550(const Host& H,P unit,I px){
 I key=keyOf(H,unit);
 I mode=I(H.opi(10431,0,0,0));
 P hd; ensureNode(H,unit,mode,&hd);
 I y=ri(unit,4);
 H.op(10175,0,0,0); H.op(10350,0,0,0);
 P d=H.ptr(unit,0x54); if(!d) return 0;
 for(P n=H.ptr(d,0x28);n;n=H.ptr(n,16)) if(ri(n,0)==-1){
  P f=H.ptr(H.ptr(n,8),0x14); if(!f) return 0;
  if(px<ri(f,8)-0x10+key) return 0;
  if(ri(f,0xc)+0x10+key<=px) return 0;
  if(y<ri(f,0x10)) return 0;
  if(ri(f,0x14)<=y) return 0;
  return 1;
 }
 return 0;
}
// 1.13c D2Client+0x6A6B0 : build draw request from the mode-0 node (record emitted via callees)
// ponytail: tail (10005/10002/10069 writes) delegated wholesale to op 0x6A6B0; only the lookup is native.
// 1.13c D2Client+0x6A6B0
void sub_6A6B0(const Host& H,P unit){
 H.op(10175,0,0,0); H.op(10350,0,0,0);
 if(((rd(unit,0xc8)>>3)&1)&&H.opi(10866,0,0,0)) H.op(0x560D0,0,0,0);
 I mode=I(H.opi(10431,0,0,0));
 P d=H.ptr(unit,0x54); if(!d){H.fatal(0x3ac);return;}
 I k=I(H.opi(0x1AF0,0,0,0));
 P n=findNode(H.ptr(d,0x28),k,mode,true);
 if(!n&&!(n=(P)(intptr_t)H.opi(0x6A2F0,(P)(intptr_t)mode,unit,0))) return;
 n=H.ptr(H.ptr(unit,0x54),0x28); while(n&&ri(n,0)!=0) n=H.ptr(n,16);
 if(!n||!H.ptr(H.ptr(n,8),0x14)) return;
 H.op(0x6A6B0,unit,0,0);
}
// 1.13c D2Client+0x6A860 / 0x6AC60 / 0x6B190 : record builders over callees
// 1.13c D2Client+0x6A860
// 1.13c D2Client+0x6AC60
// 1.13c D2Client+0x6B190
// ponytail: bodies mixed with stack-frame artifacts; forwarded to the callee ops, structure noted in spec later.
// 1.13c D2Client+0x6A860
bool sub_6A860(const Host& H,P unit){ return H.opi(0x6A860,unit,0,0)!=0; }
// 1.13c D2Client+0x6AC60
P sub_6AC60(const Host& H,P unit,I mode,I async){ return (P)(intptr_t)H.opi(0x6AC60,unit,(P)(intptr_t)mode,(P)(intptr_t)async); }
// 6B190: walk unit's list at [[unit+0x54]+0x2c] twice (flag pass 0/1), next at +0xa4
// 1.13c D2Client+0x6B190
void sub_6B190(const Host& H,P unit,I id,I dx,I dy){
 if(!unit||!H.ptr(unit,0x54)) return;
 P e=H.ptr(H.ptr(unit,0x54),0x2c); if(!e) return;
 for(I pass=0;pass<2;pass++){
  for(;e;e=H.ptr(e,0x29*4)){
   bool ok=true;
   if((ri(e,0x34)==0||(ok=pass!=0,ri(e,0x34)==0))&&pass!=0) ok=false;
   if(ri(e,0)==6&&ri(e,8)==0) ok=false;
   if(ri(e,0)==8&&ri(e,0x3c)==0) ok=false;
   I cls=ri(e,4);
   if(cls==id&&ok){
    I hp=ri(e,0x18);
    const uint8_t* dt=H.dataTables();
    if(cls<0||ri(dt,0xbc0)<=cls) return;
    const uint8_t* r=(const uint8_t*)H.ptr(dt,0xbbc)+(size_t)cls*0x84; if(!r) return;
    I rec[18]={}; rec[3+2]=id; // local_3c
    if(!H.opi(0x6E2C0,0,0,0)) return;
    if((hp>>8)<(ri(e,0x1c)>>8)) H.op(10041,(P)(intptr_t)(ri(e,0x20)+dx),(P)(intptr_t)(ri(e,0x24)+dy),(P)(intptr_t)r[0x7c]);
   }
  }
  e=H.ptr(H.ptr(unit,0x54),0x2c); // restart list: decompile reuses piVar1 (already null); second pass sees empty list
  e=nullptr;
 }
}
}

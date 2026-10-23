// Derived from cached 1.13c decompile (D2Client has no D2MOO prior art).
// Globals and callees come in through Env so the logic is testable; handles stay native pointers.
#include <cstdint>
#include <cstring>
namespace d2client_23a30 {
using U=uint32_t; using I=int32_t; using P=void*;
static U rd(P p,U n){U v;std::memcpy(&v,static_cast<uint8_t*>(p)+n,4);return v;}
static void wr(P p,U n,U v){std::memcpy(static_cast<uint8_t*>(p)+n,&v,4);}
struct Node{U id; Node* next;};

struct Env23A30 {
 P (*find)(U id);              // DAT_6fbcbf30 list: entry whose [+8]==id, else null
 bool tableOn; I tableCount;   // DAT_6fbcc24c != 0, DAT_6fbcc250
 U (*rec32)(I idx,U off);      // 0x8e-byte record field read, idx validated here
 U (*rec8)(I idx,U off);
 P (*player)();                // FUN_2a70
 U (*c37e90)(P,U); void (*c37ed0)(I,U,I); void (*c38c20)(P,I);
 U now;                        // DAT_6fbcc268
};
static I recIdx(const Env23A30& e,I i){return (i<0||e.tableCount<=i)?0:i;}
// 1.13c D2Client+0x23A30
void sub_23A30(P a,I param2,Env23A30& e){
 for(Node* n=reinterpret_cast<Node*>(static_cast<uintptr_t>(rd(a,0x78)));n;){
  U id=n->id; n=n->next;
  P en=id?e.find(id):nullptr;
  U v=0;
  if(e.tableOn) v=e.rec32(recIdx(e,en?I(rd(en,4)):0),0x60);
  P pl=e.player(); U want=pl?rd(pl,0x68):0;
  if(v!=want) continue;
  if(!param2){ e.c38c20(a,1); continue; }
  P q=id?e.find(id):nullptr; U ptr=q?rd(q,0x28):0;
  if(ptr&&rd(reinterpret_cast<P>(static_cast<uintptr_t>(ptr)),4)){ e.c38c20(a,1); continue; }
  P en2=id?e.find(id):nullptr;
  if(!en2) continue;
  if(e.c37e90(reinterpret_cast<P>(static_cast<uintptr_t>(ptr)),id)) continue;
  U s=e.rec8(recIdx(e,I(rd(en2,4))),0x40); U len=s>6?s:6;
  if(!rd(en2,0x45)||rd(en2,0x4d)||e.now+len<rd(en2,0x55)){
   wr(en2,0x3d,1);
   if(!len) e.c37ed0(1,id,0);
   else{ wr(en2,0x51,e.now); wr(en2,0x49,rd(en2,0x1c)); wr(en2,0x4d,0); wr(en2,0x55,e.now+len); wr(en2,0x45,1); }
  }
 }
}

struct Env23E20 {
 P unit;                       // DAT_6fbcbbfc
 P (*ord10331)(P); I (*ord10826)(P);
 I dataLimit;                  // sgptDataTables+0xc5c
 I* last;                      // DAT_6fbd34b4
 U* flags;                     // DAT_6fbca7e8[]
 U* const* lists; I nLists;    // PTR_DAT_6fba7950..6fba799f, each U[10]-ish record, 0-terminated ids
 bool (*c1d740)(); bool (*c37b30)(); void (*c3200)(P,U);
 U now;                        // DAT_6fbc97f8
};
// 1.13c D2Client+0x23E20
void sub_23E20(Env23E20& e){
 P x=e.ord10331(e.unit); if(!x) return;
 I v=e.ord10826(x);
 if(e.dataLimit<=v||v==*e.last) return;
 *e.last=v;
 if(e.flags[v]) return;
 U* rec=nullptr;
 for(I i=0;i<e.nLists&&!rec;++i){
  U* r=e.lists[i]; if(!r) return;
  for(I j=0;j<10&&r[j];++j) if(I(r[j])==v){rec=r;break;}
 }
 if(!rec) return;
 for(I j=0;j<10&&rec[j];++j) e.flags[rec[j]]=1;
 if(!rec[10]||e.c1d740()){            // +0x28 = word 10
  if(U(e.now-rd(e.unit,0x7c))>0x3e && !e.c37b30()) e.c3200(e.unit,uint16_t(rec[11]&0xffff)); // +0x2c
 }
}

struct Env23F20 {
 P (*lookup)(P a,U idx);                 // FUN_6fb54e20(a, &DAT_6fbba608[idx*0x80], idx)
 void (*notify)(P u,I kind,I variant);   // FUN_6fb38a70(u,0,0,0): decomp elides the per-case args
 void (*c37ed0)(P,I,I); P (*curUnit)();  // DAT_6fbcbbfc
 U (*ord10871)(P,I); void (*ord10680)(U,I,I); U (*ord10630)(); P (*c15b0)();
 I (*c2490)(); I (*c2430)(); P (*c2a70)(); void (*c2c10)(); void (*c4cce0)(I); void (*c7c270)();
 void (*c3200)(P,U);
 U* g354; U* g368; U* g36c; U* g370; U now;  // DAT_6fbcc354/368/36c/370, DAT_6fbc97f8
};
// 1.13c D2Client+0x23F20
void sub_23F20(P a,U p2,U p3,Env23F20& e){
 P u=e.lookup(a,p2&0xff); if(!u) return;
 bool fall=false;
 switch(p3&0xffff){
 case 10: e.notify(u,10,0); return;
 case 0xc: { U r=e.ord10871(u,0x44); if(!r) return; e.ord10680(r,0x15e,0);
  if(!e.ord10630()) return; P p=e.c15b0(); if(!p) return;
  if(int16_t(rd(p,0xfc))<1) return; e.notify(u,0xc,0); return; }
 case 0xd: case 0xe: e.notify(u,I(p3&0xffff),0); return;
 case 0xf: if(e.c2490()==0x10f) e.notify(u,0xf,0); return;
 case 0x10: if(e.c2430()!=1) return; e.notify(u,0x10,e.c2a70()?1:0); return;
 case 0x11: e.c2c10(); return;
 case 0x12: e.c4cce0(1); e.notify(e.curUnit(),0x12,1); return;
 case 0x54: case 0x55: case 0x56: case 0x57: e.notify(u,I(p3&0xffff),e.c2490()==0x10f?1:0); fall=true; break;
 case 0x5a: case 0x5d: e.notify(u,I(p3&0xffff),0); return;
 case 0x5b: e.notify(e.curUnit(),0x5b,0); return;
 case 0x5c: if(!*e.g354) return; *e.g368=e.now+0x19; *e.g36c=1; *e.g370=e.now+0x4b; return;
 default: break;
 }
 if(fall) e.c7c270();
 if(rd(u,0)==0) e.c3200(u,p3);
}
}

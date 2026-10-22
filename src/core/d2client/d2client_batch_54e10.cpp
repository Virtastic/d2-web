// Implementation: D2Client+0x54E10 (3216 B), item-property description builder. Decompile-derived; not yet vectored.
// D2Lang ordinals / Unicode:: / unrecovered D2Client helpers are Svc virtuals. Ordinal_10003 (string table fetch) lost its
// argument in the decompile: each call is a numbered site, in decompile order (S(n)), the host maps n -> string id.
// Register args: EAX = property index (idx), ESI = output buffer (out); EDI = property record for 0x546E0.
#include <cstdint>
#include <string>
namespace d2client_batch_54e10 {
typedef std::u16string W;
struct Unit { int32_t type; int32_t id; };
struct G {
 int32_t shift_c6c{}, mask_c70{}, count_ba0{}; // sgptDataTables +0xc6c, +0xc70, +0xba0
 int32_t h_cc3b8{};                            // DAT_6fbcc3b8
 const Unit* player{};                         // DAT_6fbcbbfc
};
typedef std::pair<int32_t,const W*> A; // sprintf argument: integer or string
struct Svc {
 virtual ~Svc()=default;
 virtual const uint8_t* rec(int32_t idx)=0;                       // sgptDataTables +0xbcc (0x144 stride, count +0xbd4); null if out of range
 virtual int32_t f546E0(const Unit*,int32_t id,int32_t base,const uint8_t* rec)=0; // FUN_6fb046e0 (base = EAX, GUESS: param_2)
 virtual int32_t f54AC0(const Unit*,uint32_t p2,int32_t idx,int32_t val,const uint8_t* rec,uint32_t* flag)=0; // FUN_6fb04ac0
 virtual const W* str(int32_t site,int32_t a=0)=0;                // Ordinal_10003; null = no string
 virtual void spf(W& dst,const W& f,std::initializer_list<A> args)=0; // Unicode::sprintf (arg lists as decompiled; cap 0x80/0x100 host side)
 virtual int32_t f1300()=0; virtual int32_t f1230()=0; virtual int32_t f1e10()=0; // record getters, 0 = null
 virtual const uint8_t* f15b0()=0;                                // FUN_6fab15b0 -> record (byte +0xc)
 virtual void f2fd10()=0;                                         // FUN_6fadfd10(0)
 virtual int32_t ord11044(int32_t h,uint32_t* n)=0;
 virtual int32_t ord10449(int32_t v,int32_t a,uint32_t n,int32_t* oa,int32_t* ob)=0;
 virtual int32_t ord10000(uint32_t v,uint32_t* io)=0;
};
static W dec(int32_t v){ std::string s=std::to_string(v); if(s.size()>9) s.resize(9); return W(s.begin(),s.end()); } // Ordinal_578(buf,10,"%d")+toUnicode(,9)

// 1.13c D2Client+0x54E10
int32_t sub_54E10(Svc& s,G& g,const Unit* unit,int32_t idx,uint32_t p2,uint32_t p3,W& out){
 static const W none;
 auto S=[&](int32_t n,int32_t a=0)->const W&{ const W* p=s.str(n,a); return p?*p:none; };
 out.clear();
 const uint8_t* r=s.rec(idx); if(!r) return 0;
 int32_t v=s.f546E0(unit,idx,(int32_t)p2,r);
 uint32_t fl=1; int32_t two=s.f54AC0(unit,p2,idx,v,r,&fl);
 if(two!=0&&fl==0) return 0;
 uint8_t b1=r[0x36],b2=r[0x37]; uint16_t tail=(uint16_t)(r[0x3c]|(r[0x3d]<<8));
 if(two!=0){ b1=r[0x40]; b2=r[0x41]; tail=(uint16_t)(r[0x46]|(r[0x47]<<8)); }
 uint32_t kind=b1, mode=b2;
 const W* P=&S(1);                      // main label
 W num=dec(v);
 const W& s240=S(2); const W& sep=S(3); const W& plus=S(4);
 W buf; W tmp;
 enum { Brk, Done, C2, C4, L519a, L5159 } go=Brk;
 switch(kind){
 case 1: case 6: case 12:
  if(v<1) buf=num; else if(kind!=12||v>1) buf=plus+num;
  if(mode==1){ out=buf+sep; } else if(mode==2){ out=*P+sep; P=&buf; } else go=Done;
  break;
 case 2: case 7: go=C2; break;
 case 3: case 9:
  if(mode==1) out=num+sep; else if(mode==2){ out=*P+sep; P=&num; } else go=L519a;
  break;
 case 4: case 8: go=C4; break;
 case 5: case 10: {
  int32_t t=(int32_t)((uint32_t)v*100u);
  num=dec((t+((t>>31)&0x7f))>>7); go=C2; break; }
 case 11: {
  if(v<1){ s.spf(out,S(5),{{0x19,nullptr}}); }
  else { int32_t q=2500/v;
   if(q>30) s.spf(out,S(6),{{1,nullptr},{(q+12)/25,nullptr}});
   else s.spf(out,S(7),{{1,nullptr}}); }
  go=Done; break; }
 case 13:
  if(v<1){ if(v>=0) return 0; buf=num; } else buf=plus+num;
  if(!s.f1300()) return 0;
  P=&S(8);
  if(mode==1) out=buf+sep; else if(mode==2){ out=*P+sep; P=&buf; } else go=Done;
  break;
 case 14:
  if(!s.f1300()) return 0;
  if((p3&7)>2) return 0;
  s.spf(out,S(9,v),{});
  out+=sep; P=&S(10); break;
 case 15: {
  int32_t sh=(int32_t)p3>>(g.shift_c6c&0x1f); uint32_t m=(uint32_t)g.mask_c70&p3;
  if(sh<1||g.count_ba0<=sh) return 0;
  s.f2fd10(); const W& a=S(11); const W& f=S(12,v);
  s.spf(out,f,{{v,nullptr},{(int32_t)m,nullptr},{0,&a}}); go=Done; break; } // sprintf args GUESS (decompile shows them on 10003)
 case 16: {
  s.f2fd10(); const W* x=s.str(13); if(!x) return 0;
  s.spf(out,*P,{{v,nullptr},{0,x}}); go=Done; break; }
 case 17: case 18: {
  uint32_t n=0; int32_t a=0,b=0;
  if(g.h_cc3b8) a=s.ord11044(g.h_cc3b8,&n);
  int32_t r12=s.ord10449(v,a,n,&a,&b);
  if(!g.h_cc3b8) r12=a;
  out+=S(14); out+=S(15);
  // retail formats r12 into aUStack_22c, then toUnicode(aUStack_22c, acStack_214) overwrites it with the text of v: num unchanged.
  if(r12<0){ if(v<0) buf=num; } else buf=plus+num;
  if(kind==18) buf+=s240;
  if(mode==1){ out+=buf; out+=sep; } else if(mode==2){ out+=*P; out+=sep; P=&buf; } else go=Done;
  break; }
 case 19: s.spf(out,*P,{{v,nullptr}}); go=Done; break;
 case 20: case 21: v=-v; num=dec(v); go=C4; break;
 case 22:
  if(mode==1){ if(v>=0) out=plus; out+=num; out+=s240; out+=sep; out+=*P; }
  else if(mode==2){ out=*P+sep; if(v>=0) out+=plus; out+=num; out+=s240; }
  else out=*P;
  if(!s.f1e10()){ go=Done; break; }
  out+=S(16); out+=S(17); P=&S(18); break;
 case 23:
  if(!s.f1230()) return 0;
  if(mode==1){ out=num+s240+sep+*P; } else if(mode==2){ out=*P+sep+num+s240; } else out=*P;
  out+=S(19); P=&S(20); break;
 case 24: {
  int32_t hi=v>>8; uint32_t lo=(uint32_t)v&0xff;
  s.f2fd10(); const W* x=s.str(21); if(!x){ go=Done; break; }
  out+=S(22); out+=sep; out+=dec((int32_t)((uint32_t)g.mask_c70&p3)); out+=sep; out+=*x; out+=sep;
  s.spf(buf,*P,{{(int32_t)lo,nullptr},{hi,nullptr}}); P=&buf; break; }
 case 25: case 26:
  if(-v<1) buf=num; else buf=plus+num;
  if(mode==1) out=buf+sep; else if(mode==2){ out=*P+sep; P=&buf; } else go=Done;
  break;
 case 27: {
  const W* x=s.str(23); if(!x||v==0){ go=Done; break; }
  s.f2fd10(); const W* y=s.str(24); if(!y){ go=Done; break; }
  if(v<1) out=num; else { out+=plus; out+=num; }
  out+=sep; out+=*x; out+=sep; out+=*y; out+=sep;
  uint32_t io=7; int32_t ok=s.ord10000(p3,&io);
  if(!ok||(int32_t)io<0||(int32_t)io>6||!s.f1300()){ go=Done; break; }
  P=&S(25); break; }
 case 28: {
  const uint8_t* q; if(v==0||!(q=s.f15b0())){ go=Done; break; }
  const Unit* u=unit; if(!u||u->type!=0) u=g.player;
  int32_t pid=u?u->id:-1;
  if((int8_t)q[0xc]==pid&&v>3) v=3;
  const W* x=s.str(26); if(!x){ go=Done; break; }
  s.f2fd10(); const W* y=s.str(27); if(!y){ go=Done; break; }
  num=dec(v);
  if(v<1) out=num; else out=plus+num;
  out+=sep; out+=*x; out+=sep; P=y; break; }
 default: return 0;
 }
 if(go==C2||go==C4){
  if(go==C2){
   if(mode==1){ out=num; go=L5159; }
   else if(mode==2){ out=*P+sep+num; P=&s240; go=Brk; }
   else go=L519a;
  } else {
   if(mode==1){ if(v<0) out=num; else out=plus+num; go=L5159; }
   else if(mode==2){ out=*P+sep; if(v>=0) out+=plus; out+=num; P=&s240; go=Brk; }
   else go=L519a;
  }
  if(go==L5159){ out+=s240; out+=sep; go=Brk; }
 }
 if(go==L519a){ out=*P; go=Done; }
 if(go==Brk) out+=*P;
 if(kind>5&&(kind<11||kind==21)){
  if(tail!=0x1506){ out+=sep; out+=S(28); return 1; }
  const W& t=S(29); out+=sep; out+=t;
 }
 return 1;
}
}

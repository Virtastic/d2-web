// Prior art: none (D2MOO has no D2Client module). Derived from cached 1.13c decompile.
// Env-seam style: callees, ordinals and table addresses come in through Env.
#include <cstdint>
namespace d2client_a9b50 {
using I=int32_t;
struct Env {
 I* (*lookup)(I id, I table, I type);   // D2Client+0xA4E20 (ECX=id, EDX=table, EAX=type reg-order per that row)
 I (*ord11080)(I* p);                   // Ordinal_11080 (unverified name)
 I (*ord11168)(I* p);                   // Ordinal_11168 (unverified name)
 void (*ord10043)(I a, I b);            // Ordinal_10043(0x4cc,0) (unverified)
 I (*ord10042)(I a, I b);               // Ordinal_10042(0x4f1,0) (unverified)
 void (*f8630)(); void (*f9480)(); void (*f9860)(); void (*f9100)(); void (*f8f60)();
 I tableA;                              // 0x6FBBA608 (RVA 0x10A608), used when s[2]&0xFF==0
 I tableB;                              // 0x6FBB9A08 (RVA 0x109A08), otherwise; both stride 0x200
};
// 1.13c D2Client+0xA9B50
// EAX = state s (dword array), stack = param_1.
void sub_A9B50(const Env& e, I* s, I param_1){
 if(s[8]==0 && s[0]!=6){
  I t=((uint8_t)s[2]==0?e.tableA:e.tableB)+s[0]*0x200;
  I* p=e.lookup(s[1],t,s[0]);
  if(p){ s[4]=(e.ord11080(p)>>13)+4; s[5]=(e.ord11168(p)>>13)+4; }
 }
 if(s[7]!=s[6]){
  if(s[3]==2){ s[0xb]=0; if(s[0xc]) e.ord10043(0x4cc,0); s[0xc]=0; }
  s[6]+= s[6]<s[7]?8:-8;
 }
 if(s[6]>0xff) s[6]=0xf8;
 if(s[8]!=0 && s[6]<1){ e.f8630(); return; }
 if(s[3]==0 && param_1>1){ e.f9480(); return; }
 if(s[3]!=2){ e.f9860(); return; }
 if(s[0xb]==0){ if(s[0xc]==0) s[0xc]=e.ord10042(0x4f1,0); e.f9100(); }
 e.f8f60();
}
}

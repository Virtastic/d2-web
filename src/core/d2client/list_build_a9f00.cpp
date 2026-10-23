// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile; ordinals are Ghidra's, UNVERIFIED.
// Register-passed values (decompile shows unaff_ESI/EBP) are modelled as seam outputs; unverified.
#include <cstdint>
namespace d2client_list_a9f00 {
using U=uint32_t; using I=int32_t;
struct Range { uint8_t used; I lo, hi; uint8_t first, last; } __attribute__((packed)); // 0xb stride at 6fb8e698
static_assert(sizeof(Range)==11,"stride");
struct Ent { U v; uint8_t kind; } __attribute__((packed));                          // 5-byte entries at 6fbacd8c
struct State { Ent ent[8]; U count, flag, group, arg, sel; Range rng[6]; U self; }; // DAT cdda, cdde, cdd6, cdd1, cdd5
struct Calls {
 void* (*unit)();              // ord 10331 (first result unused by body)
 I (*self)();                  // ord 10826
 I (*probe)(I,uint8_t*);       // ord 10436(i,&out) -> nonzero; out = value folded into first/last
 I (*test)(U,U);               // ord 10373(arg,i)
 I (*get)(U,U*,U*);            // ord 10085(i,&out) ; out value stored, second param seam
};
// 1.13c D2Client+0xA9F00
void sub_A9F00(State& s,Calls& c){
 s.count=0; s.flag=0; c.unit(); I me=c.self();
 for(auto& e:s.ent){e.v=0;e.kind=0;}
 if(s.group>5){s.sel=0;return;}                       // DAT cdd5 cleared
 Range& r=s.rng[s.group];
 if(!r.used){r.used=1;
  for(I i=r.lo;i<=r.hi;++i){uint8_t v=0;
   if(c.probe(i,&v)){if(r.first==0xFF)r.first=v; if(r.last<v)r.last=v;}}}
 for(I i=r.first;i<=I(r.last);++i){
  U o=0,x=0;
  if(!c.test(s.arg,U(i))){ if(c.get(U(i),&o,&x)){s.ent[s.count].v=o;s.ent[s.count].kind=0;++s.count;} }
  else if(c.get(U(i),&o,&x)){s.ent[s.count].v=o; if(I(o)==me)s.ent[s.count].kind=0; else{++s.flag;s.ent[s.count].kind=1;} ++s.count;}
 }
}
}

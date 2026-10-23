// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: every global and every callee comes in through Env.
#include <cstdint>
namespace d2client_click_world {
using I=int32_t; using U=uint32_t;
struct Env {
 I (*o10584)(const I* p3);                 // Ordinal_10584(param_3)
 I (*o10801)(I room,U y,U x,I* a,I* b,I u);// Ordinal_10801 -> unit ptr
 I (*o11017)(I room); I (*o10661)(I u); I (*o10478)(I u); I (*o10119)(I u);
 I (*o10695)(I t); I (*o10867)(I p); I (*o10750)(I p); I (*o10195)(I u,I k);
 I (*o10494)(I a,I k); I (*o10802)(I u);
 I (*rd)(I addr);                          // read 32-bit retail memory
 I (*f31cb0)(); void (*f4940)(); void (*f31ce0)(); void (*f2efa0)(I u);
 I (*f423f0)(); void (*f48e0)(I a,I b,I c); void (*f31d70)(); void (*f38a70)(I,I,I,I);
 I room;                                   // uStack_4 (caller frame slot)
 I xorg[3],yorg[3]; U xdiv[3],ydiv[3]; // [0]=!AL (3e4/3ec,3f4/3f5), [1]=AL&&!9854 (39c/3a4,3ac/3ad), [2]=AL&&9854 (2d4/2dc,2e4/2e5); y=(p2-yorg)/ydiv, x=(p1-xorg)/xdiv
 I g9854, gbbfc, gaade4; I cbc98, cbc28;   // DAT_6fbc9854, 6fbcbbfc, 6fbaade4; writes: 6fbcbc98, 6fbcbc28
};
// 1.13c D2Client+0x98B60
// p1=ECX, p2=EDX, al=AL, p3 = stack arg (retail address of a 32-bit struct)
U sub_98B60(Env& e,I p1,I p2,uint8_t al,I p3){
 const I* q=reinterpret_cast<const I*>(static_cast<intptr_t>(p3));
 I u1=e.o10584(q);
 int k=!al?0:(e.g9854==0?1:2);
 U uy=(U)(p2-e.yorg[k])/e.ydiv[k];
 I a,b;
 I u=e.o10801(e.room,uy,(U)(p1-e.xorg[k])/e.xdiv[k],&a,&b,u1);
 if(!u) return 0;
 if(e.o11017(e.room)==0 && e.o10661(u)){
  if(e.f31cb0()) return 0;
  e.f4940(); e.f31ce0(); return 1;
 }
 if(!e.o10478(u)){
  if(e.o10119(u)){
   I t2=e.rd(e.o10695(e.rd(u+4))+0x80);
   if(t2==0x20737274||t2==0x20736b62||t2==0x20646b62) e.f2efa0(u);
   return 1;
  }
  return 0;
 }
 I ty=e.rd(p3), p=e.rd(p3+0x2c);
 bool sp=(ty==2||(3<ty&&ty<6));
 I v1=sp?e.rd(p+0xc):(p==0?0:e.o10867(p));
 I v2=sp?e.rd(p+0x10):(p==0?0:e.o10750(p));
 if(e.o10195(u,2)) return 0;
 if(e.gbbfc && e.o10494(e.gbbfc,0x36)) return 0;
 if(!e.f423f0()) return 0;
 if(e.gaade4 && e.o10802(u)==0x20786f62) e.cbc98=1;
 e.f48e0(e.rd(u+0xc),v1,v2); e.f31ce0(); e.f31d70(); e.f38a70(0,0,0,0);
 e.cbc28=0; return 1;
}
}

// Prior art: none (D2Client has no D2MOO module). Derived from cached 1.13c decompile.
// Env-seam style: globals are plain fields, callees are callbacks.
#include <cstdint>
#include <cstring>
namespace d2client_97450 {
using I=int32_t; using U=uint32_t;
struct Env {
 I g834,beec,b408,b40c,b414,b9a4,bc4c,n911c8,c4_911c4,b9a0,x2a4,y430; // 6fbcb834,6fbcbeec,6fbbb408/40c/414,6fbcb9a4,6fb8bc4c,6fb911c8,6fb911c4,6fbcb9a0,6fbbb2a4,6fbbb430
 I d858,c8c,d854,d864,c88,c90,c34,c50;  // 6fbcb858,6fbcbc8c,6fbcb854,6fbcb864,6fbcbc88,6fbcbc90,6fbcbc34,6fbcbc50
 const I* tab;                      // 0x6FB911D0, 4 ints per row
 uint8_t* ent;                      // 0x6FBBB238, 20-byte rows
 I (*f3cf60)(); void (*c62d0)(); I (*ord11017)(); void (*ord511)(uint8_t*);
 I (*f3bbf0)(); void (*f7d60)(U y); I (*f6320)(); I (*f3bd90)(U y); void (*f72790)(); void (*f8150)(); void (*f8730)();
 void (*fatal)();                   // Ordinal_10265/10024 + _exit(-1)
};
// 1.13c D2Client+0x97450
void sub_97450(Env& e,uint8_t* ev){
 uint16_t ux,uy;std::memcpy(&ux,ev+0xc,2);std::memcpy(&uy,ev+0xe,2);I x=ux;U y=uy;
 auto st=[&](I a,I b){std::memcpy(ev+0x18,&a,4);std::memcpy(ev+0x1c,&b,4);};
 if(e.g834){I z=0;std::memcpy(ev+0x18,&z,4);return;}
 if(e.beec==1){
  bool b2=false;
  if(x<e.b408||e.b40c<x||(I)y<e.b9a4-0x6d+e.bc4c||e.b414<(I)y||e.n911c8<1){
  }else{
   I iv6=e.x2a4;
   for(I i=0;i<e.n911c8;i++){
    uint8_t* p=e.ent+i*20;I a,b,type,arg;uint16_t cmd;
    std::memcpy(&a,p,4);std::memcpy(&b,p+4,4);std::memcpy(&type,p+10,4);std::memcpy(&cmd,p+14,2);std::memcpy(&arg,p+16,4);
    I v3=e.b9a0+e.tab[(i+e.c4_911c4*4)*4];
    if(a==1&&v3<iv6&&iv6<v3+0x2d&&e.b9a4-0x6d+e.bc4c<e.y430&&e.y430<e.b9a4-0x41+e.bc4c&&b==1){
     if(type==9){st(1,0);e.ord511(ev);e.beec=0;
      if(e.ord11017()==0){e.g834=0;e.d864=1;e.d854=5;e.d858=0;}
      e.f8730();e.x2a4=0;e.y430=0;return;}
     e.c34=type;
     if(4<e.c50){e.fatal();return;}
     if(cmd==0xe||cmd==0xc){iv6=e.f3bbf0();b2=false;}
     else{b2=true;
      if(cmd==0x12){e.c90=1;e.f7d60(y);e.c90=0;iv6=e.x2a4;}
      else{e.d858=arg;e.c8c=cmd;e.g834=0;e.d854=6;e.d864=7;e.c88=i;}}
    }
   }
  }
  if(!b2){
   if(e.f3cf60()!=0){e.ord11017();e.c62d0();}
   for(I i=0;i<e.n911c8;i++){I v;std::memcpy(&v,e.ent+i*20+4,4);if(v==1){v=0;std::memcpy(e.ent+i*20+4,&v,4);}}
  }
  st(1,0);e.ord511(ev);e.beec=0;
 }
 if(e.f6320()==0&&e.ord11017()==0&&e.f3bd90(y)!=0){e.ord11017();e.c62d0();e.f72790();e.f8150();}
}
}

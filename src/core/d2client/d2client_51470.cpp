#include <cstdint>
namespace d2client_51470 {
struct Svc { virtual ~Svc()=default; virtual int32_t f557e0(int32_t)=0; virtual int32_t o10143(int32_t)=0; };
struct G { int32_t cc3dc{},c97ec{},c9828{}; };
// 1.13c D2Client+0x51470  (ESI=unit type, EDI=flag)
int32_t sub_51470(Svc& s,G& g,const int32_t* unit,int32_t p1,int32_t p2,int32_t p4,int32_t* flag){
 (void)p1; if(!unit||*unit!=5) return 0;
 if(s.f557e0(p2)&&s.f557e0(p4)&&s.o10143(p1)&&g.cc3dc){
  if(g.c97ec<g.c9828){ g.c9828=g.c97ec+500; return 1; }
  if(*flag==0){ g.c9828=g.c97ec+500; *flag=1; }
 }
 return 0;
}
}

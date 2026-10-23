#include <cstdint>
namespace d2client_510f0 {
struct Svc { virtual ~Svc()=default; virtual int32_t o10013()=0; };
struct G { int32_t bc48{},c9960{},cc418{},c995c{}; };
// 1.13c D2Client+0x510F0  (a=param_1, b=EBX)
void sub_510F0(Svc& s,G& g,int32_t* a,int32_t* b){
 int32_t w=g.bc48;
 if(s.o10013()){ *b-=*b/0x28; *a=((0x28-*b/4)*(*a-w/2))/(w/2)+*a; }
 *a+=g.c9960-g.cc418; *b+=g.c995c-8;
}
}

#include <cstdint>
namespace d2client_512e0 {
// Ordinal_10141(unit, int32_t out[2]) fills a unit's (x,y); Ordinal_10296(esi) +0x13c flag byte.
struct Svc { virtual ~Svc()=default; virtual void o10141(const void* unit,uint32_t* out)=0; virtual int32_t o10296_13c()=0; };
// 1.13c D2Client+0x512E0  (ESI=unit under test, param_1=other unit; disassembly: A=player point (0,0 if null),
// B=param_1 point, C=ESI point; flag==1 compares y else x, unsigned; 0 if C strictly between A and B)
int32_t sub_512E0(Svc& s,const void* cbbfc,const uint32_t* esi,const void* param1){
 if(esi&&esi[4]) return 0;
 uint32_t A[2]={0,0},B[2],C[2];
 if(cbbfc) s.o10141(cbbfc,A);
 s.o10141(param1,B);
 s.o10141(esi,C);
 int k=s.o10296_13c()==1?1:0;
 uint32_t a=A[k],b=B[k],c=C[k];
 if(c>b&&c<a) return 0;
 if(c<b&&c>a) return 0;
 return 1;
}
}

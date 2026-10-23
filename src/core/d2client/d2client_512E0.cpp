#include <cstdint>
namespace d2client_512e0 {
struct Svc { virtual ~Svc()=default; virtual uintptr_t o10141(void* unit)=0; virtual int32_t o10296_13c()=0; };
// 1.13c D2Client+0x512E0  (ESI=limit, param_1=probe)
int32_t sub_512E0(Svc& s,void* cbbfc,uintptr_t limit,const uint32_t* limitHdr,uintptr_t probe,uintptr_t addrLocal18,uintptr_t addrLocal8){
 if(limit&&limitHdr[4]) return 0;
 uintptr_t base=cbbfc? s.o10141(cbbfc):0;
 if(s.o10296_13c()==1){
  if(base<probe&&probe<limit) return 0;
  if(probe<base&&limit<probe) return 0;
 } else {
  if(addrLocal18<addrLocal8&&limit<addrLocal18) return 0;
  if(addrLocal8<addrLocal18&&addrLocal18<limit) return 0;
 }
 return 1;
}
}

#include <cstdint>
namespace d2client_51390 {
struct Obj { int32_t type; int32_t pad[10]; int32_t* f; };
struct Svc { virtual ~Svc()=default;
 virtual int32_t o10867(int32_t*)=0; virtual void o10750(int32_t*)=0; virtual void* o10331(void*)=0;
 virtual int16_t o10482(void*,int32_t)=0; virtual int32_t o10078(void*,int32_t* rec,int32_t,int32_t,int32_t,int32_t)=0; };
// 1.13c D2Client+0x51390  (EDI=obj, EBX=out2, EBP=arg)
Obj* sub_51390(Svc& s,void* cbbfc,Obj* o,int32_t* param1,int32_t* out2,int32_t ebp){
 int32_t v;
 if(!o) v=*param1;
 else {
  int t=o->type; bool direct=t==2||(t>3&&t<6);
  if(direct) v=o->f[3]; else v=o->f? s.o10867(o->f):0;
  if(!direct&&o->f) s.o10750(o->f);
 }
 if(cbbfc){
  void* r=s.o10331(cbbfc);
  if(r&&s.o10482(r,ebp)!=0){ int32_t rec[2]={v,1}; if(s.o10078(r,rec,1,1,0,7)){ *param1=1; *out2=1; return nullptr; } }
 }
 return o;
}
}

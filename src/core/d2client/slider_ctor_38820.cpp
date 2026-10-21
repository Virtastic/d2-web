// D2Client text-slider control constructor (sprintf "%s\ui\menu\textslid"), decompile-derived. Env seam: callees by role.
// Object fields written by byte offset; the decompile's register args (unaff_EDI/unaff_retaddr) are taken as explicit inputs.
#include <cstdint>
#include <cstring>
namespace slider_ctor_38820 {
template<class T> void wr(uint8_t* p, unsigned o, T v){ std::memcpy(p+o,&v,sizeof v); }
struct Services {
 virtual ~Services()=default;
 virtual uint32_t defaultFont()=0;               // FUN_6fadb420(0) -> +0x52
 virtual uint32_t* alloc(uint32_t id)=0;         // Fog #10042(id,0) (0x5d, 0x5e), retail passes 0 as 2nd arg
 virtual void initLabels(uint32_t* tmp)=0;       // 11x Unicode ctor + #10184 + win2Unicode(tmp,..,11)
 virtual int32_t scaleBase()=0;                  // Ordinal_10028 (/10 when width<1)
};
// 1.13c D2Client+0x38820 ; this=obj, p7=count (u16), width=unaff_retaddr, step=unaff_EDI; returns obj
uint8_t* sliderCtor(Services& s,uint8_t* o,uint32_t p2,uint32_t p3,uint32_t p5,uint16_t p7,uint32_t p8,int32_t width,int32_t step){
 wr<uint32_t>(o,0x52,s.defaultFont());
 wr<uint32_t>(o,0x42,p8);
 uint32_t bytes=(uint32_t)p7*2+2;
 wr<uint32_t>(o,8,p2); wr<uint32_t>(o,0xC,p3);
 wr<uint32_t>(o,0x3E,0); wr<uint32_t>(o,0x46,0);
 uint32_t* a=s.alloc(0x5d); wr<uint32_t*>(o,0x28,a);
 uint32_t* b=s.alloc(0x5e); wr<uint32_t*>(o,0x2C,b);
 std::memset(a,0,bytes); std::memset(b,0,bytes);
 wr<uint32_t>(o,0x30,p2); wr<uint16_t>(o,0x34,(uint16_t)p3);
 wr<uint32_t>(o,0x36,0); wr<uint32_t>(o,0x3A,0); wr<uint32_t>(o,0x4E,p5);
 wr<uint32_t>(o,0x14,0); wr<uint32_t>(o,0x20,1); wr<uint32_t>(o,0x4A,1);
 uint32_t tmp[22]; s.initLabels(tmp);
 if(width<1) width=(s.scaleBase()/10)*(step+2);
 wr<int32_t>(o,0x18,width);
 // ponytail: the tail tests `(int)this<1` in the decompile (pointer sign), kept literal: always 0/this for real pointers.
 wr<uint32_t>(o,0x56,0); wr<uint32_t>(o,0x1C,(uint32_t)(uintptr_t)o);
 wr<uint32_t>(o,0x24,0); wr<uint32_t>(o,4,0);
 return o;
}
}

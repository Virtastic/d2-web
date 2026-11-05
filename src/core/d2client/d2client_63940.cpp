// FN probe entry for 1.13c D2Client+0x63940 (src/core/fn/d2client_63940.cpp): the four seams are the D2Lang string getter, the key lookups
// 0x38E50 / 0x391E0 and the D2Win text draw; the port itself is nameLine_63940 in options_panel_62f60.cpp (layout of Imp63940 mirrored here).
#include <cstdint>
namespace d2client_optpanel {
struct Imp63940 {
 const char16_t*(*getString)(uint32_t);
 int32_t(*keyOf)(uint32_t cmd,uint32_t slot);
 const char16_t*(*keyName)(uint32_t cmd,uint32_t slot);
 void(*drawText)(const char16_t*,int32_t x,int32_t y,uint32_t color,uint32_t centered);
};
void nameLine_63940(uint32_t which,int32_t x,int32_t y,const Imp63940&f);
}
extern "C" void d2c_port_63940(uint32_t which,int32_t x,int32_t y,const char16_t*(*gs)(uint32_t),int32_t(*ko)(uint32_t,uint32_t),
  const char16_t*(*kn)(uint32_t,uint32_t),void(*dr)(const char16_t*,int32_t,int32_t,uint32_t,uint32_t)){
 d2client_optpanel::Imp63940 f{gs,ko,kn,dr};
 d2client_optpanel::nameLine_63940(which,x,y,f);
}

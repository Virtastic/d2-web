// Prior art: none (D2MOO has no D2Client module); Ghidra decompile only.
#include <cstdint>
namespace d2client_batch_42880 {
struct Services {
 virtual ~Services()=default;
 virtual int32_t wheelPresent()=0;                 // GetSystemMetrics(SM_MOUSEWHEELPRESENT=0x4b)
 virtual void registerHandler(uint32_t table)=0;   // FUN_6faeccc0(Ordinal_10048(), table)
 virtual uint32_t wheelMsg()=0;                    // RegisterWindowMessageA("MSH_WHEELSUPPORT_MSG"), 0 if MouseZ window absent/replies 0
};
struct State { int32_t g3cc, m98, m90, m8c; uint32_t msgSlot; };  // DAT_6fbcc3cc, 6fbc9898, 6fbc9890, 6fbc988c, 6fba4788
// 1.13c D2Client+0x42880 ; tables 0x6FBA4748 / 4754 / 476C / 4784
void sub_42880(Services& api, State& s, uint32_t t0, uint32_t t1, uint32_t t2, uint32_t t3){
 const bool m8c45 = s.m8c==4 || s.m8c==5;
 if(api.wheelPresent() && s.g3cc && (s.m98==1 ? s.m90!=0 : (s.m98==2 && m8c45))) api.registerHandler(t0);
 if(s.g3cc){
  if(s.m98==1 || (s.m98==2 && m8c45)) api.registerHandler(t1);
  if(s.m98==2 && s.m8c==5) api.registerHandler(t2);
 }
 if(api.wheelPresent() && s.g3cc && s.m98==1 && s.m90==0){
  uint32_t m=api.wheelMsg();
  if(m){ s.msgSlot=m; api.registerHandler(t3); }
 }
}
}

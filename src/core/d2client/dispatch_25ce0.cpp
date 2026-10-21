// Prior art used: UI/input entries; D2MOO has no D2Client.
// Env-seam: callees and the fatal path are injected.
#include <cstdint>
namespace d2client_dispatch_25ce0 {
struct Calls {
 void (*f59f0)(void*); int (*ord10934)(void*); void (*f46f0)(); void (*f4b90)();
 void (*fatal)(); // ord10265(0x39c) + ord10024(msg) + _exit(-1)
};
// 1.13c D2Client+0x25CE0
void d2_client_25ce0(int kind,void* p,Calls& c){
 if(kind==3){c.f59f0(p); if(c.ord10934(p)) c.f46f0(); return;}
 if(kind!=0x15){c.fatal(); return;}
 c.f4b90();
}
}

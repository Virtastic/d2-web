// Prior art used: community pointer tables (names/layout cross-check); D2MOO has no
// D2Client implementation. Rules and erased register arguments checked against 1.13c.
#include "object_animation_24da0.h"
#include <cstring>
#include <cstdlib>
#include <cstddef>
namespace object24 {
template<class T> T get(const void* p,size_t o){T v;std::memcpy(&v,static_cast<const uint8_t*>(p)+o,sizeof v);return v;}
void put(void* p,size_t o,uint32_t v){std::memcpy(static_cast<uint8_t*>(p)+o,&v,4);}
uint32_t u(const void*p,size_t o){return get<uint32_t>(p,o);} int32_t s(const void*p,size_t o){return get<int32_t>(p,o);} uint8_t b(const void*p,size_t o){return get<uint8_t>(p,o);}
void* record(void*p,Services&h){return h.resolve(u(h.resolve(u(p,0x14)),0));}
int32_t add(int32_t a,int32_t b){return int32_t(uint32_t(a)+uint32_t(b));}
int32_t sub(int32_t a,int32_t b){return int32_t(uint32_t(a)-uint32_t(b));}
void refresh(void*p,Services&h){h.refresh(p,0);} void collision(void*p,void*r){put(p,0xc4,(u(p,0xc4)&~2u)|(b(r,0xc4+u(p,0x10))?2u:0u));}
void anim(void*p,void*r,Services&h){h.animate(p,b(r,0x110+u(p,0x10)),b(r,0x164),b(r,0x165),b(r,0x166));}
[[noreturn]] void fatal(Services&h,uint32_t line){h.fatal(line);std::abort();}
}
using namespace object24;
// 1.13c D2Client+0x24DA0
extern "C" uint8_t d2_client_24da0(void*p,Services*h){uint32_t now=h->tick();if(u(p,0xd4)<now){if(u(p,0x10)==0){put(p,0xd4,now+10000);put(p,0x44,b(record(p,*h),0x12a));h->mode(p,1);}else{put(p,0xd4,now+h->random(static_cast<uint8_t*>(p)+0x20,7500,4500));h->mode(p,0);}refresh(p,*h);}return 1;}
static uint8_t timed(void*p,Services&h,bool exact){uint32_t now=h.tick();if(u(p,0xd4)<now){put(p,0xd4,now+h.random(static_cast<uint8_t*>(p)+0x20,exact?30000:12000,exact?20000:7000));put(p,0x44,b(record(p,h),0x12e));h.mode(p,3);refresh(p,h);}void*r=record(p,h);uint32_t mode=u(p,0x10);bool done=exact?(mode>=3&&u(p,0x44)==u(r,0xd8+mode*4)-256):(mode==3&&s(p,0x44)>=sub(s(r,0xe4),256));if(done){put(p,0x44,b(r,0x129));h.mode(p,0);refresh(p,h);}h.advance(p);return 0;}
// 1.13c D2Client+0x24E30
extern "C" uint8_t d2_client_24e30(void*p,Services*h){return timed(p,*h,true);}
// 1.13c D2Client+0x24FA0
extern "C" uint8_t d2_client_24fa0(void*p,Services*h){return timed(p,*h,false);}
// 1.13c D2Client+0x251E0
extern "C" void d2_client_251e0(void*p,Services*h){if(u(p,0x10)==3)put(p,0x10,0);else{if(u(p,0x10)!=1)fatal(*h,0x11f);h->activate(p);put(p,0x10,2);}put(p,0x44,b(record(p,*h),0x129+u(p,0x10)));refresh(p,*h);h->update(p);collision(p,record(p,*h));}
// 1.13c D2Client+0x25280
extern "C" __attribute__((weak)) uint32_t d2_client_25280(void*p,uint32_t suppress,Services*h){void*r=record(p,*h);if(!b(r,0x139))return 0;int32_t n=h->periodOfDay(h->resolve(h->act),nullptr);if(n){if(n<1||n>3)fatal(*h,0x66);if(u(p,0x10)==0){put(p,0x10,1);if(!suppress)refresh(p,*h);h->update(p);h->flags(p,2,b(r,0xc4+u(p,0x10)));}anim(p,r,*h);return 1;}if(u(p,0x10)==1||u(p,0x10)==2){put(p,0x10,0);if(!suppress)refresh(p,*h);h->update(p);anim(p,r,*h);collision(p,r);return 1;}return 0;}
// 1.13c D2Client+0x25400
extern "C" void d2_client_25400(void*p,Services*h){int32_t mode=s(p,0x10),frame=s(p,0x44),speed=get<int16_t>(p,0x4c);if(mode==1){int32_t end=sub(s(record(p,*h),0xdc),256);if(frame!=end){frame=add(frame,speed);put(p,0x44,frame>end?end:frame);return;}}else{if(mode!=3)fatal(*h,0x155);if(frame>0){put(p,0x44,frame<speed?0:sub(frame,speed));return;}}d2_client_251e0(p,h);}
// 1.13c D2Client+0x25660
extern "C" void d2_client_25660(void*p,Services*h){void*r=record(p,*h);uint32_t mode=u(p,0x10);int32_t end=s(r,0xd8+mode*4);if(end==256)return;uint8_t loop=b(r,0x108+mode);auto finish=[&](uint32_t m,uint32_t frame){put(p,0x10,m);put(p,0x44,frame);refresh(p,*h);h->update(p);};if(!loop){if(b(r,0x13a)){d2_client_25400(p,h);return;}if(s(p,0x44)>=sub(end,256)){if(u(p,4)==0xbd&&(mode==2||mode==3)){finish(mode+1,uint32_t(b(r,0x12a+mode))<<8);return;}if(mode!=1)return;put(p,0x10,2);if(b(r,0x133)==1)put(p,0xc4,u(p,0xc4)|0x100000);if(u(r,0x194))h->sound(u(r,0x194)==255?0x153:0x97,4);put(p,0x44,uint32_t(b(r,0x12b))<<8);refresh(p,*h);h->update(p);uint32_t id=u(p,4);if(b(r,0x1b5)&&(id==0x162||id==0x163||id==0x18d||id==0x195||id==0x196||id==0x197||id==0x164))h->effect(p,0x47);h->flags(p,2,b(r,0xc4+u(p,0x10)));anim(p,r,*h);if(!b(r,0x120+u(p,0x10))&&b(r,0x121))h->activate(p);return;}}
int32_t speed=get<int16_t>(p,0x4c),frame=s(p,0x44);if(u(p,4)==12){frame=sub(frame,speed);put(p,0x44,frame<0?add(frame,end):frame);}else if(u(p,4)==0xbd&&mode==3){frame=sub(frame,speed);put(p,0x44,frame);if(frame<0)finish(4,uint32_t(b(r,0x12d))<<8);}else{frame=add(frame,speed);put(p,0x44,frame);if(frame>=end)put(p,0x44,loop?uint32_t(b(r,0x129+mode))*256-u(r,0xd8+mode*4)+uint32_t(frame):uint32_t(end)-256);}}
// 1.13c D2Client+0x258F0
extern "C" uint8_t d2_client_258f0(void*p,Services*h){uint32_t now=h->tick(),old=u(p,0xd4);if(old){if(now-old<=1000)return 1;if(h->distance(p,h->resolve(h->localPlayer))>25){h->message(0x109e08,u(p,0xc)&0x7f,u(p,0xc),2);return 0;}}put(p,0xd4,now);return 1;}
// 1.13c D2Client+0x25B40
extern "C" void d2_client_25b40(void*p,Services*h){d2_client_25660(p,h);if(b(record(p,*h),0x1b4)>3&&uint8_t(h->dispatch(p))==0)return;h->advance(p);}
// 1.13c D2Client+0x25B70
extern "C" void d2_client_25b70(void*p,Services*h){if(!b(record(p,*h),0x1b5))return;uint32_t id=u(p,4),mode=u(p,0x10);if(id==0x98||(id==0x95&&mode==1)||((id==0x162||id==0x163||id==0x164||id==0x18d||id==0x195||id==0x196||id==0x197||id==0x1c7)&&mode==0))h->setMode(p,3,0,0,0);}
static void missiles(void*p,void*owner,uint32_t missile,Services&h){constexpr int32_t dx[6]={-6,-6,0,0,6,6},dy[6]={6,-6,6,-6,6,-6};for(unsigned i=0;i<6;++i){uint32_t out[23]={};out[0]=0x520;out[4]=missile;void*path=h.resolve(u(p,0x2c));uint32_t type=u(p,0);bool fixed=type==2||(type>3&&type<6);int32_t x=fixed?s(path,0xc):(path?h.pathX(path):0);int32_t y=fixed?s(path,0x10):(path?h.pathY(path):0);out[5]=uint32_t(x);out[6]=uint32_t(y);out[7]=uint32_t(add(x,dx[i]));out[8]=uint32_t(add(y,dy[i]));out[9]=1;int32_t n=h.stat(owner,12,0)/5;out[12]=n<1?1:(n>8?8:n);h.spawn(out,owner,p);}}
// 1.13c D2Client+0x25DC0
extern "C" void d2_client_25dc0(void*p,void*owner,Services*h){missiles(p,owner,0x30,*h);}
// 1.13c D2Client+0x25F10
extern "C" void d2_client_25f10(void*p,void*owner,Services*h){missiles(p,owner,0x2d,*h);}

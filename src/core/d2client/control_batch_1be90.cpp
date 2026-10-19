// Prior art used: ; d2bs pointer table is 1.14d and supplies no
// implementation for these 1.13c internals. D2MOO has no D2Client module.
#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>
namespace d2client_control_batch {
template<class T> T read(const void* p, unsigned o) { T v; std::memcpy(&v,static_cast<const uint8_t*>(p)+o,sizeof v); return v; }
struct Unit { int32_t type, classId, id, mode; void* data; void* path; Unit* next; uint32_t retailAddress; };
struct Binding { uint8_t value, slot; int32_t command, item; };
// Required native-pointer services represent imported DLL calls, never retail pointer tokens.
struct Services {
 virtual ~Services()=default;
 virtual Unit* selected()=0; virtual void* inventory(Unit*)=0;
 virtual void prepare()=0; virtual void setInventory(void*)=0; virtual void refresh()=0;
 virtual int32_t inventoryValue(void*,Unit*)=0; virtual void publish(int32_t)=0;
 virtual bool active(Unit*)=0; virtual int32_t unitValue(Unit*)=0;
 virtual void deactivate()=0; virtual void update()=0;
 virtual int32_t displayMode()=0;
 virtual void removeItem(void*,uint32_t,int32_t)=0; virtual void packet(const uint8_t*,unsigned)=0;
 virtual void destroy(void*)=0; virtual void sound(int32_t,int32_t)=0;
 [[noreturn]] virtual void fatal(int32_t)=0;
 virtual int32_t itemProperty(void*,int32_t,int32_t)=0;
 virtual void initializeItem(uint8_t*)=0; virtual void inspectItem(uint8_t*)=0;
 virtual void itemPosition(Unit*,int32_t&,int32_t&)=0;
 virtual int32_t x(Unit*)=0; virtual int32_t y(Unit*)=0;
 virtual int32_t pathCoordinate10(void*)=0; virtual int32_t pathCoordinate0C(void*)=0;
 virtual void effect(int32_t,int32_t,int32_t,int32_t,int32_t)=0;
};
struct State {
 Services& api; void* inventoryOwner{}; uint32_t active{}; int32_t value{};
 uint32_t packetValues[3]{}; std::array<Binding,41> bindings{}; std::array<uint32_t,256> slots{};
 int32_t panel{},modal{}; int32_t viewportMode{},width{},height{};
 struct Rect { int32_t left,right,top,bottom,unused; }; std::array<Rect,4> rects{};
 int32_t itemCount{}; uint8_t* itemRecords{}; // first record field at retail global 0xD24EA
 void* itemOwner{}; void* otherOwner{};
 std::array<void*,33> controls{}; uint8_t selectionActive{}; uint32_t selectionId{};
 std::array<Unit*,128> buckets{}; bool initialized{}; std::array<uint8_t,37> enabled{};
 uint8_t closeMode{},flag{},flag2{}; uint32_t closeValue{};
 const uint8_t* monstats{}; int32_t monstatsCount{}; uint8_t difficulty{};
};
// 1.13c D2Client+0x1BE90
void sub_1BE90(State& s, Unit* edi) {
 Unit* u=s.api.selected(); if(!u)return;
 void* inv=s.api.inventory(u); s.api.prepare(); s.api.setInventory(inv); s.api.refresh();
 Unit* current=s.api.selected(); s.api.publish(s.api.inventoryValue(edi->path,current));
 uint32_t active=s.api.active(u); if(s.active!=active && !active)s.api.deactivate();
 s.active=active; s.value=s.api.unitValue(u); s.api.update();
}
// 1.13c D2Client+0x1C740
void sub_1C740(State& s,const uint8_t* p) { if(read<uint16_t>(p,1)==1)for(unsigned i=0;i<3;++i)s.packetValues[i]=read<uint16_t>(p,3+2*i); }
// 1.13c D2Client+0x1C940
void sub_1C940(State& s,int32_t command) { for(auto& b:s.bindings)if(b.command==command){s.slots[b.slot]=b.value;return;} }
// 1.13c D2Client+0x1C980
uint8_t sub_1C980(State& s,int32_t eax) {
 if(eax==2) { int mode=s.api.displayMode(); if(s.panel && !s.modal)return mode!=2?3:1; return mode!=2 && !s.modal?3:1; }
 return s.api.displayMode()!=2?2:0;
}
// 1.13c D2Client+0x1CA00
void sub_1CA00(State& s,uint32_t incoming) {
 for(int32_t i=0;i<s.itemCount;++i) {const uint8_t* p=s.itemRecords+unsigned(i)*0x26a;
 if(read<int32_t>(p,0x265)==0) {auto id=read<uint32_t>(p,0);s.api.removeItem(s.itemOwner,id,12);
 std::array<uint8_t,4> packet{};std::memcpy(packet.data(),&incoming,4);packet[0]=0x58;std::memcpy(packet.data()+1,&id,2);s.api.packet(packet.data(),3);}}
}
// controls are native counterparts of globals 2415,2419,248D,2411,241D,2491,2421..2489.
// 1.13c D2Client+0x1CAD0
void sub_1CAD0(State& s) {for(void*& p:s.controls)if(p){s.api.destroy(p);s.api.sound(0x7a,0);p=nullptr;}}
// AL is the boolean result; preserve retail's otherwise incidental upper EAX bytes.
// 1.13c D2Client+0x1D6E0
uint32_t sub_1D6E0(State& s,uint32_t eax) {
 if(!s.selectionActive)return eax&0xffffff00u;
 for(Unit* p=s.buckets[s.selectionId&127];p;p=p->next)if(uint32_t(p->id)==s.selectionId){if(p->type==1)return (p->retailAddress&0xffffff00u)|1u;s.api.fatal(0x5a);}
 return 0;
}
// 1.13c D2Client+0x1D740
bool sub_1D740(State& s,int32_t edi) {
 if(!s.initialized)s.api.fatal(0x60);
 if(edi>=37)s.api.fatal(0x65);
 if(!s.enabled.at(edi))return false;
 const Binding* found=nullptr;for(auto& b:s.bindings)if(b.command==edi){found=&b;break;}
 if(!found || found->item==42 || !s.otherOwner)return false;
 int32_t item=found->item;if(s.api.itemProperty(s.otherOwner,item,13))return false;
 if(!s.itemOwner)return false;
 for(int prop:{1,0,14})if(s.api.itemProperty(s.itemOwner,item,prop))return false;
 if(edi==1){std::array<uint8_t,0x26c> data;s.api.initializeItem(data.data());std::memcpy(data.data()+1,&item,4);s.api.inspectItem(data.data());if(data[0x265]>=5)return false;}
 return true;
}
static int32_t add(int32_t a,int32_t b){return int32_t(uint32_t(a)+uint32_t(b));}
// 1.13c D2Client+0x1D840
bool sub_1D840(State& s,int32_t x,int32_t y) {
 auto index=sub_1C980(s,s.viewportMode);auto r=s.rects[index];int32_t dx=s.viewportMode==2?add(s.width,-640)/2:0;
 int32_t dy=add(s.height,-480);return add(r.left,dx)<x && x<add(r.right,dx) && add(r.top,dy)<y && y<add(r.bottom,dy);
}
// 1.13c D2Client+0x1D8F0
void sub_1D8F0(State& s,uint32_t incoming){if(s.closeMode<5)sub_1CA00(s,incoming);s.closeValue=0;s.flag=0;s.flag2=0;sub_1CAD0(s);}
// 1.13c D2Client+0x1FD00
bool sub_1FD00(Unit* u,uint16_t mask){return u && u->type==1 && u->data && (read<uint16_t>(u->data,0x16)&mask);}
// 1.13c D2Client+0x1FFC0
void sub_1FFC0(State& s,Unit* u) {
 // Retail dereferences null for an invalid monstats index; reject that invalid domain explicitly.
 if(!u || u->classId<0 || u->classId>=s.monstatsCount)throw std::out_of_range("monstats");
 auto count=read<int16_t>(s.monstats+uint32_t(u->classId)*0x1a8,0x56+2u*s.difficulty);
 int32_t a,b;s.api.itemPosition(u,a,b);if(u->mode || a!=4)return;
 if(count>=1){int32_t y=s.api.y(u);int32_t x=s.api.x(u);s.api.effect(count==1?0x1ab:0x1ac,x,y,0,1);}
 bool stat=u->type==2 || (u->type>=4 && u->type<=5);
 int32_t x=stat?read<int32_t>(u->path,0x10):(u->path?s.api.pathCoordinate10(u->path):0);
 int32_t y=stat?read<int32_t>(u->path,0x0c):(u->path?s.api.pathCoordinate0C(u->path):0);
 s.api.effect(0x1aa,y,x,0,1);
}
}

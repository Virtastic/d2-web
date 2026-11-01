#pragma once
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
}

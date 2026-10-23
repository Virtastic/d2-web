#include <cstdint>
namespace d2client_511f0 {
struct Q { int32_t pad0[2]; int32_t id; int32_t pad[8]; int32_t v; Q* next; }; // +8 id, +0x2C v, +0x30 next (32-bit layout)
struct Unit { int32_t pad[3]; int32_t id; };
// 1.13c D2Client+0x511F0  (this nullable, EAX=unit)
void sub_511F0(void* self,Q* head,Unit* u,int32_t v){
 if(!self||!u) return;
 for(Q* q=head;q;q=q->next) if(q->id==u->id){ q->v=v; return; }
 // original stores v to absolute address 8 here (faults); omitted.
}
}

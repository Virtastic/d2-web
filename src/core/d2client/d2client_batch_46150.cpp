// D2Client 0x46150: current-unit lookup (hash walk of the 128-bucket unit table).
#include <cstdint>
#include <cstddef>
namespace d2client_b46150 {
#pragma pack(push,1)
struct Unit { int32_t type, classId, f08, id; uint8_t pad[0xE4-16]; uint32_t next; };
#pragma pack(pop)
static_assert(offsetof(Unit, next) == 0xE4, "next at +0xE4");
struct Env {
 uint8_t g9721;          // DAT_6fbc9721
 uint32_t g971d;         // DAT_6fbc971d (id)
 uint32_t buckets[0x80]; // DAT_6fbba808 (addresses)
 Unit* (*at)(uint32_t);  // address -> Unit
 void (*fatal)();        // Ordinal_10265(0x5a)+Ordinal_10024+_exit(-1)
};
// 1.13c D2Client+0x46150
Unit* sub_46150(Env& e) {
 if (!e.g9721) return nullptr;
 for (uint32_t a = e.buckets[e.g971d & 0x7f]; a;) {
  Unit* u = e.at(a);
  if ((uint32_t)u->id == e.g971d) { if (u->type == 1) return u; e.fatal(); }
  a = u->next;
 }
 return nullptr;
}
}

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
}
// 1.13c D2Client+0x46150 ; globals are explicit params: g9721=DAT_6fbc9721, g971d=DAT_6fbc971d,
// hash=DAT_6fbba808[128] (links), at=link->unit (null = links are raw pointers), fatal=no-return path.
extern "C" uint32_t* d2_client_46150(uint8_t g9721, uint32_t g971d, const uintptr_t* hash,
                                     uint32_t* (*at)(uintptr_t), void (*fatal)()) {
 if (!g9721) return nullptr;
 for (uintptr_t a = hash[g971d & 0x7f]; a;) {
  uint32_t* u = at ? at(a) : reinterpret_cast<uint32_t*>(a);
  if (u[3] == g971d) { if (u[0] == 1) return u; fatal(); }
  a = u[0x39];
 }
 return nullptr;
}
namespace d2client_b46150 {
// old seam wrapper
Unit* sub_46150(Env& e) {
 uintptr_t h[0x80]; for (int i = 0; i < 0x80; ++i) h[i] = e.buckets[i];
 static Unit* (*sat)(uint32_t); sat = e.at;
 return reinterpret_cast<Unit*>(d2_client_46150(e.g9721, e.g971d, h,
   [](uintptr_t a) { return reinterpret_cast<uint32_t*>(sat((uint32_t)a)); }, e.fatal));
}
}

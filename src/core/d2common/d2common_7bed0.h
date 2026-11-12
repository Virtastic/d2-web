// 1.13c D2Common+0x7BED0: level callback (table entry) that only forwards: ecx = the room record, which is moved
// to eax for the shared routine at 0x7BD40 (a tail jump), whose result is returned.
#pragma once
#include <cstdint>

namespace c7bed0 {
template <class E> uint32_t Forward(E& e, uint32_t rec) { return e.h(0x7BD40, rec); }
}

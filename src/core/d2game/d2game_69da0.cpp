// 1.13c D2Game+0x69DA0 -- message callback (EDX = message record, ECX unused): a record whose word +0x10 is 1 looks up the 0xA520 row of dword +4; row byte +0x141 set: the dword +0x10 becomes 2 and,
// with row byte +0x122 clear, 0xA550(record) runs. Void.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
void f69da0(NE& e, uint32_t msg)
{
    if (!msg || e.rd16(msg + 0x10) != 1) return;
    const uint32_t row = e.imp(0xA520, e.rd32(msg + 4));
    if (!e.rd8(row + 0x141)) return;
    e.wr32(msg + 0x10, 2);
    if (!e.rd8(row + 0x122)) e.imp(0xA550, msg);
}
}

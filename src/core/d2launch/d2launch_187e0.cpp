// 1.13c D2Launch+0x187E0 -- stdcall message handler: runs the worker 0x15D30; when D2Sound 10035 answers non-zero calls D2Sound 10045 with ECX = 1; runs the worker 0x17DF0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f187e0(GE& e, uint32_t)
{
    e.h(0x15D30);
    if (e.h(0x9a4e)) e.h(0x9a72, 0, 1);
    e.h(0x17DF0);
    return 1;
}
}

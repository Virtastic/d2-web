// 1.13c D2sound+0x07FE0 -- clear the global flag; if the global object has a nonzero [0x48], call sub_6980 on it.
#include <cstdint>
struct D2soundObj7FE0 { uint32_t w[0x13]; };
void D2sound_sub_7FE0(D2soundObj7FE0** g, uint32_t* flag65C0, void (*sub6980)(D2soundObj7FE0*)) {
    if (*g) { *flag65C0 = 0; if ((*g)->w[0x12]) sub6980(*g); }
}

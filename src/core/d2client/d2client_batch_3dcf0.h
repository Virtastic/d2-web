// Types of the D2Client 0x3D470-0x3FC20 ports (d2client_batch_3dcf0.cpp) and their FN probes.
#pragma once
#include <cstdint>
namespace d2client_3dcf0 {
using I=int32_t; using U=uint32_t; using P=void*;
struct G { // D2Client globals (RVA = 6fbXXXXX - 6fa b0000 image base 6fa b0000)
 I id[16];   // 0xD9D30 ; 16 slots, -1 = empty
 I val[16];  // 0xD9DD0
 I mode[16]; // 0xD9E20
 I originX, originY;   // DAT_6fbc9960 / DAT_6fbc995c
 I screenW, screenH;   // DAT_6fb8bc48 / DAT_6fb8bc4c
 I viewMode, camX, camY, camOn, camDX, camDY, oy; // bcc414, bc9960 base, bc995c base, bcbefc, bcbf00, bbb9dc, bcc418
 I panelX;             // DAT_6fbc9958
};
struct H { // callees
 void (*sub_3D770)(P dst,I eax,I n);
 void (*sub_3D3C0)(P self,I arg);
 P    (*sub_AB17F0)(void);
 I    (*ord10847)(I); void* (*sub_B6F6C0)(void);
 void (*ord10019)(U*); P (*ord10061)(P); I (*ord10711)(P); I (*ord10048)(P);
 I (*ord10973)(P,I,I); P (*ord11139)(P,I); I (*ord10744)(P,I);
 P* cachedUnit; P hudUnit; P fallback; P tab[7];
};
struct H2 { // callees for the remaining rows
 P (*ord10061)(P); I (*ord10711)(P); I (*ord10048)(P); I (*ord10973)(P,I,I); I (*ord11139)(P,I); I (*ord10744)(P,I);
 P (*sub_AB17F0)(void); void (*ord10019)(U*); void (*sub_3D3C0)(P,I);
 void (*c3DCF0)(P,const int16_t*,I); void (*c3E0C0)(P,const int16_t*,I);
};
void sub_3DCF0(const G&,P,const int16_t*,I,I,const H&);
void sub_3E0C0(const G&,P,const int16_t*,I,I,const H&);
void sub_3F860(G&,I(*)(P),I(*)(P),P);
}

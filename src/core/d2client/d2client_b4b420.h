#pragma once
// D2Client batch 4B420: unit, seam and state structs (shared by the batch and the FN probes).
#include <cstdint>
namespace d2client_batch_4b420 {
struct Unit { int32_t type, classId, f08, id, f10; void* data; int32_t f18[2]; uint32_t seedLo, seedHi; void* path; Unit* next; };
struct Seams { // all unverified: names from ordinals / sub_ addresses
 virtual ~Seams()=default;
 virtual void s53d20()=0;                 // 0xD3D20-area ui teardown (FUN_6fb53d20)
 virtual void s0ac00()=0;                 // FUN_6fb0ac00
 virtual void fatal(int32_t msg)=0;       // Ordinal_10265 + 10024 + _exit(-1)
 virtual void* createDlg()=0;             // FUN_6fb53e40(...,0x6fafaaf0,...)
 virtual void s53340(int32_t)=0;
 virtual void dlgText(void* dlg,int32_t kind,int32_t cb,int32_t last)=0; // Ordinal_10003 + FUN_6fb536f0
 virtual Unit* s6150()=0;                 // sub_46150
 virtual void s8b20()=0;                  // sub_48B20
 virtual void s38a70()=0;                 // sub_88A70(0,0,0,0)
 virtual Unit* findUnit(int32_t type,uint32_t id)=0; // bucket walk, 128 buckets
 virtual void s3f660()=0; virtual void s44970()=0;
 virtual int32_t ord11017(int32_t)=0; virtual void s62d0()=0;
 virtual int32_t ord10973(int32_t,int32_t,int32_t)=0;
 virtual int32_t ord10494()=0; virtual void* s1260()=0; virtual void s630f0(uint32_t* buf)=0;
 virtual int32_t ord11085()=0; virtual uint8_t sc0b0(Unit*)=0;
};
struct State { // globals, RVAs of the D2Client image
 uint8_t g975f, g9781, g978e, g9753, g9729, g9737, g9786_dummy; uint32_t g9786, g9663; void* g975b; int32_t gbc34, gb834, gb828, gb824;
 int32_t d22f8, d22f9, d22fd, c2f8, c2f4;
 uint32_t g977f, g9783, g9787; uint16_t g978b; uint8_t g978d; int32_t g966b; int32_t clientPlayer; int32_t tab3e[5];
};
void sub_4B420(Seams& s, State& g); void sub_4B5D0(Seams& s, State& g, int32_t player); void sub_4BC80(Seams& s, State& g, const uint8_t* p, int32_t player);
}

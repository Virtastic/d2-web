#pragma once
// D2Client batch 5EBC0: state and service seam declarations (shared by the batch and the FN probes).
#include <cstdint>
namespace d2client_batch_5ebc0 {
struct G {
 uint8_t* cc24c{}; int32_t cc250{};          // sound-record array (0x8e bytes each) and count
 int32_t cc268{}, cc26c{}, cc270{}, cc274{}, cc278{}, cc27c{};
 uint8_t pal[11]{};                          // DAT_6fbcc210..21a
 int32_t cc1b0{};                            // 1 => shifted draw
 uint16_t* cc1b4{};                          // 3x u16 output cursor
 void* cc1b8{}; int32_t cc1bc{};             // 0x2800-byte block list, running entry count
 int32_t* cc1c4{}; void* cc1c0{};            // header, node chain head
 int32_t cc1c8[4]{};                         // 4 handles
 int32_t cc3bc{};                            // *DAT_6fbcc3bc value
 uint8_t c9860{};
};
struct Svc {
 virtual ~Svc()=default;
 virtual void f5da80(uint8_t* rec)=0; virtual void f5db30(int32_t idx,char* out80)=0; virtual void f5dae0()=0; virtual void f5cd10()=0;
 virtual int32_t f5ea90(int32_t a,int32_t b,int32_t c,int32_t d)=0; virtual int32_t fabd810(int32_t h,int32_t* out)=0;
 virtual int32_t o10102()=0; virtual int32_t o10105()=0; virtual void o10103()=0; virtual int32_t o10016()=0;
 virtual int32_t o10091()=0; virtual int32_t o10095()=0; virtual int32_t o10092(int32_t h)=0;
 virtual void f5ebc0(int32_t idx,int32_t eax,int32_t p1)=0; // sub_5EBC0 as 5ED90's callee seam
 virtual void fatal302()=0;                    // Ordinal_10265(0x302) + Ordinal_10024 + _exit(-1)
 virtual int32_t o10190(int r,int g,int b)=0;  // color index
 virtual void o10065(int32_t h)=0; virtual void free(int32_t tag,void* p)=0;
 virtual void o10010(int32_t x0,int32_t y0,int32_t x1,int32_t y1,int32_t col,int32_t a)=0;
 virtual void* alloc(int32_t size,int32_t tag,int32_t n)=0; // Ordinal_10042 (ECX = size, stack = line, 0)
 virtual void o10112()=0; virtual void o10111(int32_t n,void* buf,int32_t z)=0;
};
void sub_5EBC0(Svc& s,G& g,int32_t idx,int32_t eax,int32_t p1);
void sub_5F1C0(Svc& s,G& g,int32_t x,int32_t y,int32_t col);
void sub_5F240(Svc& s,G& g,uint32_t p1);
}

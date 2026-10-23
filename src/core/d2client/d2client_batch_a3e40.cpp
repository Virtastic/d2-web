// D2Client 0xA3E40: allocate and initialise a 0xB10-byte menu/dialog node (env-seam style, unverified seams).
#include <cstdint>
namespace d2client_batch_a3e40 {
// Retail globals (RVA-named by their 1.13c VA 0x6fb5.... minus base). Held in one struct so the vector can pin them.
struct Globals {
 uint32_t dAda8;            // DAT_6fbaada8: nonzero -> log line DAT_6fb80780
 uint32_t g90ea8[6];        // DAT_6fb90ea8..0ebc, set to -1
 uint32_t a46c4, a46c8;     // _DAT_6fba46c4/c8 = 0x10
 uint32_t g90ea0;           // _DAT_6fb90ea0 = 0
 uint32_t cbc28, cbc2c, cbc38, cbc3c; // = 0
 uint32_t cc3dc, cc3e0, cc3f8, cc3fc, cc2f8, cc2f4; // = 0
 uint32_t head;             // DAT_6fbcbc00: node list head (node address, 32-bit)
 uint32_t cbc04;            // DAT_6fbcbc04 = 1
 uint32_t cbc08;            // DAT_6fbcbc08: live-node count, must be 1 after
 uint32_t bb208;            // DAT_6fbbb208: ++
};
struct Seams {
 virtual ~Seams()=default;
 virtual uintptr_t alloc(int32_t line, int32_t z)=0;   // Ordinal_10042(0x3ed,0) (arg meaning unverified)
 virtual uint32_t tick()=0;                            // GetTickCount
 virtual uintptr_t ord10048()=0;                       // Ordinal_10048 (log handle)
 virtual void logA(uintptr_t h, const void* s)=0;      // FUN_6faecc60 with DAT_6fb80780
 virtual void logB(uintptr_t h, const void* s)=0;      // FUN_6faeccc0 with DAT_6fb8e858 / DAT_6fb8e900
 virtual bool s2090()=0;                               // FUN_6faf2090
 virtual void sa3a20(uint32_t p7)=0;                   // FUN_6fb53a20 (sub_A3A20)
 virtual void fatal(int32_t msg)=0;                    // Ordinal_10265 + 10024 + _exit(-1)
};
// 1.13c D2Client+0xA3E40   (ebx = unaff_EBX, stored at [9],[0xb]); returns node or 0 when p3 == 0.
// Node is a 0x2c4-dword array; offsets in dwords. ordinal args unverified.
uint32_t* sub_A3E40(Seams& s, Globals& g, uint32_t ebx, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4,
                    uint32_t p5, uint32_t p6, uint32_t p7, uint32_t p8, uint32_t p9) {
 if (g.dAda8) s.logA(s.ord10048(), nullptr);
 for (auto& v : g.g90ea8) v = 0xffffffffu;
 g.a46c8 = g.a46c4 = 0x10; g.g90ea0 = 0;
 g.cbc28 = g.cbc2c = g.cbc38 = g.cbc3c = 0;
 g.cc3dc = g.cc3e0 = g.cc3f8 = g.cc3fc = g.cc2f8 = g.cc2f4 = 0;
 if (p3 == 0) return nullptr;
 g.cbc08++;
 uint32_t* n = (uint32_t*)s.alloc(0x3ed, 0);
 for (int i = 0; i < 0x2c4; ++i) n[i] = 0;
 n[0x2c3] = g.head;                       // link, then wiped by the second zero pass below (as retail)
 for (int i = 0; i < 0x2c4; ++i) n[i] = 0;
 n[9] = ebx; n[0xb] = ebx;
 n[0x18] = p1; n[0x19] = p2; n[0x16] = p5;
 g.head = (uint32_t)(uintptr_t)n;         // ponytail: 32-bit retail slot; wasm64 needs a wider holder
 n[0xe] = 0; n[0x14] = 0; n[0x13] = 0; n[0x11] = 0xffffffffu; n[0x17] = 2; n[0x2c2] = 0;
 n[10] = p2; n[0xc] = p2; n[5] = p4; n[7] = p8; n[0xd] = p6;
 n[0] = s.tick();
 n[4] = 0; n[0x15] = 0; n[3] = 0; n[2] = 0;
 n[0xe] = p7; n[0x12] = 0xffffffffu; n[6] = p3; n[8] = p9;
 g.cbc04 = 1;
 s.sa3a20(p7);
 s.logB(s.ord10048(), nullptr);
 if (s.s2090()) s.logB(s.ord10048(), nullptr);
 g.bb208++;
 if (g.cbc08 != 1) s.fatal(0x410);
 return n;
}
} // namespace d2client_batch_a3e40

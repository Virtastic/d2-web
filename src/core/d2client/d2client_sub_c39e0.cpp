// D2Client per-frame (callee/global args are Ghidra VAs, image base 0x6fab0000) draw/UI pass, derived from the 1.13c decompile.
// Env-seam: every callee is routed through call(va,a,b,c) / ord(n,a,b,c) (D2 ordinal; names UNVERIFIED).
#include <cstdint>
struct ClientC39E0Env {
    int32_t (*call)(uint32_t va, int32_t a, int32_t b, int32_t c);  // D2Client callee by VA
    int32_t (*ord)(uint32_t ordinal, int32_t a, int32_t b, int32_t c); // imported ordinal (UNVERIFIED names)
    int32_t* g;      // DAT_6fbaad.. flag bytes indexed by (va-0x6fbaad60)/4 ; see spec
    int32_t* cc414; int32_t* cc46c; int32_t* cc578; int32_t* cca30; int32_t* cb9a0; int32_t* cb9a4; int32_t* ae04w;
    const int32_t* bbfc;       // DAT_6fbcbbfc (int*)
    const int32_t* c3a68; const uint8_t* c9721;
    int32_t nPanels;           // DAT_6fb8d5d0 (must be 0x26)
    const int32_t* panelMode;  // *(PTR_DAT_6fb8d538[i]+0x84) per panel
};
#define F(rva) (e.g[((rva) - 0x6fbaad60u) / 4])
extern "C" void d2_client_c39e0(const ClientC39E0Env& e) {
    auto C = [&](uint32_t r, int a = 0, int b = 0, int c = 0) { return e.call(r, a, b, c); };
    if (F(0x6fbaadc8)) return;
    bool two = e.ord(10031, 0, 0, 0) == 2;
    *e.cb9a0 = two ? 0x50 : 0; *e.cb9a4 = two ? -0x3c : 0;
    e.ord(10085, 0, 0, 0); *e.cca30 = 0; *e.cc578 = 0;
    if (F(0x6fbaada4)) C(0x6fb15230);
    if (F(0x6fbaadac)) C(0x6fac25a0);
    C(0x6fb70d30, C(0x6fb01a80));
    if (F(0x6fbaae0c)) C(0x6fb71340);
    if (!F(0x6fbaad84) && !F(0x6fbaadb0) && !F(0x6fbaadb8)) { C(0x6fad8680); C(0x6fad87c0); }
    if (F(0x6fbaada8) && *e.cc414 != 3) C(0x6fb12680);
    C(0x6fb6f0d0); C(0x6fb0c5c0);
    if (F(0x6fbaadcc)) C(0x6fb0c7f0);
    if (F(0x6fbaae10)) C(0x6fb4ae80);
    if (F(0x6fbaae14)) C(0x6fb3e770);
    if (F(0x6fbaadbc)) C(0x6facef10);
    if (F(0x6fbaad84) || F(0x6fbaadb0) || F(0x6fbaadb8) || F(0x6fbaade4) || F(0x6fbaade8) || F(0x6fbaadf0) || F(0x6fbaadf4)) C(0x6fb49440);
    if (F(0x6fbaaddc)) { C(0x6fb09bc0); C(0x6fb49440); }
    C(0x6fb71610);
    if (F(0x6fbaad90)) C(0x6fb28f20);
    if (F(0x6fbaad88)) C(0x6fb6cea0);
    if (F(0x6fbaadd8)) C(0x6fb27250);
    if (F(0x6fbaad60)) C(0x6fb53840);
    if (F(0x6fbaadc0)) C(0x6fb2de00);
    if (F(0x6fbaadd0)) C(0x6fb5aec0);
    if (F(0x6fbaae04)) { C(0x6fae7310); C(0x6fae6620); }
    C(0x6fad9250);
    if (F(0x6fbaad8c)) C(0x6faeebf0);
    if (F(0x6fbaadfc)) *e.cc46c = 1;
    C(0x6fb00e70);
    if (F(0x6fbaae08)) C(0x6fae7740);
    F(0x6fbaad98) ? C(0x6fb00430) : C(0x6fb00100);
    F(0x6fbaad9c) ? C(0x6fb001f0) : C(0x6fb00020);
    if (F(0x6fbaadc4)) C(0x6face5f0);
    C(0x6fad7590);
    if (F(0x6fbaadd4)) C(0x6fb14990);
    if (F(0x6fbaad94)) C(0x6fb20140);
    if (F(0x6fbaae04)) {
        int32_t was = F(0x6fbaae04);   // uVar6, sampled before ordinal 10227
        C(0x6fb6e350); e.ord(10227, 0, 0, 0);
        if (e.nPanels != 0x26) { e.ord(10024, 0, e.ord(10265, 0x139b, 0, 0), 0); return; } // _exit(-1)
        bool toggle = true;
        if (F(0x6fbaae04) == 0) {   // re-read; 6fb6e350 may clear it
            toggle = false;
            if (!*e.c3a68) {
                toggle = true;
                for (int32_t i = 0; i < e.nPanels; i++)
                    if (F(0x6fbaad80 + 4 * i) == 1) {
                        int32_t m = e.panelMode[i];
                        if (m == 1) C(0x6fb72790, i, 1, 0);
                        else if (m == 4) { if (*e.c9721) C(0x6faf7b20); }
                        else if (m != 0) { toggle = false; break; }   // case default -> straight to E7C80
                    }
            }
        }
        if (toggle) {
            int32_t cur = F(0x6fbaae04);
            if (!e.bbfc || (*e.bbfc == 0 && C(0x6fb55440) == 0)) { cur = (cur == 0); *e.ae04w = cur; }
            if (!was) { if (cur) C(0x6faeccc0, e.ord(10048, 0, 0, 0), 0x6fb80924); }
            else if (!cur) { C(0x6fae58e0); C(0x6faecc60, e.ord(10048, 0, 0, 0), 0x6fb80924); }
        }
        C(0x6fae7c80);
    }
    if (F(0x6fbaadb4) && *e.cc414 != 3) { C(0x6fb08fb0); C(0x6fb70ce0); }
    if (F(0x6fbaada0)) C(0x6faf83b0);
    if (F(0x6fbaadb8)) C(0x6fb4fb00);
    C(0x6fad85c0); C(0x6fb2ea90); e.ord(10110, 0, 0, 0);
    C(0x6fb71a20); C(0x6fb6e430); C(0x6fb719b0); C(0x6fb6f990); C(0x6fb6ee60); C(0x6fb02d90);
}

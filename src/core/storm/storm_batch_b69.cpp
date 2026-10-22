// Storm batch b69. Hidden register inputs are explicit leading params.
#include <initializer_list>
#include <cstdint>
#include <cstring>

extern "C" {
void sub_46D0(int* esi);                               // Storm+0x46D0, ESI=arg
void sub_1B960(int* esi, const char* name, int n);     // Storm+0x1B960, ESI=arg
char* sub_2AFD0(const char* s);                        // Storm+0x2AFD0 (strlen-like)
void sub_2B080(char* dst, const char* src, int n);     // Storm+0x2B080
void sub_2AE00(char* dst, const char* src, int n);     // Storm+0x2AE00
int sub_6868(const short* s);                          // Storm+0x6868 (wide length)
extern const char s_AUVIDEOREC[];                      // Storm .rdata @0x6FC41D24
extern const char DAT_36b84[];                         // Storm .rdata @0x6FC36B84
extern const char DAT_36b80[];                         // Storm .rdata @0x6FC36B80
}

// 1.13c Storm+0x45A0   (ESI = unaff_ESI)
unsigned sub_45A0(int* param_1, int* esi) {
  int* p = esi ? esi : param_1 + 1;
  unsigned v = (unsigned)p[1];
  sub_46D0(esi);
  sub_1B960(esi, s_AUVIDEOREC, -2);
  return (((int)v < 1) - 1) & v;
}

// 1.13c Storm+0x4B80
void sub_4B80(intptr_t* p) {
  for (int k = 0; k < 2; k++) {
    intptr_t* hdr = (intptr_t*)p[0];
    if (hdr) {
      intptr_t v = p[1];
      intptr_t* t = (v < 0) ? (intptr_t*)~v
                            : (intptr_t*)((char*)p + (v - hdr[1]));
      *t = (intptr_t)hdr;
      ((intptr_t*)p[0])[1] = p[1];
      p[0] = 0;
      p[1] = 0;
    }
  }
}

// 1.13c Storm+0x1D1D0   (EAX=in, EBX=out_b, ESI=out_s)
void sub_1D1D0(const uint32_t* in, uint8_t* param_1, uint16_t* param_2, uint16_t* param_3,
               uint16_t* param_4, const short** param_5, const short** out_b, const short** out_s) {
  uint32_t* d = (uint32_t*)param_1;
  d[0] = in[0]; d[1] = in[1]; d[2] = in[2]; d[3] = in[3];
  *(uint16_t*)(param_1 + 0x10) = *(const uint16_t*)(in + 4);
  short s = *(const short*)((const char*)in + 0x12);
  const short* p;
  if (s == 0 || s == -1) p = (const short*)(in + 5);
  else p = (const short*)((const char*)in + sub_6868((const short*)((const char*)in + 0x12)) * 2 + 0x14);
  if (*p == 0 || *p == -1) *out_s = p + 1;
  else *out_s = p + sub_6868(p) + 1;
  if (*param_1 & 0x40) {
    *param_2 = *(const uint16_t*)((const char*)*out_s + 2 + sub_6868(*out_s) * 2);
    *param_3 = 0;
    *param_4 = 0;
    const short* q = (const short*)((const char*)*out_s + 4 + sub_6868(*out_s) * 2);
    *out_b = q;
    *param_5 = (const short*)((const char*)q + 2 + sub_6868(q) * 2);
    return;
  }
  *param_2 = 0; *param_3 = 0; *param_4 = 0;
  *out_b = nullptr;
  *param_5 = (const short*)((const char*)*out_s + 2 + sub_6868(*out_s) * 2);
}

// 1.13c Storm+0x2F330   (EBX=n, ESI=dst)
int sub_2F330(char* param_1, char* esi, int ebx) {
  char* len = sub_2AFD0(param_1);
  if ((uintptr_t)len > 5 && param_1[0] == '.' && param_1[3] == 'U') {
    if (strpbrk(param_1 + 4, "?@")) {
      sub_2B080(esi, param_1 + 4, ebx);
      char* q = strpbrk(esi, "?@");
      if (q) {
        *q = 0;
        while (--q, esi <= q && *q == '_') *q = 0;
        sub_2AE00(esi, DAT_36b84, ebx);
        sub_2AE00(esi, param_1, ebx);
        sub_2AE00(esi, DAT_36b80, ebx);
        return 1;
      }
    }
  }
  return 0;
}

// D2Client 0x6D540 (the other 6B5A0..6D590 bodies are the stand-alone d2client_<rva>.cpp files).
#include <cstdint>
#include <cstring>
namespace d2client_batch_6b5a0 {
// 1.13c D2Client+0x6D540 (eax=dst[4], edx=src) : left-justified 3-char copy, space padded to 4
void sub_6D540(char* dst,const char* src){
 std::memcpy(dst,"    ",4);
 for(int i=0;i<3&&src[i];++i) dst[i]=src[i];
}
} // namespace

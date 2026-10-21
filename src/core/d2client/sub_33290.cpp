// Prior art: none (D2MOO has no D2Client). Derived from cached 1.13c decompile.
// Env-seam style: data tables, string fetch (Ordinal_10003), FUN_6fae16c0 and the text draw come in through Env.
#include <cstdint>
#include <string>
namespace d2client_33290 {
using I=int32_t;
struct Env {
 I tblCount; const uint8_t* tbl;                  // sgptDataTables +0xba0 count, +0xb98 base (stride 0x23c)
 const char16_t* (*str)(I callIdx);               // Ordinal_10003, callIdx = nth call in the function (0..7)
 I (*part)(const I* p1,std::u16string& out,const uint8_t* rec,I p2,I p3,I p4,I i); // FUN_6fae16c0
 void (*begin)();                                 // Ordinal_10177
 void (*draw)(I a,I b);                           // Ordinal_10085(a,b,0)
};
// 1.13c D2Client+0x33290
void sub_33290(const Env& e,const I* p1,I p2,I p3,I p4,I /*p5*/,I p6){
 if(!p1||p2<0||p2>=e.tblCount||!e.tbl) return;
 const uint8_t* rec=e.tbl+(size_t)p2*0x23c;
 std::u16string s=e.str(0); I any=0;
 for(I i=0;i<10;i++) if(e.part(p1,s,rec,p2,p3,p4,i)) any=1;
 if(any){ s+=e.str(1); s+=std::u16string(); /*wsprintfA("%d",p3)->toUnicode*/
  for(char16_t c:std::to_string(p3)) s+=c;
  s+=e.str(2); s+=e.str(3); }
 if(p2==0&&p1[0]==0&&p1[1]==6) s+=e.str(4);
 s+=e.str(5); s+=e.str(6); s+=e.str(7);
 e.begin(); e.draw(any+3+p6,p4);
}
}

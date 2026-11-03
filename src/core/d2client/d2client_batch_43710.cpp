// Batch 43710: derived from cached 1.13c D2Client decompile. Ordinals are host callbacks (Calls);
// REG? = register/stack input not confirmed against x86. Unnamed ordinals are marked ORD.
#include <cstdint>
#include <cstring>
namespace d2client_batch_43710 {
using U=uint32_t; using I=int32_t; using P=void*; using W=char16_t;
static size_t wlen(const W*s){size_t n=0;while(s[n])++n;return n;}
static void wcat(W*d,const W*s){d+=wlen(d);while((*d++=*s++)){}}
struct Calls {
 // globals (RVA in comment is the 1.13c data address)
 I g_b8bc48; I g_c9854; P g_cbbbfc; U g_cc390; const U* tbl_82e50; const U* tbl_85d20; // 8bc48 c9854 cbbbfc cc390 82e50 85d20
 void (*drawText)(I strId,I x);              // D2Client+0x42C10
 void (*ord10003)(I,I,I); void (*ord10150)(); // text/draw ORD
 P (*ord10331)(P); U (*ord10826)(P); P (*ord10014)(U); U (*ord10265)(I); void (*ord10024)(const void*,U);
 void (*ord10936)(); void (*ord10614)(); void (*fb60450)(U);
 I (*ord10106)(P); I (*ord10620)(P); P (*ord10871)(P,U); I (*ord10680)(P,I,I);
 P (*ord10218)(P); U (*ord11161)(P,P,I); I (*ord10550)(P);
 void (*fb05b20)(U ecx,P a1,P a2,U a3,U a4,U a5,U a6,U a7); // 55B20 (stdcall 7 stack args + ECX), raw as pushed by the callers
 void (*ord10996)(I,I,P,U,U,I); P (*ord10872)(P,I,I); void (*ord11111)(P,P); void (*ord11108)(P);
 I (*ord10744)(P,I); const W* (*ord10003s)(I); P (*ord10695)(I); P (*ord10806)(P); void (*ord10569)(P,I*,I*);
 const void* dataTables; // sgptDataTables
};
// 1.13c D2Client+0x43710
void sub_43710(Calls&c){
 c.drawText(0x13ea,c.g_b8bc48/2); c.ord10003(c.g_c9854?100:0x3e,4,0); c.ord10150();
 if(!c.g_c9854){c.drawText(0x13eb,c.g_b8bc48/2); c.ord10003(0x6b,4,0); c.ord10150();}
 c.drawText(0x13ec,c.g_b8bc48/2); c.ord10003(c.g_c9854?0x110:0xe8,4,0); c.ord10150();
}
// 1.13c D2Client+0x437E0  (returns *(+0x218) of record; _exit(-1) path reported via ord10024)
U sub_437E0(Calls&c,bool*fatal){
 *fatal=false; if(!c.g_cbbbfc) return 0;
 P a=c.ord10331(c.g_cbbbfc); if(!a) return 0;
 P r=c.ord10014(c.ord10826(a));
 if(!r){c.ord10024(nullptr,c.ord10265(0x5d7));*fatal=true;return 0;}
 U v; std::memcpy(&v,static_cast<uint8_t*>(r)+0x218,4); return v;
}
// 1.13c D2Client+0x4C7C0  (REG? in_EAX = unit; param_1 stack)
void sub_4C7C0(Calls&c,const I*unit,U param_1,const uint8_t*dt_a78,I dt_a80){
 if(!unit||unit[0]!=1) return;
 I i=unit[1],k=i; const uint8_t*rec=nullptr;
 if(i>=0&&i<dt_a80){rec=dt_a78+i*0x1a8;int16_t s;std::memcpy(&s,rec+2,2);k=s;}
 if(k>=0&&k<dt_a80&&k==0x3f&&rec){
  c.ord10936();c.ord10614();int16_t n;std::memcpy(&n,rec+0x62+(c.g_cc390&0xffff)*2,2);
  for(I j=n;j>0;--j)c.fb60450(param_1);
 }
}
// tooltip line builder (rva 0x55B20) is transcribed in d2client_n86.cpp:sub_55B20.
// 1.13c D2Client+0x55F10  (EAX = unit/handle; EBX = item context passed to 10871 and 55B20; stack: a1, a2 passed through to 55B20; ret 8)
void sub_55F10(Calls&c,P eax,P ebx,P a1,U a2,const uint8_t*dt_c18,I dt_c1c){
 if(c.ord10106(eax)!=5) return; I q=c.ord10620(eax);
 if(q<0||q>=dt_c1c) return; const uint8_t*rec=dt_c18+q*0x1b8; if(!rec) return;
 int16_t w; std::memcpy(&w,rec+0x2c,2);
 for(I i=0;i<6;++i){ P it=c.ord10871(ebx,c.tbl_82e50[i]);
  if(it&&(I)w==c.ord10680(it,0x47,0)){ c.fb05b20(0,ebx,a1,a2,0,c.tbl_82e50[i],0,1); return; } }
}
// 1.13c D2Client+0x55FB0  (EAX = esi ctx; EDI = type-4 unit; stack: a1, a2 (ret 8); 55B20 gets (EDI, a1, a2, 0, tbl, 0, 1) with ECX = 0)
void sub_55FB0(Calls&c,P esi,const I*edi,P a1,U a2,const U*tbl85d20){
 if(!edi||edi[0]!=4||c.ord10106((P)edi)!=5) return; const uint8_t*p=(const uint8_t*)c.ord10218((P)edi); if(!p) return;
 if(p[0x87]==1){
  U mask=c.ord11161(esi,(P)edi,0); I me=c.ord10550((P)edi);
  for(I i=0;i<6;++i){ if(i==me) continue; I o=i; if(me<i)o--;
   if((mask>>i)&1) c.fb05b20(0,(P)edi,a1,a2,0,c.tbl_82e50[o],0,1); }
 } else if(p[0x87]==2){
  U v=c.ord11161(esi,(P)edi,1); I n=v<0x40?(I)tbl85d20[v]:0;
  for(I i=0;i<n-1;++i) c.fb05b20(0,(P)edi,a1,a2,0,c.tbl_82e50[i],0,1);
 }
}
// 1.13c D2Client+0x560B0  (EDI = destination W*; stack: unit, a7 (the 7th arg handed to 55B20), tail string; ret 0xc)
void sub_560B0(Calls&c,P unit,U a7,W*dst,const W*param_3){
 static thread_local W buf[0x400]; for(int i=0;i<0x400;i++)buf[i]=0;
 c.fb05b20(0xab,unit,buf,0x400,1,0,0x40,a7);
 I len=0; if(param_3){ len=(I)wlen(buf); }
 if(!param_3||len==0){ wcat(dst,buf); return; }
 I k=len-2; bool found=false;
 if(k>0){ for(;;){ if(buf[k]==10){found=true;break;} k--; if(k<=0) break; } }
 if(!found||k<=0){ wcat(dst,param_3); wcat(dst,buf); return; }
 const W*a=c.ord10003s(0xf9e),*b=c.ord10003s(0xf0c);
 buf[k]=0; wcat(dst,buf); wcat(dst,a); wcat(dst,param_3);
 if(buf[len-1]==10) buf[len-1]=0;   // reads the original tail (buf[len-1]); k<len-1 so it was not the zeroed cell
 wcat(dst,buf+k+1); wcat(dst,b); wcat(dst,a);
}
static void trimNl(W*s){ size_t n=wlen(s); if(n&&s[n-1]==10){ s[n-1]=0; } }
// 1.13c D2Client+0x56210  (ECX, EAX = passed to 10996; EDX = out; stack: unit, arg2 (unused), tail string; ret 0xc)
void sub_56210(Calls&c,U ecx,U eax,W*out,P unit,U arg2,const W*tail){
 (void)arg2;
 c.ord10996(2,0,unit,ecx,eax,0); sub_560B0(c,unit,0,out,tail);
 P it=c.ord10872(unit,0,0x40); if(it){ c.ord11111(unit,it); c.ord11108(it); } trimNl(out);
}
// 1.13c D2Client+0x56280  (EBX = unit; ECX, EAX passed to 10996; stack: out, arg2 (unused), tail string; ret 0xc)
void sub_56280(Calls&c,P ebx,U ecx,U eax,W*out,U arg2,const W*tail){
 (void)arg2;
 c.ord10996(5,0,ebx,ecx,eax,0); static thread_local W buf[0x400]; for(int i=0;i<0x400;i++)buf[i]=0;
 sub_560B0(c,ebx,1,buf,tail); wcat(out,buf);
 P it=c.ord10872(ebx,0,0x40); if(it){ c.ord11111(ebx,it); c.ord11108(it); } trimNl(out);
}
// 1.13c D2Client+0x56330  (EAX = ebx = unit; ESI = out string; stack: arg2 passed on; ret 4). Returns 0 on the _exit(-1) paths (*fatalId = the string id).
void sub_56330(Calls&c,P ebx,W*esi,U ebp,I*fatalId){
 *fatalId=0;
 if(!c.ord10744(ebx,0x35)){*fatalId=0x21a;return;}
 I id=ebx?static_cast<const I*>(ebx)[1]:-1;
 uint8_t*edi=static_cast<uint8_t*>(c.ord10695(id));
 if(!edi){*fatalId=0x21c;return;}
 esi[0]=0;
 bool has14=c.ord10744(ebx,0x14)!=0;
 if(!has14&&!c.ord10744(ebx,0x4a)) return;
 P val; std::memcpy(&val,edi+0xf0,sizeof val); // ord10806 takes the dword at +0xf0
 val=c.ord10806(val);
 if(!val){*fatalId=has14?0x223:0x245;return;}
 static const U kA[4]={2,1,1,0}; static const U kS[4]={0x2b42,0x2b41,0x2b44,0x2b43};
 for(int g=0;g<4;++g){
  W loc[0x100]; for(int i=0;i<0x100;i++)loc[i]=0; // Unicode ctor per cell (2 wchar each in the frame)
  wcat(esi,c.ord10003s(0xf9e)); loc[0]=0;
  wcat(loc,c.ord10003s(kS[g])); wcat(loc,c.ord10003s(0xf9b));
  if(has14) sub_56210(c,(U)(uintptr_t)val,kA[g],esi,ebx,ebp,loc);
  else sub_56280(c,ebx,(U)(uintptr_t)val,kA[g],esi,ebp,loc);
 }
 wcat(esi,c.ord10003s(0xf9e)); wcat(esi,c.ord10003s(0xf9e));
}
// 1.13c D2Client+0x56700  (fastcall ECX=obj)
U sub_56700(Calls&c,uint8_t*obj){ I a,b; c.ord10569(obj,&a,&b); obj[0x4e]=0; if(a<1){U z=0;std::memcpy(obj+0x3c,&z,4);} return 1; }
// 1.13c D2Client+0x56740  (byte-identical twin of 56700)
U sub_56740(Calls&c,uint8_t*obj){ return sub_56700(c,obj); }
}

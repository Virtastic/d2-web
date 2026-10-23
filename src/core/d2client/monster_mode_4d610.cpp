// Implementation batch: D2Client+0x4D610 (change a client monster unit to class `cls`). Decompile-derived; not yet vectored.
// Imported-DLL ordinals and not-yet-recovered helpers are Svc virtuals named by ordinal/RVA; they run in retail call order.
#include <cstdint>
namespace d2client_4d610 {
struct Unit { uint32_t w[0x33]; }; // 32-bit retail unit, dword-indexed: [0]type [1]cls [5]+0x14 data [8,9]seed [0xb]anim [0x11]+0x44 [0x15]+0x54 [0x18]+0x60 [0x19]+0x64 [0x1b]+0x6c [0x2a]+0xa8 [0x31]+0xC4 [0x32]+0xC8
struct Svc {
 virtual ~Svc()=default;
 virtual int32_t dataCount()=0;               // sgptDataTables+0xA80
 virtual const uint8_t* monstats(int32_t)=0;  // sgptDataTables+0xA78 + i*0x1a8
 virtual const uint8_t* monstats2(int32_t)=0; // FUN_1260 (monstats rec +0x18 index -> record), null = reject
 virtual const uint8_t* bitMasks()=0;         // gdwBitMasks_exref (table at 0x6FB7EE50)
 virtual void f19700(Unit*)=0; virtual void f55c50(int32_t)=0; virtual void o10820(int32_t)=0; virtual void f58630(int32_t)=0;
 virtual int32_t o10327()=0; virtual void o10043(int32_t,int32_t)=0;
 virtual void o10488(uint32_t anim,int32_t shifted)=0; virtual void o10938()=0;
 virtual int32_t o10387(int32_t)=0; virtual void o11166()=0; virtual void o10887()=0; virtual void o11090()=0;
 virtual uint32_t f1380(uint32_t* seed)=0;
 virtual void fc370(int32_t)=0; virtual void fcec0(Unit*)=0; virtual void fc150()=0; virtual void f20f20()=0;
 virtual int32_t o10856(int32_t)=0; virtual void o10302(int32_t)=0; virtual void o10706(int32_t,uint32_t)=0;
 virtual void f16950()=0; virtual void f1b300(Unit*,int32_t,int32_t,int32_t,int32_t)=0;
 virtual uint32_t rd(uint32_t addr)=0; virtual void wr(uint32_t addr,uint32_t v)=0;
 virtual int32_t gate390()=0;                 // DAT_6fbcc390
 [[noreturn]] virtual void fatal(int32_t line)=0; // Ordinal_10265/10024/_exit(-1)
};
// 1.13c D2Client+0x4D610  (eax = unit, stack = new class). Data pointers at +0x14/+0x54 are guest dwords: Svc::rd/wr.
void sub_4D610(Svc& s,Unit* u,int32_t cls){
 uint32_t* w=u->w;
 if(cls<0||cls>=s.dataCount()) return;
 const uint8_t* ms=s.monstats(cls);
 if(!ms) return;
 const uint8_t* m2=s.monstats2(cls);
 if(!m2) return;
 s.f19700(u); s.f55c50(0); s.o10820(w[0x18]); w[0x18]=0;
 if(w[0x19]){ s.f58630(w[0x19]); w[0x19]=0; }
 if(int32_t t=s.o10327()) s.f58630(t);
 if(w[0]==1 && w[5] && s.rd(w[5]+0x2c)){ s.o10043(0x66,0); s.wr(w[5]+0x2c,0); }
 w[0x1b]=0; w[1]=cls;
 if(!w[0xb]) return;
 s.o10488(w[0xb],(int32_t)*(const int16_t*)(ms+0x32)<<8);
 s.o10938();
 const uint8_t* bm=s.bitMasks();
 if((bm[4]&ms[0xd]) && w[0x18]==0) w[0x18]=(uint32_t)s.o10387(0);
 s.o11166();
 for(int i=0;i<11;i++) s.o10887();
 s.o11090();
 w[0x11]=s.f1380(&w[8]);
 w[0x31]=(bm[0xc]&m2[4]) ? (w[0x31]|2) : (w[0x31]&~2u);
 w[0x32]&=0xfffbffffu;
 w[0x31]=(m2[5]&bm[0x18]) ? (w[0x31]&~0x20u) : (w[0x31]|0x20);
 w[0x31]|=8;
 w[0x31]=(bm[4]&m2[5]) ? (w[0x31]|0xc) : ((w[0x31]&~4u)|8);
 s.fc370(0); s.fcec0(u); s.fc150(); s.f20f20();
 w[0x2a]=(uint32_t)s.o10856(0);
 for(int i=0;i<8;i++){ int16_t v=*(const int16_t*)(ms+0x170+i*2); if(v>=0) s.o10302(v); }
 if((bm[0]&ms[0xd])==0){
  uint64_t v=(uint64_t)w[8]*0x6ac690c5u+w[9];
  w[8]=(uint32_t)v; w[9]=(uint32_t)(v>>32);
  s.o10706(0x8000,(uint32_t)v&0x3f);
 }
 s.f16950();
 uint32_t m=2; if(ms[0x4d]<8) m=ms[0x4d]+2u;
 int32_t c=(int32_t)w[1];
 if(c<0||c>=s.dataCount()) c=-1;
 bool gate=(c==0x9c||c==0xd3||(c>=0xf2&&c<0xf4));
 if(c==0x14d) m=5;
 else{
  if(c==0x20f) s.f1b300(u,5,0,0,0);
  if(gate){ int32_t g=s.gate390(); m=g==1?3:g==2?4:(g==0?2:(m<8?m:2)); }
  else if(m>=8) m=2;
 }
 if(!w[0x15]) s.fatal(0xce4);
 s.wr(w[0x15]+0x34,m);
 s.wr(w[0x15]+0x38,m);
}
}

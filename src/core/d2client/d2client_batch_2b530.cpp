// Prior art: ; D2MOO has no D2Client module, d2bs table is 1.14d. Decompile-derived.
// LOW CONFIDENCE batch: Ghidra lost register args (ECX/EDX/ESI/EAX) on most of these; argument
// roles below are best readings. Unrecovered imports (Ordinal_N) and unmapped internals (fn RVAs)
// go through Services::ord / Services::fn.
#include <cstdint>
#include <cstring>
namespace d2client_batch_2b530 {
struct Services {
 virtual ~Services()=default;
 virtual intptr_t ord(uint32_t ordinal,intptr_t a=0,intptr_t b=0,intptr_t c=0,intptr_t d=0)=0; // imported DLL ordinal
 virtual intptr_t fn(uint32_t rva,intptr_t a=0,intptr_t b=0,intptr_t c=0,intptr_t d=0)=0;      // D2Client internal
 virtual const uint8_t* skillRecord(int32_t id)=0; // sgptDataTables+0xB98 + id*0x23C, null if out of range (count at +0xBA0)
 virtual int32_t skillCount()=0; virtual int32_t tableCount6C()=0; // +0xB6C
};
template<class T> static T rd(const void* p,unsigned o){T v;std::memcpy(&v,(const uint8_t*)p+o,sizeof v);return v;}
// 1.13c D2Client+0x2B530
int sub_2B530(Services& s,uint8_t* unit,int32_t skillId){
 if(skillId<0||skillId>=s.skillCount())return 0;
 const uint8_t* rec=s.skillRecord(skillId); if(!rec)return 0;
 int32_t idx=rd<int16_t>(rec,0xEA); if(idx<0||idx>=s.tableCount6C())return 0;
 if(unit){uint32_t f=rd<uint32_t>(unit,0xC4)|0x40;std::memcpy(unit+0xC4,&f,4);}
 if(!s.fn(0xA5F70))return 0;
 if(!unit||!rd<int32_t>(unit,0x2C))return 0;
 if(!s.ord(10777,rd<int32_t>(unit,0x2C)))s.fn(0x1330,(intptr_t)(unit+0x20)); else s.ord(10663,(intptr_t)unit);
 s.fn(0x1330,(intptr_t)(unit+0x20)); s.fn(0x1630,(intptr_t)unit); s.fn(0x1660,(intptr_t)unit);
 if(s.ord(10786,(intptr_t)unit,rd<int32_t>(rec,0x114),idx)>0)s.fn(0x1330,(intptr_t)(unit+0x20));
 s.fn(0xB0450); return 1;
}
// 1.13c D2Client+0x32F30  (unit, out min, out max, skill, item-or-null)
void sub_32F30(Services& s,void* unit,int32_t* outMin,int32_t* outMax,intptr_t skill,intptr_t item){
 *outMin=0;*outMax=0;
 if(!skill)return;
 if(!item){ if(!rd<int32_t>(unit,0x60))return; item=s.ord(10061); if(!item)return; }
 if(!s.ord(10711))return;
 if(!s.ord(10744,item,0x26)){ // spell-damage branch
  int32_t pct=0; int16_t a=(int16_t)s.ord(10213);
  if(a)pct=(int32_t)s.ord(10973,(intptr_t)unit,0,0)*a/100;
  int16_t b=(int16_t)s.ord(10120,item);
  if(b)pct+=(int32_t)s.ord(10973,(intptr_t)unit,2,0)*b/100;
  *outMin=(int32_t)s.ord(10973,(intptr_t)unit,0x9F,0);
  *outMax=(int32_t)s.ord(10973,(intptr_t)unit,0xA0,0);
  pct+=(int32_t)s.ord(10973,(intptr_t)unit,0x19,0);
  int32_t lo=(int32_t)s.ord(10973,(intptr_t)unit,0x12,0), hi=(int32_t)s.ord(10973,(intptr_t)unit,0x11,0);
  if(pct<-0x59)pct=-0x5A;
  *outMin=(lo+pct)* *outMin/100+*outMin; *outMax=(hi+pct)* *outMax/100+*outMax;
  if(s.ord(10494,(intptr_t)unit,0x4E))s.ord(10804,(intptr_t)unit,0,0,1);
  *outMin+=(int32_t)s.fn(0x14A0,100,*outMin);
  int32_t m=(int32_t)s.fn(0x14A0,100,*outMax); *outMax+=m; if(*outMax<*outMin)*outMax=*outMin;
  int32_t xl=0,xh=0; s.fn(0x30E30,(intptr_t)unit,(intptr_t)&xl,(intptr_t)&xh);
  if(xh<xl)xh=xl; *outMin+=xl; *outMax+=xh; return;
 }
 // weapon-damage branch
 intptr_t w=s.ord(10321); int32_t lo=0;
 s.ord(10205,(intptr_t)unit,w,0); s.ord(10040,w,1);
 int32_t hi=(int32_t)s.ord(10256,1)+(int32_t)s.ord(10532,(intptr_t)unit,w,1);
 switch((uint8_t)s.ord(10557,w)){
  case 5:{int32_t d=(int32_t)s.ord(10242,(intptr_t)unit,w,1)/0x19; if(d<1)d=1; lo/=d; hi/=d; break;}
  default:break;
 }
 int32_t a=lo>>8,b=hi>>8; *outMin=a; *outMax=b>a?b:a;
}
// 1.13c D2Client+0x33240
void sub_33240(Services& s,void* u,int32_t* a,int32_t* b,int32_t* c){ s.fn(0x31220,(intptr_t)u); s.fn(0x30E30,(intptr_t)u,(intptr_t)a,(intptr_t)b,(intptr_t)c); }
// 1.13c D2Client+0x33550 (tooltip text builder, skill-level variant; text assembly via Services)
void sub_33550(Services& s,int32_t level,int32_t skillId,int32_t y){
 if(!level||skillId<0||skillId>=s.skillCount()||!s.skillRecord(skillId))return;
 s.fn(0x33550,level,skillId,y); // ponytail: string assembly (Ordinal_10003 ids lost) delegated wholesale
}
// 1.13c D2Client+0x33B50 (sibling of 0x33550, this=skillId)
void sub_33B50(Services& s,int32_t skillId,int32_t unit,int32_t y){
 if(!unit||skillId<0||skillId>=s.skillCount()||!s.skillRecord(skillId))return;
 s.fn(0x33B50,skillId,unit,y); // ponytail: same as above
}
// 1.13c D2Client+0x34960
void sub_34960(Services& s,void* unit,intptr_t a,intptr_t b,intptr_t,int32_t p5,int32_t p6){
 if(!a||!b)return; int32_t w=0,h=0;
 if(!s.fn(0xC230))return;
 sub_32F30(s,unit,&w,&h,0,0); s.fn(0xC190); s.fn(0xF0A0,w,0,p5,p6);
}
// 1.13c D2Client+0x349F0
void sub_349F0(Services& s,void* unit,intptr_t a,const uint8_t* b,intptr_t,int32_t p5,int32_t p6){
 if(!a||!b)return; uint8_t sc=b[0x1A5]; int32_t w=0,h=0;
 sub_32F30(s,unit,&w,&h,0,0);
 w=(w*sc)/128; h=h*(int32_t)sc/128; // trunc-toward-zero via +127 bias in retail; same for signed /128 in C
 s.fn(0xE190,(intptr_t)unit); s.fn(0xF0A0,w,0,p5,p6);
}
// 1.13c D2Client+0x34AB0
void sub_34AB0(Services& s,void* unit,void* b,int32_t c){
 if(!b||!c)return; intptr_t r=s.fn(0x17F0); if(!r)return;
 if(s.ord(10747,(intptr_t)unit)){
  s.ord(10601,c,0xA02); s.ord(10435,(intptr_t)unit,rd<int32_t>((void*)r,0x18));
  s.ord(10601,c,0xA03); s.ord(10435,(intptr_t)unit,rd<int32_t>((void*)r,0x1C));
  s.fn(0x30B40,(intptr_t)b,(intptr_t)unit,c); return;
 }
 s.ord(10601,c,0xA0C); s.ord(10435,(intptr_t)unit,rd<int32_t>((void*)r,0x18));
 s.ord(10601,c,0xA0D); s.ord(10435,(intptr_t)unit,rd<int32_t>((void*)r,0x1C));
 int32_t x=0,y=0,z=0; sub_33240(s,unit,&x,&y,&z); s.fn(0xF0A0,x,z,0,(intptr_t)b);
}
// 1.13c D2Client+0x34C00
void sub_34C00(Services& s,void* unit,int32_t a,const uint8_t* rec,int32_t d){
 if(!a||!rec)return; intptr_t r=s.fn(0x17F0); if(!r)return;
 s.ord(10601,a,0x98C); s.ord(10435,(intptr_t)unit,rd<int32_t>((void*)r,0x18));
 s.ord(10601,a,0x98D); s.ord(10435,(intptr_t)unit,rd<int32_t>((void*)r,0x1C));
 int32_t x=0,y=0,z=0; sub_33240(s,unit,&x,&y,&z);
 uint8_t sc=rec[0x1A5];
 if(sc!=0x80){x=(int32_t)s.fn(0x14A0,0x80,x);y=(int32_t)s.fn(0x14A0,0x80,y);}
 int16_t id=rd<int16_t>(rec,0);
 int32_t p=(int32_t)s.ord(10567,(intptr_t)unit,id,d,0); s.ord(10297,(intptr_t)unit,id);
 int32_t q=(int32_t)s.ord(10121,(intptr_t)unit,id,d,1); s.ord(11091,(intptr_t)unit,id,d,1);
 s.fn(0xF0A0,x+(q>>8)+(p>>8),d,d,0);
}
// 1.13c D2Client+0x34D70 (ESI = scale; eax = pointer to third out)
void sub_34D70(Services& s,void* unit,int32_t* outX,int32_t* outY,int32_t scale){
 if(!scale)return; int32_t a=0,b=0;
 s.fn(0x31220,(intptr_t)unit); s.fn(0x30E30,(intptr_t)unit,(intptr_t)&a,(intptr_t)&b);
 *outX+=(a*scale)/128; *outY+=(b*scale)/128;
}
// 1.13c D2Client+0x34E00
void sub_34E00(Services& s,void* unit,int32_t a,const uint8_t* rec,int32_t d){
 if(a&&rec&&s.fn(0x17F0))sub_34C00(s,unit,a,rec,d);
}
// 1.13c D2Client+0x34E50
void sub_34E50(Services& s,void* unit,int32_t a,int32_t b,const uint8_t* rec){
 if(!a||!b)return; int32_t o[2]={0,0}; s.fn(0xC830);
 sub_34D70(s,unit,&o[0],&o[1],0);
 int32_t i1,i2,k;
 if(s.ord(10641,(intptr_t)unit,1)&&s.ord(10051,(intptr_t)unit,rd<int32_t>(unit,0x60))==2){i1=(int32_t)s.ord(10973,(intptr_t)unit,0x17);k=0x18;}
 else{i1=(int32_t)s.ord(10973,(intptr_t)unit,0x15);k=0x16;}
 int32_t i2v=(int32_t)s.ord(10973,(intptr_t)unit,k,0);
 if(i1<2)i1=1; if(i2v<3)i2v=2; if(i2v<i1)i2v=i1;
 int32_t id=rd<int16_t>(rec,0);
 s.ord(10786,(intptr_t)unit,rd<int32_t>(rec,0x138),id);
 s.ord(10973,(intptr_t)unit,0x149,0); s.fn(0x14A0,100,i1); s.fn(0x14A0,100,i2v);
 s.ord(10973,(intptr_t)unit,0x14B,0); s.ord(10973,(intptr_t)unit,0x14A,0);
 int32_t r=id?id:3;
 s.fn(0xF0A0,(intptr_t)s.fn(0x14A0,100,i1),r,b,0);
}
}

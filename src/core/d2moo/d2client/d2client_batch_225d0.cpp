#include "d2client_batch_225d0.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>

// Prior art used: RE map and PRNG rows; D2MOO has no
// D2Client implementation. Local 1.13c cache supplies sound-list behavior.
// List links and sound/table fields retain explicit x86 serialized widths.
namespace d2client_batch_225d0 {
static U get(const B*p,unsigned n){U v;std::memcpy(&v,p+n,4);return v;}
static void put(B*p,unsigned n,U v){std::memcpy(p+n,&v,4);}
static B* sounds(State&s,Unit u){return s.resolve(get(u,0x78));}
static B* next(State&s,B*n){return s.resolve(get(n,4));}
static B* entryData(State&s,B*n){return s.resolve(get(n,0));}
static void require(Unit u){if(!u)std::abort();}
static B* find(State&s,U id){if(id)for(B*n=s.resolve(s.entries);n;n=next(s,n)){B*d=entryData(s,n);if(get(d,8)==id)return d;}return nullptr;}
static B* row(State&s,I i){if(!s.table)std::abort();if(i<0||i>=s.count)i=0;return s.table+U(i)*0x8e;}
static I category(State&s,U id){auto*n=find(s,id);return s.table?I(get(row(s,n?I(get(n,4)):0),0x60)):0;}
// 1.13c D2Client+0x225D0
U sub_225D0(State&s,Unit u,I eax){require(u);if(!s.enabled)return 0;I base=0;U width=0;
 if(s.table){base=I(get(row(s,eax),0x60));width=row(s,base)[0x3d];}
 for(B*n=sounds(s,u);n;n=next(s,n)){auto*e=find(s,get(n,0));I i=e?I(get(e,4)):0;
 if(base<=i&&i<I(U(base)+width))return get(n,0);}return 0;}
// 1.13c D2Client+0x22B20
void sub_22B20(State&s,Unit u,I eax){I mode=s.mode();require(u);bool old=get(u,0x88)!=0;
 if((!old&&eax<0)||(old&&eax>0))return;
 put(u,0x88,eax>0);
 B*c=nullptr;if(mode==1){c=s.config(u);if(c)for(unsigned o:{4u,0x1cu,0x34u,0x3cu,0x78u,0x58u})s.flush(get(c,o));}
 else if(mode==2){U i=s.index(u);if(i>0x23c)std::abort();c=s.indexedConfig(i);if(c)for(unsigned o=0;o<32;o+=4)s.flush(get(c,o));}}
// 1.13c D2Client+0x22C10
void sub_22C10(State&s,Unit u,U){if(s.mode()!=1)return;B*c=s.config(u);if(!c||U(s.tick-s.lastRandom)<=3)return;
 uint64_t seed=uint64_t(get(s.player,0x20))*0x6ac690c5+get(s.player,0x24);
 put(s.player,0x20,U(seed));put(s.player,0x24,U(seed>>32));s.play(u,get(c,0x78),I(U(seed)%3)+6,0,0);s.setTime(u,s.tick);s.lastRandom=s.tick;}
// 1.13c D2Client+0x22CA0
void sub_22CA0(State&s,Unit u,I edi){if(s.mode()!=1)return;B*c=s.config(u);if(!c||edi<8||edi>11)return;
 U sound=get(c,0x44+U(edi-8)*4);U tick=s.tick;
 if(sound==0xa84){if(U(tick-s.getTime(u))<25)return;}else if(!sound)return;
 s.play(u,sound,0,0,0);require(u);put(u,0x7c,s.tick);s.lastRandom=s.tick;}
// 1.13c D2Client+0x22D60
void sub_22D60(State&s,Unit u){U sound=s.setFlags(u,get(u,0xb0));s.play(u,sound,0,0,0);put(u,0x7c,s.tick);s.lastRandom=s.tick;}
static void removeConfigured(State&s,Unit u,bool narrow){if(s.mode()!=1)return;B*c=s.config(u);if(!c)return;require(u);
 for(B*n=sounds(s,u);n;){U id=get(n,0);n=next(s,n);I cat=category(s,id);
 const unsigned offsets[]={4,0x1c,0x34,0x68,0x44,0x48,0x4c,0x50,0x70,0x74,0x78};
 for(unsigned o:offsets){if(narrow&&(o<0x44||o>0x50))continue;if(cat==I(get(c,o))){s.remove(u,id,1);break;}}}}
// 1.13c D2Client+0x22DA0
void sub_22DA0(State&s,Unit u){removeConfigured(s,u,false);}
// 1.13c D2Client+0x22EB0
void sub_22EB0(State&s,Unit u){removeConfigured(s,u,true);}
// 1.13c D2Client+0x23070
void sub_23070(State&s,Unit u){require(u);for(B*n=sounds(s,u);n;){U id=get(n,0);n=next(s,n);
 auto*e=find(s,id);if(s.table&&row(s,e?I(get(e,4)):0)[0x3e])s.remove(u,id,0);}}
// 1.13c D2Client+0x23120
void sub_23120(State&s,Unit u,I ebx){require(u);for(B*n=sounds(s,u);n;){U id=get(n,0);n=next(s,n);s.remove(u,id,ebx);}}
// 1.13c D2Client+0x23170
void sub_23170(State&s,Unit u,I){if(s.mode()!=1)return;B*c=s.config(u);if(!c)return;U tick=s.tick;
 if(get(c,0x70)&&!s.busy(u)&&!s.blocked(u)){s.play(u,get(c,0x70),0,0,0);s.lastSpecial=tick;require(u);put(u,0x80,tick);s.nextSpecial=s.now()+30;return;}
 if(s.action(u)==8)sub_22CA0(s,u,8);}
// 1.13c D2Client+0x23A30
void sub_23A30(State&s,Unit u,I enable,B* ecx){require(u);B*scan=s.resolve(s.entries);
 for(B*n=sounds(s,u);n;){U id=get(n,0);n=next(s,n);B*e=nullptr;
 if(id)for(;scan;scan=next(s,scan))if(get(entryData(s,scan),8)==id){e=entryData(s,scan);break;}
 I cat=s.table?I(get(row(s,e?I(get(e,4)):0),0x60)):0;B*c=s.config(u);scan=s.resolve(s.entries);
 if(cat!=(c?I(get(c,0x68)):0))continue;
 if(!enable){s.remove(u,id,1);scan=s.resolve(s.entries);continue;}
 B* arg=ecx;e=find(s,id);if(e){arg=s.resolve(get(e,0x28));if(arg&&get(arg,4)){s.remove(u,id,1);scan=s.resolve(s.entries);continue;}}
 e=find(s,id);if(!e)continue;B*d=e;if(s.active(arg,id))continue;
 U duration=std::max(U(6),U(row(s,I(get(d,4)))[0x40]));U clock=s.clock;
 if(!get(d,0x45)||get(d,0x4d)||U(clock+duration)<get(d,0x55)){
 put(d,0x3d,1);put(d,0x51,clock);put(d,0x49,get(d,0x1c));put(d,0x4d,0);put(d,0x55,clock+duration);put(d,0x45,1);scan=s.resolve(s.entries);}}}
// 1.13c D2Client+0x23E20
void sub_23E20(State&s){Unit player=s.player;void*r=s.room(player);if(!r)return;I level=s.level(r);
 if(s.levelCount<=level||level==s.previousLevel)return;
 s.previousLevel=level;if(s.visited[level])return;
 const I*g=nullptr;for(unsigned i=0;i<20;++i){g=s.groups[i];if(!g)return;bool hit=false;
 for(unsigned j=0;j<10&&g[j];++j)if(g[j]==level){hit=true;break;}
 if(hit)break;
 g=nullptr;}
 if(!g)return;
 for(unsigned j=0;j<10&&g[j];++j)s.visited[g[j]]=1;
 if(!g[10]||s.levelBlocked()){require(player);if(U(s.tick-get(player,0x7c))>62&&!s.playing())s.levelSound(player,U(g[11])&0xffff);}}
}

// Prior art used: D2Client has no D2MOO module. Community pointer tables and
// provide names only; local 1.13c instructions settle erased args.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <cstdlib>

namespace d2client_28bc0 {
// Retail addresses are guest addresses, NEVER truncated native pointers. The host
// translates them; temporary/output buffers cross the call seam as native uintptr_t.
using Address = uint32_t;
struct Host {
    void* context;
    uint8_t* (*memory)(void*, Address);
    uintptr_t (*invoke)(void*, uint32_t, const uintptr_t*, size_t);
    template<class T> T read(Address a) const {
        T v; std::memcpy(&v, memory(context,a),sizeof(v)); return v;
    }
    template<class T> void write(Address a,T v) const {
        std::memcpy(memory(context,a),&v,sizeof(v));
    }
    uintptr_t call(uint32_t rva,std::initializer_list<uintptr_t> a={}) const {
        return invoke(context,rva,a.begin(),a.size());
    }
    uint32_t word(Address a) const { return read<uint32_t>(a); }
    void word(Address a,uint32_t v) const { write(a,v); }
};
template<class T> static uintptr_t out(T* p) { return reinterpret_cast<uintptr_t>(p); }
static constexpr Address base=0x6fab0000;
static uint32_t g(const Host& h,uint32_t rva) { return h.word(base+rva); }
static void g(const Host& h,uint32_t rva,uint32_t v) { h.word(base+rva,v); }
[[noreturn]] static void fatal(const Host& h,uint32_t line) {
    auto msg=h.call(0xbf92,{line}); h.call(0xbf80,{base+0xd359f,msg});
    h.call(0x3664,{0xffffffffu}); std::abort();
}
static uint32_t coordinate(const Host& h,Address unit,bool y) {
    const auto type=h.word(unit), path=h.word(unit+0x2c);
    if(type==2 || (int32_t(type)>3 && int32_t(type)<6)) return h.word(path+(y?0x10:0xc));
    return path?uint32_t(h.call(y?0xc2ec:0xc3be,{path})):0;
}
// 1.13c D2Client+0x28BC0
// EAX=owner; stack=(skill,x,y,left). Ord10537 has FIVE args, four outputs.
void drawSkill(const Host& h,Address owner,Address skill,uint32_t x,uint32_t y,uint32_t left) {
    if(!owner || !skill) return;
    Address record=h.call(0xc640,{skill}); if(!record) return;
    uint32_t overlay=h.call(0x3daa0,{record});
    uint32_t mode=h.call(0x3da60,{skill});
    auto masks=g(h,0xcee50);
    if(!(h.read<uint8_t>(masks)&h.read<uint8_t>(record+5))) {
        auto room=h.call(0xc1f0,{owner});
        if(room && h.call(0xc208,{room})) mode=1;
    }
    uint32_t count=0, state=0; int32_t index=-1;
    auto special=h.call(0xc766,{skill,out(&index),0,out(&state),out(&count)});
    uint32_t alternate=0;
    if(!special) { state=h.call(0xc718,{owner,skill,1}); alternate=h.call(0xce9e,{skill}); }
    // Native output buffer replaces retail's reused stack argument slot.
    h.call(0x3e5e0,{x,mode,overlay,record,uint32_t(index),count,y,left});
    if(alternate) h.call(0x3d8a0,{record,mode,alternate,x,y});
    else h.call(0x3e770,{x,y,record,mode,state,count});
}
// 1.13c D2Client+0x28CE0
void drawSelectedSkills(const Host& h) {
    Address owner=g(h,0x11bbfc); if(!owner) return;
    for(int side=0;side<2;++side) {
        auto getter=side?0xc5ce:0xc5bc;
        Address skill=h.call(getter,{owner});
        if(skill && int32_t(h.call(0xc718,{owner,skill,1}))<1) {
            uint32_t line=side?0x294:0x289;
            auto id=h.call(0xc42a,{skill,base+0xd6b38,line,base+0xd6b38,line});
            h.call(0xc706,{owner,id});
            h.call(side?0xc52c:0xc50e,{owner,0,0xffffffffu});
            skill=h.call(getter,{owner});
        }
        drawSkill(h,owner,skill,side?g(h,0xdbc48)-165:117,g(h,0xdbc4c),side?0:1);
    }
}
// 1.13c D2Client+0x28DC0
void drawBelt(const Host& h) {
    h.call(0xd3a8,{1});
    Address inventory=h.word(g(h,0x11bbfc)+0x60);
    auto belt=h.call(0xc262,{inventory,8});
    if(!belt) g(h,0xf5384,2);
    else if(h.call(0xc220,{belt})==0x13) {
        int32_t type=h.call(0xcf1c,{belt}); if(type<0) fatal(h,0xb07);
        g(h,0xf5384,uint32_t(type));
    }
    if((!g(h,0xfadfc)&&!g(h,0x11c470)) || g(h,0xfada4)) g(h,0x11c46c,0);
    g(h,0x11c474,0);
    if(g(h,0x11c46c)) {
        uint32_t type=g(h,0xf5384);
        // Jump table: 0/5 disable top row; 1/4 retain bottom only; 2 none; 3 all.
        bool rows[3]={true,true,true};
        if(type==2) rows[0]=rows[1]=rows[2]=false;
        else if(type==1 || type==4) rows[1]=rows[2]=false;
        else if(type==0 || type==5) rows[2]=false;
        g(h,0x11c474,type==2?0:1);
        uint32_t cell[18]={}; cell[13]=g(h,0x11a774);
        for(uint32_t i=0;i<3;++i) if(rows[i])
            h.call(0xd096,{out(cell),uint32_t(int32_t(g(h,0xdbc48))/2)+21,g(h,0xdbc4c)-i*32-41,0xffffffffu,5,0});
    }
    uint8_t layout[0x110];
    h.call(0xcf22,{g(h,0xf5384),g(h,0x11b99c),out(layout)});
    for(uint32_t slot=0;slot<layout[4];++slot) {
        if(slot>3&&!g(h,0x11c474)) break;
        Address item=h.call(0xcf28,{inventory,slot}); if(!item) continue;
        uint32_t itemX=coordinate(h,item,false);
        bool ordinary=!h.call(0xc1a8,{item,4,0x5d5,base+0xd6b38}) && h.call(0xc814,{item}) &&
            !h.call(0xca12,{item,2}) && (!g(h,0x11bbfc)||!h.call(0xc1fc,{g(h,0x11bbfc),0x36}));
        if(ordinary) h.call(0x265e0);
        else {
            h.write<uint8_t>(base+0x11a7d0,h.call(0xd390,{128,0,0}));
            h.write<uint8_t>(base+0x11a7d1,h.call(0xd390,{0,128,0}));
            h.write<uint8_t>(base+0x11a7d2,h.call(0xd390,{0,0,128}));
            h.write<uint8_t>(base+0x11a7d3,h.call(0xd390,{128,128,0}));
        }
        if(!ordinary || (item==g(h,0x11c47c)&&g(h,0x11c468))) {
            uint32_t box[4]; h.call(0xcf46,{g(h,0xf5384),g(h,0x11b99c),out(box),itemX});
            auto color=h.read<uint8_t>(base+(ordinary?0x11a7d1:0x11a7d0));
            h.call(0xd0ba,{box[0],box[2],box[0]+29,box[2]+29,color,0});
        }
        uint32_t box[4]; h.call(0xcf46,{g(h,0xf5384),g(h,0x11b99c),out(box),itemX});
        h.call(0x6b5a0,{item,box[0],box[2]});
        if(slot>=4 || !h.call(0xc814,{item})) continue;
        char16_t text[100];
        for(auto& c:text) h.call(0xceb00,{out(&c)}); // Unicode ctor IAT +0xCEB00
        Address match=0;
        for(int pass=1;pass>=0&&!match;--pass) {
            for(Address r=base+0x11a2e0;r<base+0x11a754;r+=10) {
                if(h.word(r)==g(h,0xf9f74+slot*4)&&h.word(r+6)==uint32_t(pass)) {
                    if(h.read<uint16_t>(r+4)!=0xffff) match=r+6;
                    break;
                }
            }
        }
        if(!match) continue;
        // EAX retains the action loaded at +0x29110; ECX is only the search cursor.
        auto string=h.call(0x39f30,{g(h,0xf9f74+slot*4),1}); h.call(0xceab8,{out(text),string});
        auto length=h.call(0xceab0,{out(text)});
        if(int32_t(h.call(0xd35a,{out(text)}))>28) {
            auto* end=text+length;
            do { *end--=0; } while(int32_t(h.call(0xd35a,{out(text)}))>28);
        }
        h.call(0xd37e,{out(text),box[0]+2,box[3]-2,4,0});
    }
    if(g(h,0xf5384)==2 || g(h,0xfadfc)||g(h,0x11c46c)) h.call(0x277e0,{inventory});
}
// 1.13c D2Client+0x29250
void drawPanel(const Host& h) {
    auto video=h.call(0xd10e); uint32_t ebx=video==3;
    if(h.call(0xd0ae)==2) {
        if(g(h,0x11c414)==2||g(h,0x11c414)==3) h.call(0x271c0,{ebx});
        if(g(h,0x11c414)==1||g(h,0x11c414)==3) h.call(0x270d0,{ebx});
    }
    for(uint32_t r:{0x27290u,0x27bd0u,0x27a90u,0x26970u,0x28320u,0x278b0u,0x26880u}) h.call(r,{ebx});
    drawBelt(h); drawSelectedSkills(h); h.call(0x289a0,{ebx});
    auto owner=g(h,0x11bbfc);
    if(owner) { auto room=h.call(0xc1f0,{owner}); if(room) {
        uint32_t level=h.call(0xc1de,{room});
        if(g(h,0x1234b0)!=level) { g(h,0x1234ac,60); g(h,0x1234b0,level); }
    }}
    h.call(0x267b0);
}
// 1.13c D2Client+0x29920
uint32_t canUseItem(const Host& h,Address record,Address player,Address item) {
    if(!record) fatal(h,0xa3d);
    if(h.read<uint8_t>(record+0x129)==1 || h.read<uint8_t>(record+0x12a)==1) {
        h.call(0x7efa0,{item}); return 0;
    }
    if(h.read<uint8_t>(record+0x11d)==1) return 1;
    h.call(0x23200,{player,20}); return 0;
}
// 1.13c D2Client+0x29990
void useBeltItem(const Host& h,uint32_t modifier,Address inventory,Address player,uint32_t slot) {
    if(!g(h,0x119854)) modifier=0;
    if(h.call(0xc202,{inventory})||g(h,0xfada4)) return;
    auto layout=h.call(0xc8a4,{player,0,g(h,0x119854)});
    uint32_t coords[8]; h.call(0xc862,{layout,g(h,0x11b99c),out(coords)});
    Address item=h.call(0xcf28,{inventory,slot}); if(!item) return;
    Address record=h.call(0xc1ba,{h.word(item+4)}); if(!record) fatal(h,0xa64);
    if(canUseItem(h,record,player,item)!=1 || h.call(0x81cb0,{item})) return;
    h.call(0x148e0,{0x26,h.word(item+0xc),modifier,0}); h.call(0x81ce0,{item});
    if(modifier&&player) {
        for(Address node=g(h,0x11c4d4);node;node=h.word(node+0x30)) {
            if(h.word(node+4)!=7||h.word(node+0xc)!=h.word(player+0xc)||h.word(node+0x20)) continue;
            uint32_t id=h.word(node+8); if(id==0xffffffffu) break;
            bool special=h.call(0xc1cc,{item,0x4c})||h.call(0xc1cc,{item,0x51})||h.call(0xc1cc,{item,0x50});
            h.call(0x23f20,{id,1,special?0x54u:0x55u}); g(h,0x11c468,0); return;
        }
    }
    // ESI supplies the item; four pending zero stack words belong to +0x88A70.
    auto sound=h.call(0x81d70,{item}); h.call(0x88a70,{sound,0,0,0,0}); g(h,0x11c468,0);
}
// 1.13c D2Client+0x29E80
bool canInteract(const Host& h,uint32_t mode,Address player,Address target) {
    if(mode==6||mode==13) {
        Address skill=h.call(mode==6?0xc5bc:0xc5ce,{player});
        if(skill) { Address rec=h.call(0xc640,{skill});
            if(rec&&h.read<uint16_t>(rec+0x2e)==19) {
                uint32_t a,b; return h.call(0x18880,{skill,target,out(&a),out(&b)})!=0;
            }
        }
    }
    if(target&&h.word(target)==2) {
        return !h.call(0xcefe,{player,target})||h.call(0xcf0a,{player,target,0x804});
    }
    auto flag=h.call(0xcf04,{target}); return !h.call(0xc6e2,{player,target,flag});
}
// 1.13c D2Client+0x29F30
uint32_t allowMode(const Host& h,Address other) {
    Address item=h.call(0xc310); if(!item) return 1;
    int32_t index=h.call(0xc42a,{item,base+0xd6ad4,0x2cc});
    Address tables=h.word(g(h,0xce474));
    if(index>=0&&index<int32_t(h.word(tables+0xba0))) {
        Address row=h.word(tables+0xb98)+uint32_t(index)*0x23c;
        if(row&&!(h.read<uint8_t>(row+7)&h.read<uint8_t>(g(h,0xcee50)+0x1c)) &&
           (!other||(h.word(other+0x10)!=1&&h.word(other+0x10)!=5))) return 0;
    }
    return 1;
}
// 1.13c D2Client+0x2A070
uint32_t setPathTarget(const Host& h,uint32_t id,uint32_t type,Address unit) {
    Address path=h.word(unit+0x2c);
    auto target=h.call(0xa4e20,{id&127,base+0x10a608+(type<<9),type});
    h.call(0xc364,{path,target});
    return target?uint32_t(h.call(0xc376,{path,unit,0})):0;
}
// 1.13c D2Client+0x2A0B0
void validatePathTarget(const Host& h,Address unit) {
    if(!unit) fatal(h,0x26);
    Address path=h.word(unit+0x2c); if(!path) fatal(h,0x2a);
    if(h.call(0xc2da,{path})) {
        uint32_t type,id; h.call(0xcd54,{path,out(&type),out(&id)});
        auto target=h.call(0xa4e20,{id&127,base+0x10a608+(type<<9),type});
        if(target!=h.call(0xc2da,{path})) h.call(0xc364,{path,0});
    }
}
// 1.13c D2Client+0x2A180
void clearInteraction(const Host& h,Address unit,uint32_t argument) {
    if(unit) h.word(unit+0xc4,h.word(unit+0xc4)&~64u);
    if(h.call(0xc43c,{unit,argument})) h.call(0x6a220,{unit,0});
}
// 1.13c D2Client+0x2A460
void moveTo(const Host& h,Address unit,uint32_t x,uint32_t y,uint32_t send) {
    uint32_t dx=x-coordinate(h,unit,false); if(int32_t(dx)<0) dx=0u-dx;
    uint32_t dy=y-coordinate(h,unit,true); if(int32_t(dy)<0) dy=0u-dy;
    uint32_t type=h.word(unit);
    if((type!=1||h.call(0x1c10,{h.word(unit+4),2}))&&(int32_t(dx)>1||int32_t(dy)>1)) {
        uint32_t mode=1;
        // Retail's command leaves words 2/3 uninitialised except word 3 for monsters.
        uint32_t command[7]; command[0]=x; command[1]=y;
        if(type==0&&unit!=g(h,0x11bbfc)) {
            if(send) { h.call(0x2a1b0,{unit,x,y,2,0,0}); return; } mode=23;
        } else if(type==1) command[3]=1;
        command[4]=100; command[5]=15;
        h.call(0x2a750,{mode,unit,out(command),0});
    } else h.call(0xc670,{h.word(unit+0x2c),0});
}
} // namespace d2client_28bc0

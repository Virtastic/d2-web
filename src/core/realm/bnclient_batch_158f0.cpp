// BNClient 1.13c grid-resource cryptography and loaders, Stage A.
// Prior art used: network row; bncsutil/bsha1.cpp for the
// broken SHA family; D2MOO's D2MCPClient is a stub and supplies no grid loader.
// Rules below follow cached 1.13c decompiles plus x86 disassembly, especially
// hidden EAX/EBX/EDI arguments, signed shifts, and Storm's output pointer.
#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace d2::bnclient::grid {
using U32 = std::uint32_t;
using I32 = std::int32_t;
using Byte = std::uint8_t;
struct HashState { U32 digest[5], low, high, block[16]; };
static_assert(sizeof(HashState) == 0x5c);
struct State {
    HashState hashes[3];                 // retail e7c0, stride 0x5c
    std::uint16_t schedules[3][52];       // retail ea18, stride 0x68
    U32 gid_size, clh_size, extra_size;
    Byte *gid, *clh, *extra;              // native pointers, separate from sizes
    U32 resource_flag;                   // retail dce4
};
// Only calls outside this batch are seams. No optional no-op implementations:
// callers supply the module's CRT, IDEA, Storm allocation, and fatal operations.
struct Imports {
    void* user;
    void (*seed)(void*, U32);             // CRT srand, 0x15f2
    I32 (*random)(void*);                 // CRT rand, 0x15ff
    U32 (*time32)(void*);                 // CRT time32, 0x1621
    void (*expand)(const Byte*, std::uint16_t*); // 0x12260: key ECX, schedule EAX
    void (*invert)(const std::uint16_t*, std::uint16_t*); // 0x125e0
    void (*decrypt8)(const Byte*, const std::uint16_t*, Byte*); // 0x12390
    void (*load)(void*, const char*, Byte**, U32*, U32, U32); // Storm 279
    void (*release)(void*, Byte*);        // Storm 280
    I32 (*extra_enabled)(void*);          // Storm 10227
    void (*fatal)(void*, U32);            // 10265(id), 10024("",text), exit(-1)
};
static constexpr U32 initial[5] = {
    0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u, 0xc3d2e1f0u
};
[[noreturn]] static void die(const Imports& io, U32 message) {
    io.fatal(io.user, message);
    std::abort(); // The imported retail exit path cannot return.
}
static void initialize(HashState& h) {
    std::copy_n(initial, 5, h.digest);
    h.low = h.high = 0;
}
static U32 signed_rotate(U32 value, unsigned amount) {
    return (value << amount) | U32(std::bit_cast<I32>(value) >> (32 - amount));
}
static void rounds(U32* h, const U32* w, bool signed_shifts) {
    U32 a=h[0], b=h[1], c=h[2], d=h[3], e=h[4];
    for (unsigned i=0; i<80; ++i) {
        U32 f, k;
        if (i<20) { f=(b & c) | (~b & d); k=0x5a827999; }
        else if (i<40) { f=b ^ c ^ d; k=0x6ed9eba1; }
        else if (i<60) { f=((b | c) & d) | (b & c); k=0x8f1bbcdc; }
        else { f=b ^ c ^ d; k=0xca62c1d6; }
        U32 t=(signed_shifts ? signed_rotate(a,5) : std::rotl(a,5)) + f + e + w[i] + k;
        e=d; d=c; c=signed_shifts ? signed_rotate(b,30) : std::rotl(b,30);
        b=a; a=t;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e;
}

// 1.13c BNClient+0x16810 -- grid SHA variant: unrotated XOR expansion and SAR.
void compress(HashState* state) {
    U32 words[80];
    std::copy_n(state->block,16,words);
    for (unsigned i=16;i<80;++i)
        words[i]=words[i-16]^words[i-3]^words[i-14]^words[i-8];
    rounds(state->digest,words,true);
}

// 1.13c BNClient+0x167B0 -- EAX index, ECX nullable output.
void digest(const State& state, U32 index, U32* output) {
    if (output) std::copy_n(state.hashes[index].digest,5,output);
}

// 1.13c BNClient+0x16EC0 -- stack index/block, EBX nullable digest output.
void update(State& state, U32 index, const void* input, U32* output) {
    auto& h=state.hashes[index];
    U32 next=h.low+0x200u;
    // The carry test is JGE (signed), not an unsigned carry or a 64-bit add.
    if (std::bit_cast<I32>(next)<std::bit_cast<I32>(h.low)) ++h.high;
    h.low=next;
    std::memcpy(h.block,input,64);
    compress(&h);
    digest(state,index,output);
}

// 1.13c BNClient+0x158F0 -- stack length, EBX output; seed is restored to time.
void random_key(const Imports& io, Byte* output, I32 length) {
    io.seed(io.user,0x150b);
    for (I32 i=0;i<std::bit_cast<I32>(U32(length)-1u);) {
        Byte value=Byte(io.random(io.user));
        if (value) output[i++]=value;
    }
    io.seed(io.user,io.time32(io.user));
    output[std::bit_cast<I32>(U32(length)-1u)]=0;
}

// 1.13c BNClient+0x15960 -- EAX password, stack direction flag.
void setup(State& state, const Imports& io, const Byte* password, U32 forward) {
    if (!password) die(io,0x4e);
    Byte random_bytes[112];
    io.seed(io.user,0x4fa7);
    for (Byte& byte:random_bytes) byte=Byte(io.random(io.user));
    io.seed(io.user,io.time32(io.user));
    Byte repeated[64];
    unsigned p=0;
    for (Byte& byte:repeated) {
        if (!password[p]) p=0;
        byte=password[p++];
    }
    initialize(state.hashes[0]);
    U32 hash[5];
    update(state,0,repeated,hash);
    std::memset(state.hashes,0,sizeof(state.hashes));
    const Byte* mask=reinterpret_cast<const Byte*>(hash);
    for (unsigned i=0;i<112;++i) random_bytes[i]^=mask[i%20];
    for (unsigned i=0;i<3;++i) {
        if (forward) io.expand(random_bytes+16*i,state.schedules[i]);
        else {
            std::uint16_t temporary[52];
            io.expand(random_bytes+16*i,temporary);
            io.invert(temporary,state.schedules[i]);
            std::memset(temporary,0,sizeof(temporary));
        }
        initialize(state.hashes[i]);
        // All three hash contexts consume the SAME random suffix at offset 48.
        update(state,i,random_bytes+48,nullptr);
    }
}

// 1.13c BNClient+0x15E70 -- EAX password, stack buffer and byte length.
U32 decrypt(State& state, const Imports& io, Byte* buffer, U32 length, const Byte* password) {
    if (!buffer) die(io,0x91);
    if (!password) die(io,0x92);
    setup(state,io,password,0);
    if (length<9 || ((length-8)&63)) return 0; // retail leaves setup state here
    U32 left=length-8, block_index=0;
    Byte* current=buffer;
    while (left) {
        Byte original[64], intermediate[64], result[64];
        std::memcpy(original,current,64);
        U32 saved_hash[5];
        digest(state,0,saved_hash);
        if (!(block_index&7)) {
            for (unsigned i=0;i<64;i+=8)
                io.decrypt8(original+i,state.schedules[0],intermediate+i);
        } else std::memcpy(intermediate,current,64);
        const Byte* mask=reinterpret_cast<const Byte*>(saved_hash);
        for (unsigned i=0;i<64;++i) result[i]=Byte(intermediate[i]-mask[(63-i)%20]);
        if (!(block_index&15)) update(state,0,result,nullptr);
        std::memset(saved_hash,0,sizeof(saved_hash));
        std::memcpy(current,result,64);
        current+=64; left-=64; ++block_index;
    }
    U32 expected, actual=state.hashes[0].digest[0];
    std::memcpy(&expected,current,4);
    U32 result=(!current[4] && expected==actual) ? length-0x48u+current[5] : 0;
    std::memset(state.schedules,0,sizeof(state.schedules));
    std::memset(state.hashes,0,sizeof(state.hashes));
    return result;
}

// 1.13c BNClient+0x16070 -- EAX buffer, EDI length.
U32 validate(State& state, const Imports& io, Byte* buffer, U32 length) {
    Byte password[20];
    random_key(io,password,20);
    U32 size=decrypt(state,io,buffer,length,password);
    if (!size || size>=length) return 0;
    buffer[size]=0;
    U32 limit=std::min(size+1,128u);
    for (U32 i=0;i<limit;++i) if (!buffer[i]) return 1;
    return 0;
}

// 1.13c BNClient+0x160E0 -- ECX filename, EAX size output. `data` starts as ECX (push ecx reserves
// the slot), so a Storm load that writes nothing leaves data == path.
template <class Valid>
Byte* load_with(const Imports& io, const char* path, U32* size, Valid valid) {
    Byte* data=reinterpret_cast<Byte*>(const_cast<char*>(path));
    *size=0x400;
    io.load(io.user,path,&data,size,1,0);
    if (data && *size) {
        if (valid(data,*size)) return data;
        io.release(io.user,data);
    }
    return nullptr;
}
Byte* load(State& state, const Imports& io, const char* path, U32* size) {
    return load_with(io,path,size,[&](Byte* d,U32 n){ return validate(state,io,d,n); });
}

// 1.13c BNClient+0x162B0 -- initialize the .gid/.clh resource buffers.
void load_resources(State& state, const Imports& io) {
    if (state.gid || state.clh) die(io,0x213);
    char path[260];
    std::snprintf(path,sizeof(path),"%s\\%s.gid","font","font");
    state.gid=load(state,io,path,&state.gid_size);
    // These are retail's indexed-alphabet fallback paths, decoded below instead
    // of preserving the compiler's overlapping byte-array stack temporaries.
    constexpr char alphabet[]="vwaxdnirosucflbgt\\.mpyeh8";
    auto fallback=[&](const Byte* indices, U32 count) {
        for (U32 i=0;i<count;++i) path[i]=alphabet[indices[i]];
        path[count]=0;
    };
    if (!state.gid) {
        constexpr Byte indices[]={4,2,16,2,17,15,13,8,14,2,13,17,9,12,3,17,11,10,7,9,
                                 8,7,17,1,2,0,6,5,4,3,18,1,2,0};
        fallback(indices,sizeof(indices));
        state.gid=load(state,io,path,&state.gid_size);
    }
    std::snprintf(path,sizeof(path),"%s\\%s.clh","font","font");
    state.clh=load(state,io,path,&state.clh_size);
    if (!state.clh) {
        constexpr Byte indices[]={4,2,16,2,17,15,13,8,14,2,13,17,9,12,3,17,11,10,7,9,
                                 8,7,17,11,10,7,6,5,4,3,18,1,2,0};
        fallback(indices,sizeof(indices));
        state.clh=load(state,io,path,&state.clh_size);
    }
    if (io.extra_enabled(io.user)) {
        constexpr Byte indices[]={4,2,16,2,17,15,13,8,14,2,13,17,11,23,2,7,9,17,2,19,
                                 17,11,8,12,17,2,19,14,13,3,14,8,1,18,11,8,12};
        fallback(indices,sizeof(indices));
        state.extra=load(state,io,path,&state.extra_size);
    }
    state.resource_flag=0;
    for (char* p=path;*p;++p) *p=0;
}

// 1.13c BNClient+0x16F40 -- broken SHA used by BNCS, distinct from grid SHA.
void broken_compress(U32* state) {
    U32 words[80];
    std::copy_n(state+5,16,words);
    for (unsigned i=16;i<80;++i) {
        unsigned n=(words[i-3]^words[i-8]^words[i-16]^words[i-14])&31;
        words[i]=std::rotl(U32(1),int(n));
    }
    rounds(state,words,false);
}

// 1.13c BNClient+0x172C0 -- stack output/input/length; zero-pad final block.
void broken_hash(U32* output, const void* input, U32 length) {
    U32 state[21];
    std::copy_n(initial,5,state);
    for (U32 offset=0;offset<length;offset+=64) {
        U32 count=std::min(length-offset,64u);
        // Retail never advances param_2. Multi-block calls hash the input prefix
        // again, with the final prefix zero padded; do not fix this to SHA-1.
        std::memcpy(state+5,input,count);
        std::memset(reinterpret_cast<Byte*>(state+5)+count,0,64-count);
        broken_compress(state);
    }
    std::copy_n(state,5,output);
}

struct ExceptionLink { ExceptionLink* previous; };
struct ContinuationFrame { void* saved_stack; void* frame_at_plus_12; };
struct ContinuationImports {
    void* user;
    // wasm/native cannot assign x86 ESP/EBP or jump out of a C++ frame. This
    // adapter must perform a nonlocal transfer with all retail live registers.
    void (*resume)(void*, void(*)(), void*, void*, ContinuationFrame*);
};
// 1.13c BNClient+0x1750D -- _JumpToContinuation: pop FS:0, restore ESP/EBP/EBX.
void jump_to_continuation(ExceptionLink*& exception_list,
                                     void (*target)(), ContinuationFrame* registration,
                                     const ContinuationImports& io) {
    exception_list=exception_list->previous;
    io.resume(io.user,target,registration->saved_stack,
              registration->frame_at_plus_12,registration);
    std::abort();
}
} // namespace d2::bnclient::grid

// FN-probe seam for BNClient+0x160E0: Storm load/release and the 0x16070 check are the caller's recorders.
std::uint8_t* bn_load_160e0(const char* path, std::uint32_t* size,
    void (*load)(const char*, std::uint8_t**, std::uint32_t*), void (*release)(std::uint8_t*),
    bool (*valid)(std::uint8_t*, std::uint32_t)) {
    using namespace d2::bnclient::grid;
    static void (*l)(const char*, Byte**, U32*); static void (*r)(Byte*);
    l=load; r=release;
    Imports io{}; io.user=nullptr;
    io.load=[](void*,const char* n,Byte** d,U32* z,U32,U32){ l(n,d,z); };
    io.release=[](void*,Byte* d){ r(d); };
    return load_with(io,path,size,valid);
}


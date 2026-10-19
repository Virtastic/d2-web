// src/core/d2client/d2client_batch_2a700.cpp -- twelve UNMAPPED 1.13c D2Client rows transcribed from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c (Stage A / RE-DONE lane). D2MOO has no D2Client module;
// OpenDiablo2's stateful input model is only a cross-check; no D2Client implementation exists in D2MOO.
// Import and internal callees not yet recovered are explicit
// host seams. No retail data or Ghidra output is included here.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

// D2Client call gate. Arguments keep the retail register/stack order where the cached decompile
// exposes it; `out` lets a host seam return pointer-like values without widening retail's 32-bit ABI.
struct D2ClientBatch2a700Call {
    uint32_t rva, eax, ecx, edx, ebx, esi;
    uint32_t stack[4], out[4];
    uintptr_t stack_ptr[4];
};
using D2ClientBatch2a700Host = uint32_t (*)(D2ClientBatch2a700Call*);
static uint32_t invoke(D2ClientBatch2a700Host host, uint32_t rva,
                       uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0,
                       uint32_t ebx = 0, uint32_t esi = 0,
                       uint32_t s0 = 0, uint32_t s1 = 0,
                       uint32_t s2 = 0, uint32_t s3 = 0) {
    D2ClientBatch2a700Call c{rva,eax,ecx,edx,ebx,esi,{s0,s1,s2,s3},{0,0,0,0}};
    return host(&c);
}

// 1.13c D2Client+0x2A700 (65 B): EAX = unit. Run 0x2A0B0; when the first D2Win predicate is true,
// notify 0x74480; test the predicate again before 0x578F0; return the inverse of Ordinal 10342.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2a700(
    D2ClientBatch2a700Host host, uint32_t unit,
    int32_t (*ordinal10494)(), int32_t (*ordinal10342)()) {
    invoke(host,0x2A0B0,unit);
    if (ordinal10494()) invoke(host,0x74480,0,0,0,unit);
    if (ordinal10494()) invoke(host,0x578F0,0,0,0,0,unit);
    return ordinal10342() == 0;
}

struct D2ClientBatchUnit {
    int32_t type, pad4, pad8, padC, mode;
    uint32_t* playerData; // retail unit[5], fields +0x150..+0x15C
    uint32_t id;          // test seam identity, not an asserted retail field offset
    uint32_t flags;       // test seam for the recovered +0xC4 bit tests
};

// 1.13c D2Client+0x2AA40 (454 B): action enqueue/update tail shared by 0x2AE30/0x2AEA0/0x2AFB0.
// This body preserves both queue modes, their notification codes, and the four-word player-data
// clear/set. The `query`/`dispatch` seams represent the unresolved retail queue and packet helpers.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2aa40(
    D2ClientBatch2a700Host host, D2ClientBatchUnit* unit, uint32_t action,
    int32_t arg, uint32_t payload, int32_t queue_busy, int32_t queue_locked,
    int32_t (*query)(void), void (*dispatch)(const uint8_t*, uint32_t)) {
    if (!queue_busy && !queue_locked) {
        if (query()) {
            uint32_t out[5] = {0,0,0,0,0};
            struct { int32_t local_1c; uint32_t local_18; } args{arg, payload};
            D2ClientBatch2a700Call c{0x2A750,0,0,unit ? unit->id : 0,0,0,
                                     {(uint32_t)arg,payload,0,0},{0,0,0,0},{0,0,0,0}};
            c.stack_ptr[0] = reinterpret_cast<uintptr_t>(&args.local_1c);
            c.stack_ptr[1] = reinterpret_cast<uintptr_t>(&args.local_18);
            if (host(&c)) {
                // FUN_6fac43e0 receives &local_28: opcode at +0, param_2 at +1,
                // and param_3 at +5. Retail's unaligned 1+4+4 byte packet is 9 bytes.
                uint8_t packet[9] = {2};
                std::memcpy(packet + 1, &arg, sizeof arg);
                std::memcpy(packet + 5, &payload, sizeof payload);
                dispatch(packet,sizeof packet);
            }
            if (!unit) return;
            if (unit->type == 0 && unit->playerData)
                std::memset(unit->playerData+0x54,0,4*sizeof(uint32_t));
        }
    } else if (query()) {
        struct { int32_t local_1c; uint32_t local_18; } args{arg, payload};
        D2ClientBatch2a700Call c{0x2A750,0,0x18,unit ? unit->id : 0,0,0,
                                 {(uint32_t)arg,payload,0,0},{0,0,0,0},{0,0,0,0}};
        c.stack_ptr[0] = reinterpret_cast<uintptr_t>(&args.local_1c);
        c.stack_ptr[1] = reinterpret_cast<uintptr_t>(&args.local_18);
        if (host(&c)) {
            // Same packed local layout as the opcode-2 path, with opcode 4.
            uint8_t packet[9] = {4};
            std::memcpy(packet + 1, &arg, sizeof arg);
            std::memcpy(packet + 5, &payload, sizeof payload);
            dispatch(packet,sizeof packet);
        }
        if (!unit) return;
        if (unit->type == 0 && unit->playerData)
            std::memset(unit->playerData+0x54,0,4*sizeof(uint32_t));
    }
    if (unit && unit->type == 0 && unit->playerData) {
        unit->playerData[0x54]=1; unit->playerData[0x55]=action;
        unit->playerData[0x56]=(uint32_t)arg; unit->playerData[0x57]=payload;
    }
}

// 1.13c D2Client+0x2AC10 (514 B): EAX low word selects an action category; EDX is the unit id.
// The 0x80-byte action record is resolved by 0x54E20. Category-specific handlers are passed as
// seams, while the 200 ms throttle and message tail stay local and deterministic.
struct D2ClientBatch2ac10Env {
    D2ClientBatch2a700Host host;
    uint32_t* globals;
    uint8_t* table; // DAT_6fbba608, 0x80 bytes per kind
    uint32_t (*tick)(void);
    void (*message)(const uint8_t*, uint32_t);
    uint32_t* (*record)(uint32_t arg, uint8_t* slot, uint32_t kind); // D2Client+0x54E20
    void (*category0)(uint32_t* record);
    void (*category2)(uint32_t* record, uint32_t arg);
    void (*category4)(uint32_t* record, uint32_t arg, uint32_t flag);
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2ac10(
    uint32_t selector, uint32_t argument, D2ClientBatch2ac10Env* env) {
    uint32_t kind=selector&0xFFFFu;
    uint32_t* record=env->record(argument,env->table+kind*0x80u,kind);
    if (!record) return;
    if (kind==0) env->category0(record);
    else if (kind==1) {
        uint32_t now=env->tick();
        if (now-record[0x35] < 200) return;
        record[0x35]=now;
    } else if (kind==2) {
        invoke(env->host,0xC836,env->globals[0xB],0,0,0,0,0);
        if (record[0x31] & 4u) env->category2(record,argument);
    } else if (kind==4) {
        env->category4(record,argument,env->globals[0xAAD84]);
        return;
    }
    uint8_t packet[9] = {0x13,(uint8_t)kind,(uint8_t)(kind>>8),0,0,0,0,0,0};
    std::memcpy(packet+5,&argument,sizeof argument);
    env->message(packet,sizeof packet);
}

// 1.13c D2Client+0x2AE30 (100 B): when the unit supports queued actions, test 0x29E80 and enqueue
// through 0x2AA40; otherwise latch the two failure globals. Return values are retail's 0/1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2ae30(
    D2ClientBatch2a700Host host, D2ClientBatchUnit* unit, uint32_t action,
    int32_t arg, uint32_t payload, uint32_t* result_flags,
    void (*enqueue)(uint32_t,uint32_t,int32_t,uint32_t)) {
    if (unit && (unit->flags & 2u)) {
        if (invoke(host,0x29E80,unit->id)) {
            enqueue(action,unit->id,arg,payload);
            return 0;
        }
        return 1;
    }
    result_flags[0]=1; result_flags[1]=0;
    return 0;
}

// 1.13c D2Client+0x2AEA0 (245 B): validates action ids 5..0x12 against the corresponding resource
// table, then requires 0x29E80 before queueing. A rejected action sets failure globals and returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2aea0(
    D2ClientBatch2a700Host host, D2ClientBatchUnit* unit, uint32_t action,
    int32_t arg, uint32_t payload, uint32_t* result_flags,
    int32_t (*resource_ok)(uint32_t), void (*enqueue)(uint32_t,uint32_t,int32_t,uint32_t)) {
    bool mode_ok=false;
    if (unit && !(unit->flags & 0x10000u)) {
        if (unit->type==0) mode_ok=(unit->mode==0 || unit->mode==0x11);
        else if (unit->type==1) mode_ok=(unit->mode==0 || unit->mode==0xC);
        if (!mode_ok && !(unit->flags&2u)) goto rejected;
    }
    if ((action>=5 && action<=0xB) || (action>=0xC && action<=0x12)) {
        if (!resource_ok(action)) goto rejected;
    } else goto rejected;
    if (!invoke(host,0x29E80,unit?unit->id:0)) return 1;
    enqueue(action,unit?unit->id:0,arg,payload);
    return 0;
rejected:
    result_flags[0]=0; result_flags[1]=1;
    return 0;
}

// 1.13c D2Client+0x2AFB0 (220 B): unit queue compatibility gate. A failed queue lookup or mismatched
// resource class latches failure; a valid unit reaches 0x2A7C0. The unresolved object lookups are seams.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2afb0(
    D2ClientBatch2a700Host host, D2ClientBatchUnit* unit, uint32_t action,
    uint32_t object_id, uint32_t payload, int32_t arg, uint32_t* result_flags,
    int32_t (*lookup)(uint32_t,uint32_t*), int32_t (*class_ok)(uint32_t,uint32_t)) {
    uint32_t object_class=0;
    if (unit && !(unit->flags&0x10000u)) {
        bool allowed=(unit->type==0 && (unit->mode==0 || unit->mode==0x11)) ||
                     (unit->type==1 && (unit->mode==0 || unit->mode==0xC));
        if (!allowed && !(unit->flags&2u)) goto failed;
        if (!invoke(host,0x29E80,unit->id)) return 1;
        if (unit->flags&2u) { invoke(host,0x2AA40,action,0,unit->id,0,0,arg,payload); return 0; }
    }
    if (!lookup(object_id,&object_class)) goto failed;
    if (object_class && !class_ok(object_class,1)) goto failed;
    invoke(host,0x2A7C0,action);
    return 0;
failed:
    result_flags[0]=0; result_flags[1]=1;
    return 0;
}

// 1.13c D2Client+0x2B090 (326 B): pending-action worker. It validates the unit/player-data fields,
// restores the queued tuple, resolves the matching record, then dispatches by queue kind. The last
// queue action is performed only on a nonzero handler result (except the kind-2 fallback).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2b090(
    D2ClientBatch2a700Host host, D2ClientBatchUnit* unit,
    D2ClientBatch2ac10Env* env, uint32_t* cooldown,
    uint32_t (*dispatch0)(uint32_t*,D2ClientBatchUnit*,uint32_t,uint32_t),
    uint32_t (*dispatch1)(uint32_t,uint32_t,uint32_t),
    uint32_t (*dispatch2)(uint32_t,uint32_t)) {
    if (!unit || unit->type!=0 || !unit->playerData) return;
    uint32_t* pd=unit->playerData;
    if (!pd[0x54]) return;
    uint32_t action=pd[0x55], kind=pd[0x56], value=pd[0x57];
    uint32_t* record=env->record(value,env->table+kind*0x80u,kind);
    invoke(host,0x16A0);
    if (action==0x13 && kind==5) *cooldown=env->globals[0x97EC]+500;
    if (!record || action==0) return;
    uint32_t ok=0;
    if (kind==0) ok=dispatch0(record,unit,action,0);
    else if (kind==1) ok=dispatch1(action,1,value);
    else if (kind==2) ok=dispatch2(2,value);
    else {
        if ((record[0x31]&2u)==0) {
            env->globals[0xCC2F8]=1; env->globals[0xCC2F4]=0; return;
        }
        invoke(host,0x2A7C0,action);
        return;
    }
    if (ok) invoke(host,0x2A7C0,action);
}

// 1.13c D2Client+0x2B420 (153 B): the path-facing imports/helper are seams; the byte copy, append
// order, helper sequence, and literal
// return follow the cached 1.13c body. The literal is the function's actual return, not the archive
// helper's result.
struct D2ClientBatch2b420Env {
    const char* (*ordinal10070)(void);
    uint32_t (*ordinal10016)(const char*);
    int32_t (*helper_bd810)(uint32_t, uint32_t*);
    void (*ordinal10006)(int32_t, uint32_t*, const char*, uint32_t, uint32_t, char*);
};
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_2b420(
    const char* path, uint32_t arg, const uint8_t acStack_108_initial[4],
    D2ClientBatch2b420Env* env) {
    struct { uint32_t uStack_10c; char acStack_108[4]; char local_104[260]; } frame;
    frame.uStack_10c = arg;
    std::memcpy(frame.acStack_108, acStack_108_initial, sizeof frame.acStack_108);
    if (path) {
        size_t i = 0;
        for (; i + 1 < sizeof frame.local_104 && path[i]; ++i) frame.local_104[i] = path[i];
        frame.local_104[i] = '\0';
    }
    const char* root = env->ordinal10070();
    // Preserve the four stack bytes preceding local_104 as caller-supplied state. Retail scans
    // from (&uStack_10c + 3) across acStack_108 and local_104, then appends the returned root at
    // that exact NUL. Do not clear or relocate the prefix: whether the resulting string includes
    // the copied input path depends on those captured bytes.
    char* pcVar8 = reinterpret_cast<char*>(&frame.uStack_10c) + 3;
    char* pcVar7;
    do {
        pcVar7 = pcVar8 + 1;
        pcVar8 = pcVar8 + 1;
    } while (*pcVar7 != '\0');
    if (root) {
        size_t i = 0;
        for (; root[i]; ++i) pcVar8[i] = root[i];
        pcVar8[i] = '\0';
    }
    char* append_start = reinterpret_cast<char*>(&frame.uStack_10c) + 4;
    uint32_t file = env->ordinal10016(append_start);
    int32_t archive = env->helper_bd810(file, nullptr);
    char helper_out[4]{};
    uint32_t archive_arg = 0;
    env->ordinal10006(archive, &archive_arg,
        "..\\Source\\D2Client\\CORE\\ARCHIVE.CPP", 0x56, 0xFFFFFFFFu, helper_out);
    return "..\\Source\\D2Client\\CORE\\ARCHIVE.CPP";
}

// 1.13c D2Client+0x2C3D0 (285 B): the item descriptor at +0x60 is resolved with Ordinal 10061;
// Ordinals 11160/10164/10621/10910/10653/10973/10402/11038 drive the stat flow. The three output
// pairs are descriptor-backed short records written in the original order, not a synthetic flag word.
struct D2ClientBatch2c3d0Env {
    void* (*ordinal10061)(uint32_t);
    int32_t (*ordinal11160)(void*, uint32_t**);
    void (*ordinal10164)(void*, void*, uint32_t);
    void (*ordinal10164_noargs)(void);
    uint16_t* (*ordinal11127)(void);
    int32_t (*ordinal10621)(void*);
    int32_t (*ordinal10910)(void*, uint32_t, uint32_t);
    int32_t (*ordinal10653)(uint32_t, void*);
    int32_t (*ordinal10973)(void*, uint32_t, uint32_t);
    int32_t (*ordinal10402)(void*);
    int32_t (*ordinal11038)(void*);
    uint8_t* bit_masks;
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2c3d0(
    void* item, int32_t enabled, int32_t* value, uint32_t* flags,
    uint16_t* unaff_ESI,
    D2ClientBatch2c3d0Env* env) {
    void* descriptor = nullptr;
    if (item) {
        uint32_t handle = 0;
        std::memcpy(&handle, static_cast<uint8_t*>(item) + 0x60, sizeof handle);
        if (handle) descriptor = env->ordinal10061(handle);
    }
    *value = 0;
    *flags = 0;
    if (!enabled) return;

    uint32_t* lookup = nullptr;
    int32_t found = descriptor ? env->ordinal11160(descriptor, &lookup) : 0;
    if (descriptor) env->ordinal10164(item, descriptor, 0);
    if (found) env->ordinal10164_noargs();
    *value = env->ordinal10621(item);

    uint16_t* descriptor_a = nullptr;
    uint16_t* descriptor_b = nullptr;
    uint16_t* descriptor_c = nullptr;
    if (found) {
        descriptor_a = unaff_ESI;
        descriptor_b = static_cast<uint16_t*>(descriptor);
        descriptor_c = static_cast<uint16_t*>(item);
    } else {
        descriptor_a = static_cast<uint16_t*>(descriptor);
        descriptor_b = static_cast<uint16_t*>(item);
        env->ordinal11127(); // Return value is ignored by retail.
        descriptor_c = reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(0x6fadc44a));
    }
    uint32_t descriptor_stat = descriptor_a ? descriptor_a[0] : 0;
    int32_t pct = env->ordinal10910(item, 0x77, 0) +
        env->ordinal10653(descriptor_stat, item);
    uint8_t descriptor_mask = descriptor_a
        ? *reinterpret_cast<uint8_t*>(descriptor_a + 2) : 0;
    if (descriptor && env->bit_masks && (env->bit_masks[0xC] & descriptor_mask)) {
        int32_t extra = env->ordinal10973(item, 0x145, 0);
        if (extra) {
            pct += extra;
            if (descriptor_a) { descriptor_a[0] = 3; descriptor_a[1] = 0; }
        }
    }
    *value = (*value * pct) / 100 + *value;
    if (env->ordinal10402(item) && descriptor_b) { descriptor_b[0] = 3; descriptor_b[1] = 0; }
    if (env->ordinal11038(item) && descriptor_c) { descriptor_c[0] = 1; descriptor_c[1] = 0; }
}

// 1.13c D2Client+0x2C4F0 (291 B): variant descriptor pass. Rejects a missing item/descriptor or a
// failed descriptor lookup, initializes the output value, then applies base, percentage and flag stats.
struct D2ClientBatchStatEnv {
    int32_t (*base)(void*); int32_t (*percent)(void*,uint32_t);
    int32_t (*flag)(void*,uint32_t,uint32_t); int32_t (*special)(void*);
    uint8_t* mask; uint32_t mask_bit;
};
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2c4f0(
    void* item, void* descriptor, int32_t* value, uint32_t* flags,
    int32_t enabled, D2ClientBatchStatEnv* env) {
    *flags=0;
    if (!descriptor || !enabled) return 0;
    *value=env->base(item);
    int32_t pct=env->percent(item,0x77)+env->percent(item,0xC0)+env->percent(item,0x106);
    int32_t extra=env->flag(item,0x145,0);
    if (extra && env->mask && (env->mask[0xC]&env->mask_bit)) pct+=extra;
    *value += (*value*pct)/100;
    if (env->special(item)) *flags |= 1u;
    if (env->flag(item,0x11038,0)) *flags |= 2u;
    return 1;
}

// 1.13c D2Client+0x2C6A0 (236 B): format a signed min/max range. Values above 9999 use the K form;
// the UI's sign and K-prefix strings are selected from the two module literals before Unicode copy.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2c6a0(
    int32_t bonus, int32_t range_mode, int32_t minimum, int32_t maximum, char* output,
    void (*to_unicode)(void*,const char*,uint32_t), void* unicode_target,
    const char* plus, const char* plain, const char* kilo) {
    const char* prefix=bonus?plus:plain;
    if (range_mode && minimum>9999) std::snprintf(output,32,"%s%dK-%dK",kilo,(minimum+500)/1000,(maximum+500)/1000);
    else if (range_mode && maximum>9999) std::snprintf(output,32,"%s%d-%dK",kilo,minimum,(maximum+500)/1000);
    else std::snprintf(output,32,"%s%d-%d",prefix,minimum,maximum);
    to_unicode(unicode_target,output,0x20);
}

// 1.13c D2Client+0x2C790 (73 B): format a signed decimal range and convert the 32-byte buffer to
// Unicode. Negative values suppress the plus sign exactly as the cached branch condition specifies.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2c790(
    int32_t bonus, int32_t minimum, int32_t maximum, char* output,
    void (*to_unicode)(void*,const char*,uint32_t), void* unicode_target) {
    std::snprintf(output,32,(bonus && minimum>=0)?"+%d.%d":"%d.%d",minimum,maximum);
    to_unicode(unicode_target,output,0x20);
    return 1;
}

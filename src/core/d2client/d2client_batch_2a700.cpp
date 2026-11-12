// src/core/d2client/d2client_batch_2a700.cpp -- twelve UNMAPPED 1.13c D2Client rows transcribed from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c (Stage A / RE-DONE lane). D2MOO has no D2Client module;
// OpenDiablo2's stateful input model is only a cross-check; no D2Client implementation exists in D2MOO.
// Import and internal callees not yet recovered are explicit
// host seams. No retail data or Ghidra output is included here.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>
#include "file_services.h"

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
    uint8_t* table; // DAT_6fbba608, 0x200 bytes per kind (shl edx,9)
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
    uint32_t* record=env->record(argument,env->table+kind*0x200u,kind);
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

// 1.13c D2Client+0x2AE30 moved to d2client_2ae30.cpp (faithful register/stack form).

// 1.13c D2Client+0x2AEA0 (245 B): EAX = unit, ESI = unit id word, stack (ret 0xC) = action, p1, p2. Unit flag bit 16 (+0xC4) or an allowed type/mode
// (type 0: mode 0/0x11, type 1: mode 0/0xC) go to the resource check; otherwise flag bit 1 alone decides. Actions 5..0xB resolve through 0xC5BC(esi),
// 0xC..0x12 through 0xC5CE(esi); the id goes to 0xC42A (UnitMode.cpp:0x44F), a row of the 0x23C-stride table at sgptDataTables+0xB98 (count +0xBA0) must
// exist and its byte +7 must intersect the first byte of gdwBitMasks. Then 0x29E80(EAX = action, ECX = esi): zero returns 1, else 0x2AA40(EAX = esi; action, p1, p2) and 0.
// A refusal sets [0x11C2F4] = 0, [0x11C2F8] = 1 and returns 0.
struct D2c2aea0Env {
    uint32_t (*c5bc)(uint32_t esi);
    uint32_t (*c5ce)(uint32_t esi);
    int32_t (*c42a)(uint32_t id);                       // Ordinal_10601 (id, "..\\Source\\D2Client\\UNIT\\UnitMode.cpp", 0x44F)
    const uint8_t* tbl;                                 // *sgptDataTables
    const uint8_t* mask;                                // *gdwBitMasks
    uint32_t (*c29e80)(uint32_t eax, uint32_t ecx);
    void (*c2aa40)(uint32_t esi, uint32_t action, uint32_t p1, uint32_t p2);
    uint32_t* g_c2f4;
    uint32_t* g_c2f8;
};
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2aea0(
    const uint8_t* unit, uint32_t esi, uint32_t action, uint32_t p1, uint32_t p2, const D2c2aea0Env* env) {
    uint32_t flags; std::memcpy(&flags, unit + 0xC4, 4);
    int32_t type, mode; std::memcpy(&type, unit, 4); std::memcpy(&mode, unit + 0x10, 4);
    bool pass;
    if (flags & 0x10000u) goto resource;
    if (type == 0) { if (mode == 0 || mode == 0x11) goto resource; }
    else if (type == 1 && (mode == 0 || mode == 0xC)) goto resource;
    pass = ((flags >> 1) & 1u) != 0;
    goto decided;
resource: {
        uint32_t id;
        if (action >= 5 && action <= 0xB) id = env->c5bc(esi);
        else if (action >= 0xC && action <= 0x12) id = env->c5ce(esi);
        else id = 0;
        if (id == 0) goto refuse;
        int32_t idx = env->c42a(id);
        if (idx < 0) goto refuse;
        int32_t cnt; std::memcpy(&cnt, env->tbl + 0xBA0, 4);
        if (idx >= cnt) goto refuse;
        uint32_t rows; std::memcpy(&rows, env->tbl + 0xB98, 4);
        const uint8_t* row = (const uint8_t*)(uintptr_t)(rows + (uint32_t)idx * 0x23Cu);
        if (!row) goto refuse;
        pass = (*env->mask & row[7]) != 0;
    }
decided:
    if (!pass) goto refuse;
    if (!env->c29e80(action, esi)) return 1;
    env->c2aa40(esi, action, p1, p2);
    return 0;
refuse:
    *env->g_c2f4 = 0; *env->g_c2f8 = 1;
    return 0;
}

// old test-seam port of D2Client 2AFB0 (220 B; the retail port is d2client_2afb0.cpp): `unit` is the thiscall object (the 0x80-byte kind record from 0x54E20: type @0, mode @0x10, flags @0xC4),
// `target_id` is param_1->+0xC (or -1 when param_1 is null). Allowed type/mode (0/0|0x11, 1/0|0xC) or
// flags&0x10000 skip straight to the record check; otherwise flags&2 selects the 0x9E80 test and the
// 0x2AA40 enqueue, else the failure latch. `lookup` is 0x4D630 (fills the record's +0x10), `class_ok` is 0x4FD90.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2afb0(
    D2ClientBatch2a700Host host, D2ClientBatchUnit* unit, uint32_t action,
    uint32_t target_id, int32_t arg, uint32_t* result_flags,
    int32_t (*lookup)(uint32_t*), int32_t (*class_ok)(uint32_t,uint32_t)) {
    if (!(unit->flags&0x10000u)) {
        bool direct=false;
        if (unit->type==0) direct=(unit->mode==0 || unit->mode==0x11);
        else if (unit->type==1) direct=(unit->mode==0 || unit->mode==0xC);
        if (!direct) {
            if (!(unit->flags&2u)) { result_flags[0]=0; result_flags[1]=1; return 0; }
            if (!invoke(host,0x29E80,target_id)) return 1;
            invoke(host,0x2AA40,target_id,0,0,0,0,action,(uint32_t)arg); // EAX = param_1; stack = param_2, param_3
            return 0;
        }
    }
    uint32_t field10=0;
    if (!lookup(&field10)) { result_flags[1]=1; result_flags[0]=0; return 0; }
    if (target_id!=field10 && !class_ok(field10,1)) { result_flags[0]=0; result_flags[1]=1; return 0; }
    invoke(host,0x2A7C0,action);
    return 0;
}

// old test-seam port of D2Client 2B090 (326 B; the retail port is d2client_2b090.cpp): pending-action worker. It validates the unit/player-data fields,
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
    uint32_t* record=env->record(value,env->table+kind*0x200u,kind);
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
    const char* file = env->ordinal10016(append_start);
    void* archive = env->helper_bd810(file, nullptr);
    void* archive_arg = nullptr; // retail reuses uStack_10c, zeroed, as the out slot
    env->ordinal10006(archive, &archive_arg,
        "..\\Source\\D2Client\\CORE\\ARCHIVE.CPP", 0x56, 0xFFFFFFFFu, frame.acStack_108);
    return "..\\Source\\D2Client\\CORE\\ARCHIVE.CPP";
}

// 1.13c D2Client+0x2C3D0: moved to d2client_2c3d0.cpp (the unit, gate and stack words as retail passes them).

// 1.13c D2Client+0x2C4F0: moved to d2client_2c4f0.cpp (EAX item, ESI unit and the four stack words as retail passes them).

// 1.13c D2Client+0x2C6A0 (236 B): ECX bonus, EDX range_mode, stack unicode target, ESI minimum, EAX maximum.
// The K forms prefix the module string DAT_6fb86a30 (`plus_str`, bonus set) or DAT_6fb8359f (`plain_str`);
// the plain form is the literal "+%d-%d" / "%d-%d". Strings are read from the module, so they are arguments.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_2c6a0(
    int32_t bonus, int32_t range_mode, int32_t minimum, int32_t maximum, char* output,
    void (*to_unicode)(void*,const char*,uint32_t), void* unicode_target,
    const char* plus_str, const char* plain_str) {
    if (range_mode && minimum>9999)
        std::snprintf(output,32,"%s%dK-%dK",bonus?plus_str:plain_str,(minimum+500)/1000,(maximum+500)/1000);
    else if (range_mode && maximum>9999)
        std::snprintf(output,32,"%s%d-%dK",bonus?plus_str:plain_str,minimum,(maximum+500)/1000);
    else std::snprintf(output,32,bonus?"+%d-%d":"%d-%d",minimum,maximum);
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

// D2Multi batch n37: 7B5F 8C80 8D80 8E70 9000 90D0 9550 9D60 AA60 ACB0 B160 B970. Retail globals named by address; host calls are seams.
// ponytail: decompiler hides register args (EAX/ESI/EBX); roles assumed from callers, low confidence on seam arg lists.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>

extern "C" {
#pragma clang attribute push(__attribute__((weak)), apply_to=function)
void n37_Ordinal_10000(...); void n37_Ordinal_10001(...); int32_t n37_Ordinal_10003(...);
intptr_t n37_Ordinal_10020(intptr_t); void n37_Ordinal_10022(...); void n37_Ordinal_10024(...);
void n37_Ordinal_10031(...); int32_t n37_Ordinal_10037(...); void n37_Ordinal_10053(...);
int32_t n37_Ordinal_10080(...); int32_t n37_Ordinal_10098(...); int32_t n37_Ordinal_10101(...);
intptr_t n37_Ordinal_10111(...); int32_t n37_Ordinal_10126(...); void n37_Ordinal_10131(...);
int32_t n37_Ordinal_10140(...); void n37_Ordinal_10147(...); int32_t n37_Ordinal_10154(...);
void* n37_Ordinal_10166(...); void n37_Ordinal_10167(...); void n37_Ordinal_10170(...);
int32_t n37_Ordinal_10172(...); int32_t n37_Ordinal_10179(...); void n37_Ordinal_10185(...);
void n37_Ordinal_10196(...); intptr_t n37_Ordinal_10202(...); int32_t n37_Ordinal_10010(...);
int32_t n37_Ordinal_10021(...); int32_t n37_Ordinal_10227(...); uint32_t n37_Ordinal_10265(...);
#pragma clang attribute pop
void d2_multi_sub_8790(void); __attribute__((weak)) void n37_cb_8790(void) { d2_multi_sub_8790(); }
void d2_multi_unicode_strcpy(void* dst, const void* src);
void d2_multi_utf8ToUnicode(void* dst, const char* src, int32_t n);
void d2_multi_unicode2Win(char* dst, const void* src, int32_t n);
void d2_multi_toUtf(char* dst, const void* src, int32_t n);
// host seams: registry read, date format
int32_t d2_host_reg_query(const char* key, const char* val, uint8_t* out, uint32_t* size);
int32_t d2_host_date_format(const void* filetime, char* out, int32_t n);
extern int32_t DAT_6fa09f24, DAT_6fa09f2c, DAT_6fa09f38, DAT_6fa09f88;
extern int32_t DAT_6fa0a0e0;
extern int32_t DAT_6fa0917c;
extern uint8_t* DAT_6fa09f14;
extern char* DAT_6fa0a0f0;
extern int32_t DAT_6fa08e80[];
extern char DAT_6f9ea360[], DAT_6f9ea390[], DAT_6f9ea3c0[], DAT_6f9ea3f0[], DAT_6f9e75d7[];
extern char DAT_6f9ea7b0[], DAT_6f9ea7e0[], DAT_6f9ea810[], DAT_6f9ea840[], DAT_6f9ea900[], DAT_6f9eaff0[], DAT_6f9ea870[], DAT_6f9ea8d0[];
extern int32_t DAT_6fa0a018, DAT_6fa0a01c, DAT_6fa0a020, DAT_6fa0a024, DAT_6fa0a028, DAT_6fa0a02c, DAT_6fa0a030, DAT_6fa0a038;
extern uint32_t d2_host_tick(void);
extern void d2_host_exit(int32_t);
extern int32_t DAT_6fa09f40;
extern int32_t DAT_6fa09f44;
extern int32_t DAT_6fa09f48;
extern int32_t DAT_6fa09f4c;
extern int32_t DAT_6fa09f50;
extern int32_t DAT_6fa09f54;
extern int32_t DAT_6fa09f58;
extern int32_t DAT_6fa09f5c;
extern int32_t DAT_6fa09f60;
extern int32_t DAT_6fa09f64;
extern int32_t DAT_6fa09f68;
extern int32_t DAT_6fa09f6c;
extern int32_t DAT_6fa09f70;
extern int32_t DAT_6fa09f74;
extern int32_t DAT_6fa09f78;
extern int32_t DAT_6fa09f8c;
extern int32_t DAT_6fa09f90;
extern int32_t DAT_6fa09f94;
extern int32_t DAT_6fa09f98;
extern int32_t DAT_6fa09f9c;
extern int32_t DAT_6fa09fa0;
extern int32_t DAT_6fa09fa4;
extern int32_t DAT_6fa09fa8;
extern int32_t DAT_6fa09fac;
extern int32_t DAT_6fa09fb0;
extern int32_t DAT_6fa09fb4;
extern int32_t DAT_6fa09fb8;
extern int32_t DAT_6fa09fbc;
extern int32_t DAT_6fa09fc0;
extern int32_t DAT_6fa09fc4;
extern int32_t DAT_6fa09fc8;
extern int32_t DAT_6fa09fcc;
extern int32_t DAT_6fa09fd0;
extern int32_t DAT_6fa09ff4;
extern int32_t DAT_6fa09ff8;
extern int32_t DAT_6fa09ffc;
extern int32_t DAT_6fa0a000;
extern int32_t DAT_6fa0a004;
extern int32_t DAT_6fa0a008;
extern int32_t DAT_6fa0a00c;
extern int32_t DAT_6fa0a010;
extern int32_t DAT_6fa0a014;
extern int32_t DAT_6fa0a048;
extern int32_t DAT_6fa0a04c;
extern int32_t DAT_6fa0a054;
extern int32_t DAT_6fa0a068;
extern int32_t DAT_6fa0a06c;
extern int32_t DAT_6fa0a070;
extern int32_t DAT_6fa0a074;
extern int32_t DAT_6fa0a078;
extern int32_t DAT_6fa0a07c;
extern int32_t DAT_6fa0a080;
extern int32_t DAT_6fa0a084;
extern int32_t DAT_6fa0a088;
}

// 1.13c D2Multi+0x7B5F
void d2_multi_sub_7b5f(uint32_t* p) {
    if ((p[3] & 0x83) != 0 && (p[3] & 8) != 0) {
        free((void*)(uintptr_t)p[2]);
        *(uint16_t*)(p + 3) &= 0xfbf7;
        p[0] = 0; p[2] = 0; p[1] = 0;
    }
}

// 1.13c D2Multi+0x8C80 (FUN_6f9d8c80, ret 8): EAX = text, edited in place (each line NUL-ended, restored after); stack: p1, flag.
// Each line is one D2Win #10022 call (thunk 0x7C5C) with ECX = p1 and EDX = the line start; with flag set the call repeats with
// EDX = the module constant 0x6F9E75D7 (not text). The empty text is one call with EDX = 0x6F9E75D7.
extern "C" void d2_multi_sub_8c80_r(int32_t p1, int32_t flag, char* s) {
    if (strlen(s) == 0) { n37_Ordinal_10022(p1, (int32_t)0x6F9E75D7); return; }
    while (*s) {
        char* e = s; while (*e && *e != '\n') e++;
        char* nl = (*e == '\n') ? e : nullptr;
        if (nl) *nl = 0;
        n37_Ordinal_10022(p1, (int32_t)(uintptr_t)s);
        if (flag) n37_Ordinal_10022(p1, (int32_t)0x6F9E75D7);
        if (nl) *nl = '\n';
        if (!*e) return;
        s = nl ? nl + 1 : e;
    }
}
// batch_16 (B1F0) calls with flag and no text: the port runs on an empty line.
extern "C" void d2_multi_sub_8c80(int32_t p1, int32_t flag) { char empty[1] = { 0 }; d2_multi_sub_8c80_r(p1, flag, empty); }

// 1.13c D2Multi+0x8D80 (FUN_6f9d8d80, ret): EAX = sel picks the string id (0-6: 0xFAB, 0xFAA, 0xFA9, 0xFA8, 0xFA7, 0x2771, 0x2772;
// default 0x1507); ESI = destination. D2Lang #10003 (ECX = id) answers the string; Unicode::strcpy (ECX = dst, EDX = src).
extern "C" void d2_multi_sub_8d80_r(int32_t sel, void* dst) {
    static const uint32_t ids[7] = { 0xFAB, 0xFAA, 0xFA9, 0xFA8, 0xFA7, 0x2771, 0x2772 };
    const uint32_t id = (sel >= 0 && sel <= 6) ? ids[sel] : 0x1507u;
    d2_multi_unicode_strcpy(dst, (const void*)(uintptr_t)n37_Ordinal_10003(id));
}
// batch_16 (B300) calls with no arguments: the destination is a scratch buffer and the selector is 0.
void d2_multi_sub_8d80(void) { static uint16_t buf[64]; d2_multi_sub_8d80_r(0, buf); }

// 1.13c D2Multi+0x8E70
extern "C" void d2_multi_sub_8e70(uint8_t* s) {
    int32_t v = 5;
    uint16_t f = *(uint16_t*)(s + 0x3c7);
    if ((f & 4) && !(f & 8)) {
        if (n37_Ordinal_10227() != 0 || s[0x1ea] < 5) v = 1;
    }
    DAT_6fa09f38 = n37_Ordinal_10154((int)s[0x1ea], v, (uint32_t)(uintptr_t)(s + 0x39d), (uint32_t)(uintptr_t)(s + 0x3ad));
    if (DAT_6fa09f38) {
        n37_Ordinal_10140(DAT_6fa09f38, 0x42);
        n37_Ordinal_10179(DAT_6fa09f38, 0x23b);
        n37_Ordinal_10080(DAT_6fa09f38, 0);
        n37_Ordinal_10172(DAT_6fa09f38, 0);
    }
}

// 1.13c D2Multi+0x9000
extern "C" bool d2_multi_sub_9000(const char* key, const char* val, uint8_t* out, uint32_t* size, uint32_t cap) {
    *size = 0;
    uint32_t sz = cap;
    if (!d2_host_reg_query(key, val, out, &sz)) return false;
    *size = sz;
    return true;
}

// 1.13c D2Multi+0x90D0: releases each handle that is set, in decompile order: D2Win #10126 (thunk 0x7D40) with ECX = the handle,
// then the slot is cleared. Slot pairs as decompiled (54 blocks).
extern "C" void d2_multi_sub_90d0(void) {
    if (DAT_6fa09f48 != 0) { n37_Ordinal_10126(DAT_6fa09f48); DAT_6fa09f48 = 0; }
    if (DAT_6fa09f4c != 0) { n37_Ordinal_10126(DAT_6fa09f4c); DAT_6fa09f4c = 0; }
    if (DAT_6fa09f40 != 0) { n37_Ordinal_10126(DAT_6fa09f40); DAT_6fa09f40 = 0; }
    if (DAT_6fa09f44 != 0) { n37_Ordinal_10126(DAT_6fa09f44); DAT_6fa09f44 = 0; }
    if (DAT_6fa09f50 != 0) { n37_Ordinal_10126(DAT_6fa09f50); DAT_6fa09f50 = 0; }
    if (DAT_6fa09f54 != 0) { n37_Ordinal_10126(DAT_6fa09f54); DAT_6fa09f54 = 0; }
    if (DAT_6fa09f58 != 0) { n37_Ordinal_10126(DAT_6fa09f58); DAT_6fa09f58 = 0; }
    if (DAT_6fa09f5c != 0) { n37_Ordinal_10126(DAT_6fa09f5c); DAT_6fa09f5c = 0; }
    if (DAT_6fa09f64 != 0) { n37_Ordinal_10126(DAT_6fa09f64); DAT_6fa09f64 = 0; }
    if (DAT_6fa09f68 != 0) { n37_Ordinal_10126(DAT_6fa09f68); DAT_6fa09f68 = 0; }
    if (DAT_6fa09f6c != 0) { n37_Ordinal_10126(DAT_6fa09f6c); DAT_6fa09f6c = 0; }
    if (DAT_6fa09f70 != 0) { n37_Ordinal_10126(DAT_6fa09f70); DAT_6fa09f70 = 0; }
    if (DAT_6fa09f74 != 0) { n37_Ordinal_10126(DAT_6fa09f74); DAT_6fa09f74 = 0; }
    if (DAT_6fa09f78 != 0) { n37_Ordinal_10126(DAT_6fa09f78); DAT_6fa09f78 = 0; }
    if (DAT_6fa09f8c != 0) { n37_Ordinal_10126(DAT_6fa09f8c); DAT_6fa09f8c = 0; }
    if (DAT_6fa09f98 != 0) { n37_Ordinal_10126(DAT_6fa09f98); DAT_6fa09f98 = 0; }
    if (DAT_6fa09f9c != 0) { n37_Ordinal_10126(DAT_6fa09f9c); DAT_6fa09f9c = 0; }
    if (DAT_6fa09ffc != 0) { n37_Ordinal_10126(DAT_6fa09ffc); DAT_6fa09ffc = 0; }
    if (DAT_6fa0a000 != 0) { n37_Ordinal_10126(DAT_6fa0a000); DAT_6fa0a000 = 0; }
    if (DAT_6fa0a004 != 0) { n37_Ordinal_10126(DAT_6fa0a004); DAT_6fa0a004 = 0; }
    if (DAT_6fa0a008 != 0) { n37_Ordinal_10126(DAT_6fa0a008); DAT_6fa0a008 = 0; }
    if (DAT_6fa0a00c != 0) { n37_Ordinal_10126(DAT_6fa0a00c); DAT_6fa0a00c = 0; }
    if (DAT_6fa09fa0 != 0) { n37_Ordinal_10126(DAT_6fa09fa0); DAT_6fa09fa0 = 0; }
    if (DAT_6fa0a010 != 0) { n37_Ordinal_10126(DAT_6fa0a010); DAT_6fa0a010 = 0; }
    if (DAT_6fa0a014 != 0) { n37_Ordinal_10126(DAT_6fa0a014); DAT_6fa0a014 = 0; }
    if (DAT_6fa0a048 != 0) { n37_Ordinal_10126(DAT_6fa0a048); DAT_6fa0a048 = 0; }
    if (DAT_6fa0a04c != 0) { n37_Ordinal_10126(DAT_6fa0a04c); DAT_6fa0a04c = 0; }
    if (DAT_6fa09f60 != 0) { n37_Ordinal_10126(DAT_6fa09f60); DAT_6fa09f60 = 0; }
    if (DAT_6fa0a054 != 0) { n37_Ordinal_10126(DAT_6fa0a054); DAT_6fa0a054 = 0; }
    if (DAT_6fa0a068 != 0) { n37_Ordinal_10126(DAT_6fa0a068); DAT_6fa0a068 = 0; }
    if (DAT_6fa0a06c != 0) { n37_Ordinal_10126(DAT_6fa0a06c); DAT_6fa0a06c = 0; }
    if (DAT_6fa0a084 != 0) { n37_Ordinal_10126(DAT_6fa0a084); DAT_6fa0a084 = 0; }
    if (DAT_6fa0a088 != 0) { n37_Ordinal_10126(DAT_6fa0a088); DAT_6fa0a088 = 0; }
    if (DAT_6fa09f90 != 0) { n37_Ordinal_10126(DAT_6fa09f90); DAT_6fa09f90 = 0; }
    if (DAT_6fa09f94 != 0) { n37_Ordinal_10126(DAT_6fa09f94); DAT_6fa09f94 = 0; }
    if (DAT_6fa0a070 != 0) { n37_Ordinal_10126(DAT_6fa0a070); DAT_6fa0a070 = 0; }
    if (DAT_6fa0a074 != 0) { n37_Ordinal_10126(DAT_6fa0a074); DAT_6fa0a074 = 0; }
    if (DAT_6fa0a078 != 0) { n37_Ordinal_10126(DAT_6fa0a078); DAT_6fa0a078 = 0; }
    if (DAT_6fa0a080 != 0) { n37_Ordinal_10126(DAT_6fa0a080); DAT_6fa0a080 = 0; }
    if (DAT_6fa0a07c != 0) { n37_Ordinal_10126(DAT_6fa0a07c); DAT_6fa0a07c = 0; }
    if (DAT_6fa09fa4 != 0) { n37_Ordinal_10126(DAT_6fa09fa4); DAT_6fa09fa4 = 0; }
    if (DAT_6fa09fa8 != 0) { n37_Ordinal_10126(DAT_6fa09fa8); DAT_6fa09fa4 = 0; }
    if (DAT_6fa09fac != 0) { n37_Ordinal_10126(DAT_6fa09fac); DAT_6fa09fac = 0; }
    if (DAT_6fa09fb0 != 0) { n37_Ordinal_10126(DAT_6fa09fb0); DAT_6fa09fb0 = 0; }
    if (DAT_6fa09fb4 != 0) { n37_Ordinal_10126(DAT_6fa09fb4); DAT_6fa09fb4 = 0; }
    if (DAT_6fa09fb8 != 0) { n37_Ordinal_10126(DAT_6fa09fb8); DAT_6fa09fb8 = 0; }
    if (DAT_6fa09fbc != 0) { n37_Ordinal_10126(DAT_6fa09fbc); DAT_6fa09fbc = 0; }
    if (DAT_6fa09fc0 != 0) { n37_Ordinal_10126(DAT_6fa09fc0); DAT_6fa09fc0 = 0; }
    if (DAT_6fa09fc4 != 0) { n37_Ordinal_10126(DAT_6fa09fc4); DAT_6fa09fc4 = 0; }
    if (DAT_6fa09fc8 != 0) { n37_Ordinal_10126(DAT_6fa09fc8); DAT_6fa09fc8 = 0; }
    if (DAT_6fa09fcc != 0) { n37_Ordinal_10126(DAT_6fa09fcc); DAT_6fa09fcc = 0; }
    if (DAT_6fa09fd0 != 0) { n37_Ordinal_10126(DAT_6fa09fd0); DAT_6fa09fd0 = 0; }
    if (DAT_6fa09ff4 != 0) { n37_Ordinal_10126(DAT_6fa09ff4); DAT_6fa09ff4 = 0; }
    if (DAT_6fa09ff8 != 0) { n37_Ordinal_10126(DAT_6fa09ff8); DAT_6fa09ff8 = 0; }
}

// 1.13c D2Multi+0x9550: each UI path DATA\GLOBAL\... is formatted into a stack buffer and D2Win #10111 (thunk 0x7DAC, ECX = the path)
// returns the handle stored in its slot (53 slots, decompile order); then D2Win #10101 (thunk 0x7D6A) with the last two slots.
extern "C" void d2_multi_sub_9550(void) {
    { char p[] = "DATA\\GLOBAL\\UI\\FrontEnd\\PopUpLargest2"; DAT_6fa09f48 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\FrontEnd\\MediumButtonBlank2"; DAT_6fa09f4c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\bnet"; DAT_6fa09f40 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\waitingroombkgd"; DAT_6fa09f44 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\CURSOR\\ohand"; DAT_6fa09f50 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\CURSOR\\lrotate"; DAT_6fa09f54 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\scrollbar"; DAT_6fa09f58 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\FRONTEND\\joingamescrollbars"; DAT_6fa09f5c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\chatleftbuttons"; DAT_6fa09f64 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\chatleftbuttons1"; DAT_6fa09f68 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\chatleftbuttons2"; DAT_6fa09f6c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\chatrightbuttons"; DAT_6fa09f70 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\chatrighttopbuttons"; DAT_6fa09f74 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\chatarrows"; DAT_6fa09f78 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\joingamebckg"; DAT_6fa09f8c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\GameButtonBlank"; DAT_6fa09f98 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\joingameclickbox"; DAT_6fa09f9c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\numberarrows"; DAT_6fa09ffc = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\numberarrows"; DAT_6fa0a000 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\numberarrows"; DAT_6fa0a004 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\numberarrows"; DAT_6fa0a008 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\numbergreyarrows"; DAT_6fa0a00c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\GreyLevelBox"; DAT_6fa09fa0 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\createjoinbckg"; DAT_6fa0a010 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\okaybutton"; DAT_6fa0a014 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\creategamebckg"; DAT_6fa0a048 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\creategamebutton"; DAT_6fa0a04c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\widebuttonblank"; DAT_6fa0a054 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\CancelButtonBlank"; DAT_6fa0a068 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\yesnobuttonblank"; DAT_6fa0a06c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\D2logoBlackBnet"; DAT_6fa0a084 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\D2logoBnet"; DAT_6fa0a088 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\blankbckg2"; DAT_6fa09f90 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\gembutton"; DAT_6fa09f94 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\LadderChoiceEXP"; DAT_6fa0a070 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\HardcoreLadderBckg"; DAT_6fa0a074 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\PlayerProfileBckg"; DAT_6fa0a07c = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\PlayerProfileBckg"; DAT_6fa0a078 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\HelpBckg"; DAT_6fa0a080 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\radiobutton"; DAT_6fa09fa4 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UI\\BIGMENU\\radiobuttonGrey"; DAT_6fa09fa8 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\PopUpOK"; DAT_6fa09fac = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\CancelButtonBlank"; DAT_6fa09fb0 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\overalltab"; DAT_6fa09fb4 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\amazontab"; DAT_6fa09fb8 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\sorceresstab"; DAT_6fa09fbc = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\necromancertab"; DAT_6fa09fc0 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\paladintab"; DAT_6fa09fc4 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\barbariantab"; DAT_6fa09fc8 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\druidtab"; DAT_6fa09fcc = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\ui\\BIGMENU\\assassintab"; DAT_6fa09fd0 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UncompOverlays\\SquelchAura"; DAT_6fa09ff4 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    { char p[] = "DATA\\GLOBAL\\UncompOverlays\\SelectAura"; DAT_6fa09ff8 = (int32_t)n37_Ordinal_10111((intptr_t)p); }
    n37_Ordinal_10101(DAT_6fa09ff4, DAT_6fa09ff8);
}

// 1.13c D2Multi+0x9D60
// The Unicode default-constructor loop (0x200 two-byte units) zeroes the local wide buffer; D2Win #10131 (IAT 0x161D4: ECX = DAT_6fa0a044,
// EDX = 1 when the joined text is empty) and #10000 (IAT 0x16174: ECX = DAT_6fa09f88, EDX = the wide text, stack 1,0,0) take register inputs.
__attribute__((weak)) int32_t DAT_6fa0a044;
extern "C" void d2_multi_sub_9d60(void) {
    uint16_t wide[512]; memset(wide, 0, sizeof wide);
    if (DAT_6fa09f88) {
        char buf[512]; buf[0] = 0;
        int32_t n = 0;
        for (char* s = DAT_6fa0a0f0; s && n < 0x200; s = (char*)(uintptr_t)*(uint32_t*)(s + 0x204)) {   // retail's next is a 32-bit word at +0x204
            strncat(buf, s, 0x200 - n);
            n = (int32_t)strlen(buf);
        }
        n37_Ordinal_10131(DAT_6fa0a044, buf[0] == 0 ? 1 : 0);
        d2_multi_utf8ToUnicode(wide, buf, 0x200);
        n37_Ordinal_10000(DAT_6fa09f88, wide, 1, 0, 0);
    }
}

static void fatal13e8(void) {
    n37_Ordinal_10024(DAT_6f9e75d7, n37_Ordinal_10265(0x13e8));
    d2_host_exit(-1);
}
// One control of the 32-slot table: Ordinal_10098(descriptor VA), table[count++] = handle, 10196(h, 0) on the first, 10001(h, 1). Returns the handle.
static int32_t push_ctl(int32_t name, bool zero) {
    int32_t h = n37_Ordinal_10098(name);
    DAT_6fa08e80[DAT_6fa09f24++] = h;
    if (zero) n37_Ordinal_10196(h, 0);
    n37_Ordinal_10001(h, 1);
    return h;
}

// 1.13c D2Multi+0xAA60: EBX = callback (0: LAB_6f9daa50), stack word 0 = the wide text handed to D2Win #10000 in EDX (ECX = the last control handle); ret 4.
extern "C" void d2_multi_sub_aa60(int32_t ebx, const void* text) {
    n37_cb_8790();
    if (DAT_6fa09f24 > 0x1f) { fatal13e8(); return; }
    push_ctl(0x6F9EA360, true);
    if (DAT_6fa09f24 >= 0x20) { fatal13e8(); return; }
    push_ctl(0x6F9EA3C0, false);
    if (DAT_6fa09f24 >= 0x20) { fatal13e8(); return; }
    int32_t h = push_ctl(0x6F9EA3F0, false);
    n37_Ordinal_10000(h, text, 0, 0, 0);
    DAT_6fa0a0e0 = ebx ? ebx : 0x6f9daa50;
    DAT_6fa09f2c = (int32_t)d2_host_tick();
}

// 1.13c D2Multi+0xACB0: EBX = callback (0: LAB_6f9daa50), stack word 0 = the wide text handed to D2Win #10000 in EDX (ECX = the last control handle); ret 4.
extern "C" void d2_multi_sub_acb0(int32_t ebx, const void* text) {
    n37_cb_8790();
    if (DAT_6fa09f24 > 0x1f) { fatal13e8(); return; }
    push_ctl(0x6F9EA360, true);
    if (DAT_6fa09f24 >= 0x20) { fatal13e8(); return; }
    push_ctl(0x6F9EA390, false);
    if (DAT_6fa09f24 >= 0x20) { fatal13e8(); return; }
    int32_t h = push_ctl(0x6F9EA3F0, false);
    n37_Ordinal_10000(h, text, 0, 0, 0);
    DAT_6fa0a0e0 = ebx ? ebx : 0x6f9daa50;
}

// 1.13c D2Multi+0xB160: EBX = out buffer, stack words (FILETIME*, n); ret 8. FileTimeToSystemTime + GetDateFormatA(0x400, 2, st, 0, out, n) is the host's
// d2_host_date_format; MultiByteToWideChar(ACP, 1, out, -1, wide, 0x100) widens the date (ASCII here); Unicode::toUtf(out, wide, n) is D2Lang's.
extern "C" void d2_multi_sub_b160(const void* filetime, char* out, int32_t n) {
    uint16_t wide[256]; memset(wide, 0, sizeof wide);
    d2_host_date_format(filetime, out, n);
    for (int i = 0; i < 255 && out[i]; i++) wide[i] = (uint8_t)out[i];
    d2_multi_toUtf(out, wide, n);
}

// 1.13c D2Multi+0xB970
void d2_multi_sub_b970(void) {
    DAT_6fa0a018 = n37_Ordinal_10098(DAT_6f9ea7b0);
    DAT_6fa0a01c = n37_Ordinal_10098(DAT_6f9ea7e0);
    n37_Ordinal_10003(); n37_Ordinal_10020(0);
    DAT_6fa0a020 = n37_Ordinal_10098(DAT_6f9ea810);
    n37_Ordinal_10003(); n37_Ordinal_10020(0);
    DAT_6fa0a024 = n37_Ordinal_10098(DAT_6f9ea840);
    n37_Ordinal_10003(); n37_Ordinal_10020(0);
    DAT_6fa0a028 = n37_Ordinal_10098(DAT_6f9ea900);
    DAT_6fa0a02c = n37_Ordinal_10098(DAT_6f9eaff0);
    n37_Ordinal_10031(DAT_6fa0a02c);
    DAT_6fa0a030 = n37_Ordinal_10098(DAT_6f9ea870);
    n37_Ordinal_10202(); n37_Ordinal_10170(); n37_Ordinal_10010();
    DAT_6fa0a038 = n37_Ordinal_10098(DAT_6f9ea8d0);
    const char* list = (const char*)(uintptr_t)(*(int32_t (**)(void))(uintptr_t)(DAT_6fa0917c + 0x20))();
    const char* me = (const char*)DAT_6fa09f14 + 0x35d;
    if (DAT_6fa09f14[0x37d]) n37_Ordinal_10167();
    if (*me && strcmp((const char*)DAT_6fa09f14 + 0x37d, me)) n37_Ordinal_10167();
    while (*list) {
        if (!*me || strcmp(list, me)) n37_Ordinal_10167();
        list += strlen(list) + 1;
    }
    n37_Ordinal_10185();
    n37_Ordinal_10053(DAT_6fa0a030);
    n37_Ordinal_10021();
    n37_Ordinal_10037();
    void* u = (void*)(uintptr_t)n37_Ordinal_10166();
    if (u) { char nm[0x20]; d2_multi_unicode2Win(nm, u, 0x1f); }
    n37_Ordinal_10147();
}

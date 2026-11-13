// superseded port of D2Multi 0x11A20 / +0x11DA0: C-linkage names the batch-17 teardown (BE00) calls; forward to the native ports in batch n49.
void d2multi_11A20(void);
void d2multi_11DA0(void);
extern "C" void d2_multi_sub_1a20(void) { d2multi_11A20(); }
extern "C" void d2_multi_sub_1da0(void) { d2multi_11DA0(); }

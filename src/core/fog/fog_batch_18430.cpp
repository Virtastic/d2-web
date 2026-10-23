// Fog logging / stack-dump helpers. Host form: no log files, no imagehlp walker, so each is a no-op.
#include <cstdarg>
#include <cstdint>

// 1.13c Fog+0x18430: build the log file name from the program name and append the argument; no file on the host.
void fog_log_name_18430(uint32_t) {}

// 1.13c Fog+0x18770: format a message and queue it on the error-reporter list; the reporter is absent, so drop it.
void fog_log_va_18770(const char*, int, int, const char*, va_list) {}

// 1.13c Fog+0x15C00: imagehlp stack walk of a thread; there is no imagehlp, retail's own early-out path.
void fog_dump_stack_15C00(void*) {}

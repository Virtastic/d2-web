// src/core/fog/fog_batch_102C0.cpp -- twelve 1.13c Fog.dll alpha-lane rows reimplemented from the
// cached Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c). Base 0x6FF50000.
// Rows: the OS/memory probe (+0x102C0), the log-header writer (+0x10660), the log-close thunk
// (+0x10810), the assert reporter (+0x109A0), the stack/code dump (+0x10B80), the flag-gated trace
// writer (+0x10BF0), the thread dump (+0x10E30), the startup banner (+0x113B0), the critical-section
// validator (+0x11690), and the lookup tree/table trio (+0x11DF0, +0x11E40, +0x11F00).
//
// The module callees that are separate, still UNMAPPED Fog rows (log writers +0x190A0/+0x19100/
// +0x18AF0/+0x18B50, log close +0x184A0, stack trace +0x155E0, capture-ip +0xFC20, path strip
// +0xFC70, memory/app-path/local-ip queries +0x1C830/+0x143D0/+0x16A10, error report +0x10F30)
// are host seams here: their argument shape and log side effect are kept, nothing more.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <Windows.h>   // SYSTEMTIME, GetLocalTime, OutputDebugStringA (host shims)
#include <emscripten.h>

namespace {

// Fog+0x6FF7xxxx module state these rows read and write.
uint32_t g_fogOsProbed;                          // +0x32EFC, first dword of the info export
uint32_t g_fogPlatformId;                        // +0x32F00
float    g_fogPlatformVersion;                   // +0x32F04
uint32_t g_fogTotalPhys;                         // +0x32F08
uint8_t  g_fogSystemInfo[0x40];                  // +0x32F10, GetSystemInfo output
char     g_fogProgramName[0x104] = "Diablo II";  // +0x2FCD8
char     g_fogVersionText[0x104];                // +0x32B08
uint32_t g_fogThreadDumpCount;                   // +0x32AC8
uint32_t g_fogCriticalSectionChecks;             // +0x5A6E74, validation-enabled flag

// Fog+0x6FF78350 = "???": the +0x11DF0 tree returns this pointer, not NULL, when the key is absent.
char g_fogNotFound[] = "???";
const char* g_fogPendingLine = "";   // the line the +0x10660 callback prints

// Fog+0x6FF7FC74, the error-message pointer table (index 0..10 used by the assert paths).
const char* const kFogErrorText[] = {
    "No Error", "Unknown Error", "Out of Memory Error", "File Read Error",
    "File Write Error", "Assertion Failure", "Halt", "Warning", "Missing Dll",
    "Registry Error", "Deadlock",
};

const char* fog_error_text(int32_t code) {
    return code >= 0 && code < static_cast<int32_t>(sizeof(kFogErrorText) / sizeof(kFogErrorText[0]))
               ? kFogErrorText[code]
               : "Unknown Error";
}

// The platform names the +0x113B0 banner selects by Fog+0x32F00.
const char* fog_platform_name(uint32_t platform) {
    switch (platform) {
    case 1: return "Windows 95";
    case 2: return "Windows 98";
    case 3: return "Windows ME";
    case 4: return "Windows NT";
    case 5: return "Windows 2000";
    case 6: return "Windows XP";
    case 7: return "Windows .Net";
    case 8: return "MacOS";
    case 9: return "MacOS X";
    default: return "unknown";
    }
}

// Fog+0x190A0 / +0x19100: append one formatted record to the open log.
void fog_log_printf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    std::vfprintf(stderr, format, ap);
    va_end(ap);
    std::fputc('\n', stderr);
}

// Fog+0xFD80, the default +0x10660 sink: print the pending line.
void fog_log_sink_default(void) {
    std::fputs(g_fogPendingLine, stderr);
    std::fputc('\n', stderr);
}

// Fog+0x18950: bounded byte dump.
void fog_hex_dump(const void* data, uint32_t length) {
    if (data == nullptr) return;
    const uint8_t* p = static_cast<const uint8_t*>(data);
    for (uint32_t i = 0; i < length; ++i) {
        if (i % 16 == 0) std::fprintf(stderr, i == 0 ? "%08X" : "\n%08X", i);
        std::fprintf(stderr, " %02X", p[i]);
    }
    std::fputc('\n', stderr);
}

// Fog+0x184A0 / +0x18430: close the log and flush the reporter; a no-op on the host.
void fog_close_logs(void) {}
void fog_log_thunk_18430(uint32_t param) { (void)param; }

// Fog+0xFC20 / +0xCD8E / +0x155E0: capture the instruction pointer and dump the current stack.
uint32_t fog_capture_ip(void) { return 0; }
void fog_stack_dump(void) {}
void fog_stack_trace_155E0(void) { fog_stack_dump(); }

// Fog+0x10F30: the reporter writes the formatted record (Win32 dialog/file effects omitted).
void fog_error_report(int32_t type, const char* file, uint32_t line, const char* format, ...) {
    std::fprintf(stderr, "Fog error %d at %s:%u: ", static_cast<int>(type), file ? file : "", line);
    va_list ap;
    va_start(ap, format);
    std::vfprintf(stderr, format, ap);
    va_end(ap);
    std::fputc('\n', stderr);
}

// Fog+0xFC70: the %s the assert path prints for a source path. Retail does not search for a separator:
// the offset is strlen("..\\Source\\Fog\\Src\\ErrorManager.cpp") - strlen("Fog\\Src\\ErrorManager.cpp") = 34 - 24 = 10
// (computed once into DAT_6FFA6ED0), and the result is path + 10 when strlen(path) > 10, else path.
const char* fog_basename(const char* path) {
    const size_t offset = 10;
    return std::strlen(path) > offset ? path + offset : path;
}

// Fog+0x1BA9 (Storm _exit) and the reporter's MessageBoxA tail.
[[noreturn]] void fog_exit_process(int32_t code) { std::_Exit(code); }
void fog_message_box(const char* text, const char* caption, uint32_t type) {
    (void)text; (void)caption; (void)type;
}

// Ordinal_116 (Fog export): a side-effect call in the fatal path; kept as a seam.
void fog_ordinal_116(void) {}

// Host seams for the Win32 platform queries. The engine is wasm64 and deterministic: the version
// probe fails, the memory report and system-info block are zero, and the user/computer names come
// back empty (retail stores the empty string when the query fails).
struct FogOsVersionInfo {
    uint32_t size;      // +0x00
    uint32_t major;     // +0x04
    uint32_t minor;     // +0x08
    uint32_t build;     // +0x0C
    uint32_t platform;  // +0x10
};
int32_t fog_host_get_version_ex(FogOsVersionInfo* osvi) { std::memset(osvi, 0, sizeof(*osvi)); return 0; }
void fog_host_global_memory_status(uint32_t* totalPhys) { *totalPhys = 0; }
void fog_host_get_system_info(void* block) { std::memset(block, 0, 0x24); }
int32_t fog_host_user_name(char* buffer, uint32_t* count) { (void)buffer; *count = 0; return 0; }
int32_t fog_host_computer_name(char* buffer, uint32_t* count) { (void)buffer; *count = 0; return 0; }
uint32_t fog_host_current_thread_id(void) { return 1; }
uint32_t fog_host_last_error(void) { return 0; }
int32_t fog_host_duplicate_current_thread(HANDLE* out) { *out = nullptr; return 0; }
void fog_host_close_handle(HANDLE handle) { (void)handle; }
int16_t fog_host_get_key_state(int32_t key) { (void)key; return 0; }

// Ordinal_501 SStrCopy: bounded copy, always terminated.
void fog_str_copy(char* dest, const char* src, uint32_t max) {
    if (max == 0) return;
    uint32_t i = 0;
    while (i + 1 < max && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// The handle the +0x11DF0 / +0x11E40 / +0x11F00 lookups consume: count, an unused word, then the
// 8-byte entries (4-character code, then a 32-bit key).
struct FogLookupTable {
    int32_t count;    // +0x00
    int32_t at04;     // +0x04
    uint8_t* entries; // +0x08
};

uint32_t fog_lookup_key(const FogLookupTable* table, int32_t index) {
    return *reinterpret_cast<const uint32_t*>(table->entries + index * 8);
}
int32_t fog_lookup_value(const FogLookupTable* table, int32_t index) {
    return *reinterpret_cast<const int32_t*>(table->entries + index * 8 + 4);
}

}  // namespace

// 1.13c Fog+0x102C0
// The platform/memory probe behind GetSystemInfo (0x105D0): query the OS version, fold the minor
// version into a float "major.minor" (halving the scale until it drops below 1), classify the
// platform into Fog+0x32F00, then fill the physical-memory dword and the SYSTEM_INFO block and set
// the initialised flag. The host probe reports failure, so the platform name stays "unknown".
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_102C0(void) {  // 1.13c Fog+0x102C0
    FogOsVersionInfo osvi;
    osvi.size = 0x94;
    if (fog_host_get_version_ex(&osvi) != 0) {
        float minor = static_cast<float>(static_cast<int32_t>(osvi.minor));
        if (static_cast<int32_t>(osvi.minor) < 0) minor += 4294967296.0f;
        while (1.0f < minor) minor *= 0.1f;
        g_fogPlatformVersion = static_cast<float>(static_cast<int32_t>(osvi.major));
        if (static_cast<int32_t>(osvi.major) < 0) g_fogPlatformVersion += 4294967296.0f;
        g_fogPlatformVersion += minor * 0.01f;
        if (osvi.platform == 1) {
            g_fogPlatformId = 1;
            if (osvi.minor < 0x5a) {
                if (osvi.minor > 9) g_fogPlatformId = 2;
            } else {
                g_fogPlatformId = 3;
            }
        } else if (osvi.platform == 2) {
            g_fogPlatformId = 4;
            if (osvi.major > 4) {
                if (osvi.minor < 2) g_fogPlatformId = 6u - (osvi.minor == 0 ? 1u : 0u);
                else g_fogPlatformId = 7;
            }
        } else {
            g_fogPlatformId = 0;
        }
    }
    fog_host_global_memory_status(&g_fogTotalPhys);
    fog_host_get_system_info(g_fogSystemInfo);
    g_fogOsProbed = 1;
}

// 1.13c Fog+0x10660
// Build the five-line log header: program name (with the version suffix when one was set),
// computer name, user name, local time, then an empty line. Each line is formatted into the same
// 0x200-byte buffer and handed to the caller's no-argument sink (the +0xFD80 default when NULL).
// 1.13c Fog+0x10660
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_10660(uint32_t param_1,
                                                   void (*sink)(void)) {
    (void)param_1;
    if (sink == nullptr) sink = fog_log_sink_default;
    SYSTEMTIME time;
    GetLocalTime(&time);
    char computer[64];
    char user[64];
    uint32_t count = 0x40;
    if (fog_host_computer_name(computer, &count) == 0) computer[0] = '\0';
    count = 0x40;
    if (fog_host_user_name(user, &count) == 0) user[0] = '\0';
    char program[0x104];
    if (g_fogVersionText[0] == '\0') {
        fog_str_copy(program, g_fogProgramName, 0x104);
    } else {
        std::snprintf(program, 0x104, "%s %s", g_fogProgramName, g_fogVersionText);
    }
    char line[0x200];
    std::snprintf(line, 0x200, "PROGRAM:       %s", program);
    g_fogPendingLine = line;
    sink();
    std::snprintf(line, 0x200, "COMPUTER NAME: %s", computer);
    g_fogPendingLine = line;
    sink();
    std::snprintf(line, 0x200, "USER NAME:     %s", user);
    g_fogPendingLine = line;
    sink();
    std::snprintf(line, 0x200, "TIME:          %04u-%02u-%02u %02u:%02u:%02u.%03u",
                  static_cast<unsigned>(time.wYear), static_cast<unsigned>(time.wMonth),
                  static_cast<unsigned>(time.wDay), static_cast<unsigned>(time.wHour),
                  static_cast<unsigned>(time.wMinute), static_cast<unsigned>(time.wSecond),
                  static_cast<unsigned>(time.wMilliseconds));
    g_fogPendingLine = line;
    sink();
    line[0] = '\0';  // +0x6FF768EC is an empty format at runtime
    g_fogPendingLine = line;
    sink();
}

// 1.13c Fog+0x10810
// A one-instruction thunk: forward the argument to Fog+0x18430 (a separate row).
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_10810(uint32_t param_1) {  // 1.13c Fog+0x10810
    fog_log_thunk_18430(param_1);
}

// 1.13c Fog+0x109A0
// The assert reporter: two blank log lines, a rule, the +0x10660 header, the FILE/LINE line when a
// file was given, then the formatted expression. A NULL format is the unrecoverable path (capture
// IP, report level 6 at line 0x7E2, close and exit). A NULL file prints the raw expression; else the
// display text is "message\nLocation : file, line #N\nExpression : expr". Finally the text is
// logged, the logs closed, and MessageBoxA shown (seam).
// 1.13c Fog+0x109A0
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_109A0(uint32_t line, const char* file,
                                                   int32_t errorCode, const char* format,
                                                   va_list args) {
    fog_log_printf("");
    fog_log_printf("");
    fog_log_printf("------------------------------------------------------");
    fog_sub_10660(0, fog_log_sink_default);
    if (file != nullptr) {
        char expression[0x1000];
        std::snprintf(expression, sizeof(expression),
                      "--------  FILE: %s    LINE: %d  --------\n\n", fog_basename(file),
                      static_cast<int>(line));
        fog_log_printf("%s", expression);
    }
    if (format == nullptr) {
        const uint32_t ip = fog_capture_ip();
        fog_ordinal_116();
        fog_error_report(6, "", 0x7e2, "Unrecoverable internal error %08x", ip);
        fog_close_logs();
        fog_exit_process(-1);
    }
    char message[0x1000];
    std::vsnprintf(message, sizeof(message), format, args);
    char display[0x1000];
    if (file == nullptr) {
        std::snprintf(display, sizeof(display), "%s", message);
    } else {
        std::snprintf(display, sizeof(display),
                      "%s\nLocation : %s, line #%d\nExpression : %s", fog_error_text(errorCode),
                      fog_basename(file), static_cast<int>(line), message);
    }
    char title[0x100];
    std::snprintf(title, sizeof(title), "%s Error", g_fogProgramName);
    const size_t used = std::strlen(display);
    if (used + 1 < sizeof(display)) {
        display[used] = '\n';  // +0x6FF76F64 is the two-byte "\n\0"
        display[used + 1] = '\0';
    }
    fog_log_printf("%s", display);
    fog_close_logs();
    fog_message_box(display, title, 0x30);
}

// 1.13c Fog+0x10B80
// Dump the captured stack bytes (0x1000) and code bytes (0x20) to the log, then close it.
// 1.13c Fog+0x10B80
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_10B80(void* stackBytes,
                                                   void* codeBytes) {
    fog_log_printf("Stack bytes:");
    fog_hex_dump(stackBytes, 0x1000);
    fog_log_printf("Code bytes:");
    fog_hex_dump(codeBytes, 0x20);
    fog_close_logs();
}

// 1.13c Fog+0x10BF0
// The flag-gated trace writer: bit 4 prefixes "hh:mm:ss.mmm    ", bit 2 suppresses the debugger
// output, bit 1 writes the line to the log, and bits 0x18 poll the key state (side effect only).
// 1.13c Fog+0x10BF0
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_10BF0(uint8_t flags, const char* format,
                                                   va_list args) {
    char buffer[0x1000];
    buffer[0] = '\0';
    if ((flags & 4) != 0) {
        SYSTEMTIME time;
        GetLocalTime(&time);
        std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d    ",
                      static_cast<int>(time.wHour), static_cast<int>(time.wMinute),
                      static_cast<int>(time.wSecond), static_cast<int>(time.wMilliseconds));
    }
    const size_t used = std::strlen(buffer);
    std::vsnprintf(buffer + used, sizeof(buffer) - used, format, args);
    if ((flags & 2) == 0) OutputDebugStringA(buffer);
    if ((flags & 1) != 0) fog_log_printf("%s", buffer);
    if ((flags & 0x18) != 0) (void)fog_host_get_key_state(0x14);
}

// 1.13c Fog+0x10E30
// The per-thread dump: bump the counter, log the rule and the thread id, duplicate the current
// thread handle (logging "  err = %d" on failure and returning), then run the stack trace and close
// the duplicate. Both retail branches run the same tail.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_10E30(void) {  // 1.13c Fog+0x10E30
    ++g_fogThreadDumpCount;
    fog_log_printf("--------------------------------------");
    const uint32_t threadId = fog_host_current_thread_id();
    fog_log_printf("Thread %d ***", static_cast<int>(threadId));
    fog_log_printf("----------");
    HANDLE duplicate = nullptr;
    if (fog_host_duplicate_current_thread(&duplicate) == 0) {
        const uint32_t error = fog_host_last_error();
        fog_log_printf("  err = %d", static_cast<int>(error));
        return;
    }
    fog_stack_trace_155E0();
    fog_host_close_handle(duplicate);
}

// 1.13c Fog+0x113B0
// The startup banner: time stamp, "<program> running under <platform> (Version x.yyy)", total
// physical memory, the application path when the query returns one, the local IP, the computer
// name, then the closing rules and a blank line. The query rows are seams.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_113B0(void) {  // 1.13c Fog+0x113B0
    fog_log_printf("------------------------------------------------------");
    SYSTEMTIME time;
    GetLocalTime(&time);
    fog_log_printf("%04d-%02d-%02d %02d:%02d:%02d", static_cast<int>(time.wYear),
                   static_cast<int>(time.wMonth), static_cast<int>(time.wDay),
                   static_cast<int>(time.wHour), static_cast<int>(time.wMinute),
                   static_cast<int>(time.wSecond));
    fog_log_printf("%s running under %s (Version %.3f)", g_fogProgramName,
                   fog_platform_name(g_fogPlatformId),
                   static_cast<double>(g_fogPlatformVersion));
    fog_log_printf("Total Physical Memory: %.2fMB", 0.0);  // Fog+0x1C830
    const char* applicationPath = nullptr;                 // Fog+0x143D0
    if (applicationPath != nullptr) fog_log_printf("Application Path: %s", applicationPath);
    fog_log_printf("Local IP: %s", "");                   // Fog+0x16A10
    char computer[64];
    uint32_t count = 0x40;
    if (fog_host_computer_name(computer, &count) == 0) computer[0] = '\0';
    fog_log_printf("Computer name: %s", computer);
    fog_log_printf("------------------------------------------------------");
    fog_log_printf("");
}

// 1.13c Fog+0x11690
// Validate a 0x18-byte CRITICAL_SECTION while the module flag +0x5A6E74 is set: the debug pointer
// is non-NULL, dword-aligned and below 0x80000000, the lock/recursion counts are at most 10 (the
// lock count may also be -1), the debug pointer's +4 back-pointer is this section and its +8/+0xC
// fields are non-NULL, aligned and in range. Any failure logs and dumps the section, then reports
// level 6 at line 0x165 and exits.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_11690(uint32_t* section) {  // 1.13c Fog+0x11690
    if (g_fogCriticalSectionChecks == 0) return;
    bool bad = false;
    uint32_t first = *section;
    if (first == 0 || (first & 3) != 0 || first > 0x7fffffff) {
        bad = true;
    } else if ((section[1] > 10 && section[1] != 0xffffffff) || section[2] > 10) {
        bad = true;
    } else {
        const uint32_t backPointer =
            *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(first) + 4);
        const uint32_t second =
            *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(first) + 8);
        if (backPointer != static_cast<uint32_t>(reinterpret_cast<uintptr_t>(section)) || second == 0 ||
            (second & 3) != 0 || second > 0x7fffffff) {
            bad = true;
        } else {
            const uint32_t third =
                *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(first) + 0xc);
            if (third == 0 || (third & 3) != 0 || third > 0x7fffffff) bad = true;
        }
    }
    if (!bad) return;
    fog_log_printf("FogValidateCriticalSection(%d) - bad critical section err=%d",
                   static_cast<int>(fog_host_current_thread_id()),
                   static_cast<int>(fog_host_last_error()));
    fog_hex_dump(section, 0x18);
    if (*section != 0) {
        fog_hex_dump(reinterpret_cast<void*>(static_cast<uintptr_t>(*section)), 0x20);
    }
    const uint32_t ip = fog_capture_ip();
    fog_ordinal_116();
    fog_error_report(6, "", 0x165, "Unrecoverable internal error %08x", ip);
    fog_close_logs();
    fog_exit_process(-1);
}

// 1.13c Fog+0x11DF0
// Recursive tree lookup: a node matches when its +0x20 key equals the argument; otherwise search
// the +0x24 child and then the +0x28 child. A NULL node or a miss returns the Fog+0x6FF78350
// sentinel (the "???" string pointer), not NULL.
// 1.13c Fog+0x11DF0
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_sub_11DF0(void* node,
                                                    int32_t key) {
    if (node != nullptr) {
        uint8_t* base = static_cast<uint8_t*>(node);
        if (*reinterpret_cast<int32_t*>(base + 0x20) == key) return node;
        void* left = *reinterpret_cast<void**>(base + 0x24);
        if (left != nullptr) {
            void* found = fog_sub_11DF0(left, key);
            if (found != g_fogNotFound) return found;
        }
        void* right = *reinterpret_cast<void**>(base + 0x28);
        if (right != nullptr) {
            void* found = fog_sub_11DF0(right, key);
            if (found != g_fogNotFound) return found;
        }
    }
    return g_fogNotFound;
}

// 1.13c Fog+0x11E40
// Linear lookup of an 8-byte entry by its +4 key: copy the 4-character code to the caller's buffer,
// mapping a trailing space to the terminator (and a space in the last slot also clears the fifth
// byte). Returns 1 on a match, 0 for a NULL table, a negative/out-of-range key, or no match.
// 1.13c Fog+0x11E40
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_sub_11E40(FogLookupTable* table, int32_t key,
                                                      char* out) {
    if (table->entries == nullptr) return 0;
    if (key < 0 || key >= table->count) return 0;
    for (int32_t i = 0; i < table->count; ++i) {
        if (fog_lookup_value(table, i) == key) {
            const uint8_t* entry = table->entries + i * 8;
            for (int32_t j = 0; j < 4; ++j) {
                const char c = static_cast<char>(entry[j]);
                out[j] = (c == ' ') ? '\0' : c;
            }
            out[4] = '\0';
            return 1;
        }
    }
    return 0;
}

// 1.13c Fog+0x11F00
// Binary search of the same table by key: returns the entry's +4 value, or -1 when absent.
// The bound arithmetic is retail's (midpoint, then the updated upper bound is compared with mid).
// 1.13c Fog+0x11F00
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_sub_11F00(FogLookupTable* table,
                                                      uint32_t key) {
    int32_t upper = table->count;
    int32_t lower = 0;
    while (lower < upper) {
        int32_t mid = (upper - lower) / 2 + lower;
        const uint32_t entryKey = fog_lookup_key(table, mid);
        if (entryKey < key) {
            lower = mid + 1;
            mid = upper;
        } else if (entryKey == key) {
            return fog_lookup_value(table, mid);
        }
        upper = mid;
    }
    return -1;
}

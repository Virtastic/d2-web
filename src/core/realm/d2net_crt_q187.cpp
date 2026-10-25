// D2Net CRT startup: command line / environment splitting (q187). Pure functions; platform state passed in.
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace d2net_q187 {

// 1.13c D2Net+0x2349 parse_cmdline: MS tokenizer. isLead(b) = DBCS lead-byte test (ctype bit 4).
// Returns argv strings; argv[0] ends at first space/tab outside quotes, quotes dropped.
static std::vector<std::string> parse_cmdline(const char* p, bool (*isLead)(uint8_t)) {
  std::vector<std::string> out;
  std::string a;
  bool q = false;
  for (;; ++p) {
    uint8_t c = (uint8_t)*p;
    if (c == '"') { q = !q; continue; }
    if (c == 0) { out.push_back(a); return out; }
    if (!q && (c == ' ' || c == '\t')) { ++p; break; }
    a += (char)c;
    if (isLead(c) && p[1]) a += *++p;
  }
  out.push_back(a);
  q = false;
  while (*p) {
    while (*p == ' ' || *p == '\t') ++p;
    if (!*p) break;
    a.clear();
    for (;;) {
      unsigned bs = 0; bool copy = true;
      for (; *p == '\\'; ++p) ++bs;
      if (*p == '"') {
        if (!(bs & 1)) {
          if (q && p[1] == '"') ++p; else copy = false;
          q = !q;
        }
        bs >>= 1;
      }
      a.append(bs, '\\');
      uint8_t c = (uint8_t)*p;
      if (!c || (!q && (c == ' ' || c == '\t'))) break;
      if (copy) { a += (char)c; if (isLead(c)) a += *++p; }
      ++p;
    }
    out.push_back(a);
  }
  return out;
}

// 1.13c D2Net+0x24B5 __setargv: program name + command line -> argv (argc = size).
std::vector<std::string> setargv(const std::string& module, const std::string& cmdline, bool (*isLead)(uint8_t)) {
  std::vector<std::string> v = parse_cmdline(cmdline.c_str(), isLead);
  (void)module;  // retail passes the module path as the command-line head via GetModuleFileNameA
  return v;
}

// 1.13c D2Net+0x2282 __setenvp: NUL-separated block (double-NUL end) -> entries not starting with '='.
std::vector<std::string> setenvp(const char* block) {
  std::vector<std::string> out;
  for (; block && *block; block += strlen(block) + 1)
    if (*block != '=') out.emplace_back(block);
  return out;
}

}  // namespace d2net_q187

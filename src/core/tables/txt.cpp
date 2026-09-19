#include "txt.h"
#include <cstdlib>
#include <cstring>

namespace d2 {

int TxtTable::column(const char* name) const {
    for (size_t i = 0; i < columns.size(); i++) if (columns[i] == name) return (int)i;
    return -1;
}

int32_t TxtTable::num(size_t r, int c) const {
    const std::string& s = cell(r, c);
    return s.empty() ? 0 : (int32_t)strtol(s.c_str(), nullptr, 10);
}

TxtTable parseTxt(const uint8_t* data, size_t len) {
    TxtTable t;
    std::vector<std::string> cells;
    std::string cur;
    bool header = true;
    auto endRow = [&]() {
        cells.push_back(cur); cur.clear();
        if (header) { t.columns = cells; header = false; }
        else if (!(cells.size() == 1 && cells[0].empty())) {   // a bare "\r\n" is not a row
            cells.resize(t.columns.size());
            t.rows.push_back(cells);
        }
        cells.clear();
    };
    for (size_t i = 0; i < len; i++) {
        uint8_t ch = data[i];
        if (ch == '\t') { cells.push_back(cur); cur.clear(); }
        else if (ch == '\n') endRow();
        else if (ch != '\r') cur.push_back((char)ch);
    }
    if (!cur.empty() || !cells.empty()) endRow();   // last line without a newline
    return t;
}

} // namespace d2

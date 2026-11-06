// Test-only native ccall host for the realm composers. test/realm-bnet-e2e.test.mjs gives
// play/assets/realm-client.mjs a fake Emscripten Module whose ccall runs this binary: it loads the
// Module's heap image from <memfile>, calls one src/core/realm export on it, writes the heap back and
// prints the return value. Built with the same shim as realmd (tools/realmd/shim empties
// EMSCRIPTEN_KEEPALIVE), so the client composes its bytes with the real sources and no emcc.
//
//   realm-ccall-host <memfile> <export> [n:<int> | p:<heap offset> | s:<hex string>]...
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" {
int d2_realm_bncs_header(uint8_t, int, uint8_t*);
int d2_realm_bncs_checkrev(const char*, const char*, int, char*);
int d2_realm_bncs_auth_info_request(const char*, uint8_t*);
int d2_realm_bncs_auth_check_request(uint32_t, uint32_t, uint32_t, uint32_t, const uint8_t*, int, const char*, const char*, uint8_t*);
int d2_realm_bncs_logonresponse2_request(uint32_t, uint32_t, const char*, const char*, uint8_t*);
int d2_realm_bncs_logonrealmex_request(uint32_t, uint32_t, const char*, const char*, uint8_t*);
int d2_realm_client_bncs_auth_info(int, uint8_t*);
int d2_realm_client_bncs_logon(uint32_t, uint32_t, const char*, const char*, uint8_t*);
int d2_realm_client_bncs_query_realms(uint8_t*);
int d2_realm_client_bncs_logon_realm(uint32_t, uint32_t, const char*, const char*, uint8_t*);
int d2_realm_client_mcp_startup(const uint8_t*, int, uint8_t*);
int d2_realm_client_mcp_charlist(uint8_t*);
int d2_realm_client_mcp_charlogon(const char*, uint8_t*);
int d2_realm_client_mcp_creategame(uint32_t, uint32_t, uint32_t, const char*, const char*, const char*, uint8_t*);
int d2_realm_client_mcp_joingame(uint32_t, const char*, const char*, uint8_t*);
int d2_realm_mcp_protocol_selector(uint8_t*);
int d2_realm_mcp_header(uint8_t, int, uint8_t*);
int d2_realm_mcp_startup_request(const uint8_t*, int, uint8_t*);
int d2_realm_mcp_charlist2_request(uint32_t, uint8_t*);
int d2_realm_mcp_charlogon_request(const char*, uint8_t*);
int d2_realm_mcp_creategame_request(uint16_t, uint32_t, uint8_t, const char*, const char*, const char*, uint8_t*);
int d2_realm_mcp_joingame_request(uint16_t, const char*, const char*, uint8_t*);
int d2_realm_greeting_parse(const uint8_t*, int);
int d2_realm_d2gs_frame_size(const uint8_t*, int);
int d2_realm_huffman_decompress(const uint8_t*, int, uint8_t*, int);
int d2_realm_huffman_compress(const uint8_t*, int, uint8_t*, int);
int d2_realm_d2gs_gamelogon(uint32_t, uint16_t, uint8_t, uint32_t, const char*, uint8_t*);
int d2_realm_d2gs_ping(uint32_t*, uint32_t, int, uint32_t, uint32_t, uint8_t*);
uint32_t d2_realm_d2gs_ping_module(const uint32_t*, char*);
int d2_realm_d2gs_pong(uint32_t*, const uint8_t*, int, uint32_t);
int d2_realm_bncs_checkrev_classic(const char*, const char*, const uint8_t*, const uint32_t*, uint32_t*);
uint32_t d2_realm_bncs_exe_version(const uint8_t*, uint32_t);
int d2_realm_bncs_auth_info_env(int, uint32_t, uint32_t, int32_t, uint32_t, uint32_t, const char*, const char*, uint8_t*);
int d2_realm_bncs_auth_check_env(uint32_t*, uint32_t, uint32_t, uint32_t, int, const char*, const char*, const uint8_t*, const uint32_t*,
                                 const char*, const char*, const char*, const char*, uint8_t*);
}

static std::vector<uint8_t> heap;
static std::vector<std::string> strs;
static std::vector<std::string> raw;

static uint32_t N(size_t i) { return (uint32_t)strtoull(raw[i].c_str() + 2, nullptr, 0); }
static uint8_t* P(size_t i) { return heap.data() + strtoull(raw[i].c_str() + 2, nullptr, 0); }
static const char* S(size_t i) { return strs[i].c_str(); }

int main(int argc, char** argv) {
    if (argc < 3) return 2;
    FILE* f = fopen(argv[1], "rb");
    if (!f) return 2;
    fseek(f, 0, SEEK_END); heap.resize((size_t)ftell(f)); fseek(f, 0, SEEK_SET);
    if (fread(heap.data(), 1, heap.size(), f) != heap.size()) return 2;
    fclose(f);
    const std::string fn = argv[2];
    for (int i = 3; i < argc; i++) {
        raw.emplace_back(argv[i]);
        std::string s;
        if (raw.back().rfind("s:", 0) == 0)
            for (size_t k = 2; k + 1 < raw.back().size(); k += 2) s += (char)strtoul(raw.back().substr(k, 2).c_str(), nullptr, 16);
        strs.push_back(s);
    }
    long r = 0;
    if (fn == "d2_realm_bncs_header") r = d2_realm_bncs_header(N(0), N(1), P(2));
    else if (fn == "d2_realm_bncs_checkrev") r = d2_realm_bncs_checkrev(S(0), S(1), N(2), (char*)P(3));
    else if (fn == "d2_realm_bncs_auth_info_request") r = d2_realm_bncs_auth_info_request(S(0), P(1));
    else if (fn == "d2_realm_bncs_auth_check_request") r = d2_realm_bncs_auth_check_request(N(0), N(1), N(2), N(3), P(4), N(5), S(6), S(7), P(8));
    else if (fn == "d2_realm_bncs_logonresponse2_request") r = d2_realm_bncs_logonresponse2_request(N(0), N(1), S(2), S(3), P(4));
    else if (fn == "d2_realm_bncs_logonrealmex_request") r = d2_realm_bncs_logonrealmex_request(N(0), N(1), S(2), S(3), P(4));
    else if (fn == "d2_realm_client_bncs_auth_info") r = d2_realm_client_bncs_auth_info((int)N(0), P(1));
    else if (fn == "d2_realm_client_bncs_logon") r = d2_realm_client_bncs_logon(N(0), N(1), S(2), S(3), P(4));
    else if (fn == "d2_realm_client_bncs_query_realms") r = d2_realm_client_bncs_query_realms(P(0));
    else if (fn == "d2_realm_client_bncs_logon_realm") r = d2_realm_client_bncs_logon_realm(N(0), N(1), S(2), S(3), P(4));
    else if (fn == "d2_realm_client_mcp_startup") r = d2_realm_client_mcp_startup(P(0), (int)N(1), P(2));
    else if (fn == "d2_realm_client_mcp_charlist") r = d2_realm_client_mcp_charlist(P(0));
    else if (fn == "d2_realm_client_mcp_charlogon") r = d2_realm_client_mcp_charlogon(S(0), P(1));
    else if (fn == "d2_realm_client_mcp_creategame") r = d2_realm_client_mcp_creategame(N(0), N(1), N(2), S(3), S(4), S(5), P(6));
    else if (fn == "d2_realm_client_mcp_joingame") r = d2_realm_client_mcp_joingame(N(0), S(1), S(2), P(3));
    else if (fn == "d2_realm_mcp_protocol_selector") r = d2_realm_mcp_protocol_selector(P(0));
    else if (fn == "d2_realm_mcp_header") r = d2_realm_mcp_header(N(0), N(1), P(2));
    else if (fn == "d2_realm_mcp_startup_request") r = d2_realm_mcp_startup_request(P(0), N(1), P(2));
    else if (fn == "d2_realm_mcp_charlist2_request") r = d2_realm_mcp_charlist2_request(N(0), P(1));
    else if (fn == "d2_realm_mcp_charlogon_request") r = d2_realm_mcp_charlogon_request(S(0), P(1));
    else if (fn == "d2_realm_mcp_creategame_request") r = d2_realm_mcp_creategame_request(N(0), N(1), N(2), S(3), S(4), S(5), P(6));
    else if (fn == "d2_realm_mcp_joingame_request") r = d2_realm_mcp_joingame_request(N(0), S(1), S(2), P(3));
    else if (fn == "d2_realm_greeting_parse") r = d2_realm_greeting_parse(P(0), N(1));
    else if (fn == "d2_realm_d2gs_frame_size") r = d2_realm_d2gs_frame_size(P(0), N(1));
    else if (fn == "d2_realm_huffman_decompress") r = d2_realm_huffman_decompress(P(0), N(1), P(2), N(3));
    else if (fn == "d2_realm_huffman_compress") r = d2_realm_huffman_compress(P(0), N(1), P(2), N(3));
    else if (fn == "d2_realm_d2gs_gamelogon") r = d2_realm_d2gs_gamelogon(N(0), N(1), N(2), N(3), S(4), P(5));
    else if (fn == "d2_realm_d2gs_ping") r = d2_realm_d2gs_ping((uint32_t*)P(0), N(1), (int)N(2), N(3), N(4), P(5));
    else if (fn == "d2_realm_d2gs_ping_module") r = d2_realm_d2gs_ping_module((const uint32_t*)P(0), (char*)P(1));
    else if (fn == "d2_realm_d2gs_pong") r = d2_realm_d2gs_pong((uint32_t*)P(0), P(1), (int)N(2), N(3));
    else if (fn == "d2_realm_bncs_checkrev_classic") r = d2_realm_bncs_checkrev_classic(S(0), S(1), P(2), (const uint32_t*)P(3), (uint32_t*)P(4));
    else if (fn == "d2_realm_bncs_exe_version") r = d2_realm_bncs_exe_version(P(0), N(1));
    else if (fn == "d2_realm_bncs_auth_info_env") r = d2_realm_bncs_auth_info_env((int)N(0), N(1), N(2), (int32_t)N(3), N(4), N(5), S(6), S(7), P(8));
    else if (fn == "d2_realm_bncs_auth_check_env") r = d2_realm_bncs_auth_check_env((uint32_t*)P(0), N(1), N(2), N(3), (int)N(4), S(5), S(6), P(7),
                                                                                   (const uint32_t*)P(8), S(9), S(10), S(11), S(12), P(13));
    else { fprintf(stderr, "unknown export %s\n", fn.c_str()); return 3; }
    f = fopen(argv[1], "wb");
    if (!f || fwrite(heap.data(), 1, heap.size(), f) != heap.size()) return 2;
    fclose(f);
    printf("%ld\n", r);
    return 0;
}

// The Battle.net game list on the BNCS socket: SID_GETADVLISTEX (0x09) asks, the reply lists the open games.
// Reply body: `dword count`, then per game a 0x20 byte header (+0 type, +2 extension, +4 language, +8 family,
// +0xA port, +0xC address, +0x18 status, +0x1C uptime) and `name\0 password\0 statstring\0`, the statstring's
// first character the difficulty ('0'..'2' softcore, '3' and up hardcore) and the rest the description.
// Pure of the page: bytes in, rows out.
const enc = new TextEncoder(), dec = new TextDecoder();
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
export const GAME_LIST_ID = 0x09;
// Retail's refresh period while the join panel is up (D2Multi's idle tick: 15 000 ms since the last request).
export const GAME_REFRESH_MS = 15000;

// The request, as Bnclient's list sender builds it: game type and sub type (words), view filter and reserved
// (dwords) all zero, 0x19 games wanted (dword), then an empty name, password and statstring. 19 body bytes.
// The two arguments D2Multi passes with it are not read.
export function getGamesPacket(count = 0x19) {
  const body = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, count & 255, 0, 0, 0, 0, 0, 0];
  return Uint8Array.from([0xff, GAME_LIST_ID, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]);
}

export function parseGameList(body) {
  const games = [];
  if (body.length < 4) return games;
  const count = u32(body, 0);
  let o = 4;
  const cstr = () => { const e = body.indexOf(0, o); if (e < 0) return null; const s = dec.decode(body.subarray(o, e)); o = e + 1; return s; };
  for (let i = 0; i < count; i++) {
    if (o + 0x20 > body.length) break;
    const h = o; o += 0x20;
    const name = cstr(), password = name === null ? null : cstr(), stats = password === null ? null : cstr();
    if (stats === null) break;
    games.push({ name, password: password !== '', difficulty: stats.length ? stats.charCodeAt(0) - 0x30 : 0, description: stats.slice(1),
      status: u32(body, h + 0x18), uptime: u32(body, h + 0x1c), address: `${body[h + 0xc]}.${body[h + 0xd]}.${body[h + 0xe]}.${body[h + 0xf]}` });
  }
  return games;
}

// D2Multi 0x13F20's keep rule, with reach = progress / 5 + 1. A softcore character lists a game whose difficulty
// digit is at most reach; a hardcore one lists digits 3 .. 3 + reach.
export function filterGames(games, character) {
  const reach = (((character?.progression ?? 0) & 0x1f) / 5 | 0) + 1;
  return games.filter((g) => (character?.hardcore ? g.difficulty >= 3 && g.difficulty - 3 <= reach : g.difficulty <= reach));
}

export const encodeGameList = (games) => {
  const parts = [Uint8Array.of(games.length, 0, 0, 0)];
  games.forEach((g, i) => {
    const h = new Uint8Array(0x20); h[0] = 1; h[4] = 9; h[5] = 4; h[8] = 2; h[0xc] = 192; h[0xd] = 0; h[0xe] = 2; h[0xf] = i + 1;
    parts.push(h, enc.encode(`${g.name}\0${g.password ?? ''}\0${String.fromCharCode(0x30 + (g.difficulty ?? 0))}${g.description ?? ''}\0`));
  });
  const out = new Uint8Array(parts.reduce((n, p) => n + p.length, 0)); let o = 0; for (const p of parts) { out.set(p, o); o += p.length; }
  return out;
};

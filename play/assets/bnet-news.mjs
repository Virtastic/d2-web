// The pre-chat lobby's two server texts. News: SID_NEWS_INFO (BNCS 0x46), asked once at lobby entry when the lobby is not
// in chat (D2Multi+0xE150 calls BNClient slot 0xA8 = +0x12E40), each reply merged by BNClient+0x12C30 into a ts-ascending
// list and redrawn by +0x12810 into the news box (D2Multi+0xC490 clears it, +0xC510 adds each entry). MOTD: MCP 0x12, its
// reply parts kept key-sorted by D2Multi+0x9E30 and joined by +0x9D60 into the server box. Pure: bytes in, lines out.
const dec = new TextDecoder('latin1'), utf8 = new TextDecoder();
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;

// The request body is the newest timestamp the client holds (BNClient's DAT_8e0, 0 with no cache): the server sends what is newer.
export const newsRequestPacket = (newest = 0) => Uint8Array.of(0xff, 0x46, 8, 0, newest & 255, (newest >>> 8) & 255, (newest >>> 16) & 255, (newest >>> 24) & 255);

// BNClient+0x12C30 over one reply body: count = body[0] as a signed char (over 0x100 as unsigned is fatal, so a byte of 0x80 and up
// is), the oldest timestamp at +5 is the shown threshold, the newest at +9 the next request's value, then `[ts u32][text\0]` entries.
// A ts of 0 goes straight to the add-entry callback, which shows nothing for it. Any other ts is inserted ascending (an equal ts
// replaces the text), text cut at 0x400. Returns the entries the redraw shows, in the order it adds them: the newest first, down to
// the first not older than the threshold.
export function createNews() {
  const entries = []; let newest = 0, threshold = 0;
  // +0x12810's walk: from the newest entry down to the first one not older than the threshold.
  const shown = () => { let older = 0; while (older < entries.length && entries[older].ts < threshold) older++; return entries.slice(older).reverse().map((e) => ({ ...e })); };
  return {
    get newest() { return newest; },
    get entries() { return entries.map((e) => ({ ...e })); },
    // The redraw from what is held (retail's cache load at lobby entry, +0x12A70, before the request goes out).
    shown,
    apply(body) {
      if (body.length < 0xd) return null;
      const count = body[0] >= 0x80 ? body[0] - 0x100 : body[0];
      if (count < 0) throw new Error('news reply count is over 0x100');
      let o = 0xd;
      for (let i = 0; i < count; i++) {
        const ts = u32(body, o); o += 4;
        let end = body.indexOf(0, o); if (end < 0) end = body.length;
        const text = dec.decode(body.subarray(o, Math.min(end, o + 0x400))); o = end + 1;
        if (ts === 0) continue;
        let at = entries.length - 1;
        while (at >= 0 && entries[at].ts > ts) at--;
        if (at >= 0 && entries[at].ts === ts) { entries[at].text = text; continue; }
        entries.splice(at + 1, 0, { ts, text });
      }
      threshold = u32(body, 5); newest = u32(body, 9);
      return shown();
    },
  };
}

// D2Multi+0xC510's three lines per entry: the long date of the timestamp in local time (GetDateFormatA, colour 0), the text split
// on newlines (colour 4), an empty line.
export function newsLines(shown, formatDate = (d) => d.toLocaleDateString('en-US', { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' })) {
  return shown.flatMap(({ ts, text }) => [{ text: formatDate(new Date(ts * 1000)), colour: 0 }, ...text.split('\n').map((t) => ({ text: t, colour: 4 })), { text: '', colour: 0 }]);
}

// D2Multi+0x9E30 / +0x9D60: each MCP 0x12 part is filed under its key byte (a key seen again replaces its text, cut at 0x200), the
// parts in ascending key order are joined, at most 0x200 bytes, and the result read as UTF-8.
export function createMotd() {
  const parts = new Map();
  return {
    add(key, textBytes) {
      let end = textBytes.indexOf(0); if (end < 0) end = textBytes.length;
      parts.set(key, textBytes.subarray(0, Math.min(end, 0x200)));
      return this.text;
    },
    get text() {
      const out = []; for (const k of [...parts.keys()].sort((a, b) => a - b)) { if (out.length >= 0x200) break; out.push(...parts.get(k).subarray(0, 0x200 - out.length)); }
      return utf8.decode(Uint8Array.from(out));
    },
  };
}

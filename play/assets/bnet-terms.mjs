// The terms of service file, fetched as the original client fetches it.
//
// CREATE NEW ACCOUNT's terms screen (D2Launch+0x18E10) names the file "tos-unicode_" + the language's country code
// (D2Lang 10004 with selector 2: "USA" for English) + ".txt" and asks the Battle.net client (BNClient interface slot
// +0x50, BNClient+0xE740) to download it. That download is a file transfer on a second connection to the Battle.net
// port: one protocol byte 2, then a request, and the server answers with a header and the file bytes
// (BNClient+0xDC50). The screen then reads the file: UTF-16 with either byte order mark, or 8-bit text widened a
// byte at a time.
//
// The realm serves the file; nothing here ships terms text of its own.

/** BNClient+0xE740's request kind and its connection's protocol byte. */
export const BNFTP_PROTOCOL = 0x02;
/** BNClient+0xDC50: a header of 0x100 bytes or more ends the process; here it fails the download. */
export const BNFTP_HEADER_MAX = 0x100;
/** BNClient+0xDC50: SO_RCVTIMEO is 15000 ms. */
export const BNFTP_RECV_MS = 15000;
/** BNClient+0xDC50: a refused connect is retried after 1000 ms, and the fourth refusal gives up. */
export const BNFTP_CONNECT_TRIES = 4;
export const BNFTP_RETRY_MS = 1000;

/** D2Launch+0x18E10: "tos-unicode_" + D2Lang 10004(selector 2) + ".txt". */
export function termsFileName(country = 'USA') { return `tos-unicode_${country}.txt`; }

/**
 * BNClient+0xE740's request: word length (name + 0x21), word 0x100, 'IX86', 'D2DV', banner id 0, banner extension 0,
 * start position 0, the cached copy's FILETIME (0: the browser keeps no cache) and the name with its NUL.
 */
export function bnftpRequest(name, { startPos = 0, fileTime = 0n, bannerId = 0, bannerExt = 0 } = {}) {
  const n = name.length;
  const out = new Uint8Array(0x21 + n);
  const v = new DataView(out.buffer);
  v.setUint16(0, 0x21 + n, true);
  v.setUint16(2, 0x0100, true);
  v.setUint32(4, 0x49583836, true); // 'IX86'
  v.setUint32(8, 0x44324456, true); // 'D2DV'
  v.setUint32(0x0c, bannerId >>> 0, true);
  v.setUint32(0x10, bannerExt >>> 0, true);
  v.setUint32(0x14, startPos >>> 0, true);
  v.setBigUint64(0x18, BigInt(fileTime), true);
  for (let i = 0; i < n; i++) out[0x20 + i] = name.charCodeAt(i) & 0xff;
  return out;
}

/**
 * BNClient+0xDC50's reading of the answer: a dword header size (below 0x100), the header (file size at +4, FILETIME at
 * +0x10, name at +0x18), then exactly the file size in bytes. Returns null while incomplete.
 */
export function parseBnftpReply(bytes) {
  if (bytes.length < 4) return null;
  const v = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const headerSize = v.getUint32(0, true);
  if (headerSize >= BNFTP_HEADER_MAX || headerSize < 0x18) throw new Error(`file transfer header of ${headerSize} bytes`);
  if (bytes.length < headerSize) return null;
  const size = v.getUint32(4, true);
  if (bytes.length < headerSize + size) return null;
  const nameEnd = bytes.indexOf(0, 0x18);
  return {
    headerSize, size, bannerId: v.getUint32(8, true), bannerExt: v.getUint32(12, true), fileTime: v.getBigUint64(0x10, true),
    name: latin1(bytes.subarray(0x18, nameEnd < 0 || nameEnd > headerSize ? headerSize : nameEnd)),
    data: bytes.slice(headerSize, headerSize + size),
  };
}

/**
 * D2Launch+0x18E10's reading of the file: two bytes or fewer is no text; a 0xFEFF mark is UTF-16LE, a 0xFFFE mark is
 * UTF-16 byte-swapped (D2Launch+0x1000), anything else is widened a byte at a time (Unicode::win2Unicode, D2Lang+0x8320,
 * which stops at a NUL).
 */
export function decodeTermsText(data) {
  if (!data || data.length <= 2) return '';
  const mark = data[0] | (data[1] << 8);
  if (mark === 0xfeff || mark === 0xfffe) {
    const units = (data.length - 2) >> 1;
    let s = '';
    for (let i = 0; i < units; i++) {
      const a = data[2 + i * 2], b = data[3 + i * 2];
      s += String.fromCharCode(mark === 0xfeff ? a | (b << 8) : (a << 8) | b);
    }
    return s;
  }
  const end = data.indexOf(0);
  return latin1(data.subarray(0, end < 0 ? data.length : end));
}

function latin1(bytes) { let s = ''; for (const b of bytes) s += String.fromCharCode(b); return s; }
const transportFailure = (message) => Object.assign(new Error(message), { transport: true });

/**
 * Download one file on a fresh connection from `dial()` (a transport with ready/send/onData/onClose/close), as
 * BNClient+0xDC50 does: up to four connects a second apart, the protocol byte and the request as two sends, then the
 * answer. Any failure is a transport failure (the terms screen then shows its disconnected notice).
 */
export async function bnftpDownload(dial, name, { request = {}, recvMs = BNFTP_RECV_MS, tries = BNFTP_CONNECT_TRIES, retryMs = BNFTP_RETRY_MS,
  setTimer = setTimeout, clearTimer = clearTimeout } = {}) {
  let link = null;
  for (let attempt = 1; ; attempt++) {
    try { link = dial(); await link.ready(); break; } catch {
      try { link?.close(); } catch { /* already closed */ }
      link = null;
      if (attempt >= tries) throw transportFailure('the file transfer connection could not open');
      await new Promise((r) => setTimer(r, retryMs));
    }
  }
  try {
    return await new Promise((resolve, reject) => {
      let buf = new Uint8Array(0), timer = null;
      const arm = () => { clearTimer(timer); timer = setTimer(() => reject(transportFailure('the file transfer timed out')), recvMs); };
      link.onData((chunk) => {
        const next = new Uint8Array(buf.length + chunk.length); next.set(buf); next.set(chunk, buf.length); buf = next;
        let reply;
        try { reply = parseBnftpReply(buf); } catch (error) { clearTimer(timer); reject(transportFailure(String(error.message))); return; }
        if (reply) { clearTimer(timer); resolve(reply); } else arm();
      });
      link.onClose(() => { clearTimer(timer); reject(transportFailure('the file transfer connection closed')); });
      arm();
      if (!link.send(Uint8Array.of(BNFTP_PROTOCOL)) || !link.send(bnftpRequest(name, request))) { clearTimer(timer); reject(transportFailure('the file transfer request could not be sent')); }
    });
  } finally { try { link.close(); } catch { /* already closed */ } }
}

/** The terms screen's text: download the language's file and read it as D2Launch+0x18E10 does. */
export async function fetchTermsOfService(dial, { country = 'USA', ...options } = {}) {
  const reply = await bnftpDownload(dial, termsFileName(country), options);
  return decodeTermsText(reply.data);
}

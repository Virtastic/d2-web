// A minimal reader for the protodef packet schemas in reference/diablo2-protocol/data/1.13.
//
// Those schemas are an INDEPENDENT definition of the same 1.13 protocols this repo implements,
// written by someone else from their own reading of the wire. Decoding realmd's real replies with
// them is the strongest check available without a stock client: if our composer and their schema
// disagree about a field's type, order or endianness, the decode lands on the wrong bytes.
//
// Only the subset the 1.13 sid/mcp schemas actually use is supported. Anything else throws rather
// than guessing -- a decoder that silently skips an unknown type would turn a real disagreement
// into a pass.
import { readFileSync, existsSync } from 'node:fs';
import { join } from 'node:path';
import { ROOT } from './realmd.mjs';

// reference/ is vendored prior art that lives only in a local checkout -- it is not committed, so
// CI does not have it. Return null instead of throwing so the caller can skip rather than fail.
export const loadSchema = (name) => {
  const path = join(ROOT, 'reference', 'diablo2-protocol', 'data', '1.13', `${name}.json`);
  if (!existsSync(path)) return null;
  return JSON.parse(readFileSync(path, 'utf8'));
};

const NATIVE = {
  lu8: [1, (b, o) => b.readUInt8(o)],
  lu16: [2, (b, o) => b.readUInt16LE(o)],
  lu32: [4, (b, o) => b.readUInt32LE(o)],
  lu64: [8, (b, o) => b.readBigUInt64LE(o)],
  u16: [2, (b, o) => b.readUInt16BE(o)],   // big-endian: the schema uses this for the realm port
  u32: [4, (b, o) => b.readUInt32BE(o)],
  bool: [1, (b, o) => b.readUInt8(o) !== 0],
};

// Decode `type` from buf at off. Returns { value, off }. `ctx` carries already-decoded sibling
// fields so an array can size itself by one of them.
function read(schema, type, buf, off, ctx) {
  if (typeof type === 'string') {
    if (type === 'string' || type === 'cstring') {
      const end = buf.indexOf(0, off);
      if (end < 0) throw new Error(`unterminated cstring at ${off}`);
      return { value: buf.subarray(off, end).toString('latin1'), off: end + 1 };
    }
    if (NATIVE[type]) {
      const [n, fn] = NATIVE[type];
      if (off + n > buf.length) throw new Error(`${type} at ${off} runs past ${buf.length}`);
      return { value: fn(buf, off), off: off + n };
    }
    // A named type defined elsewhere in the schema.
    const named = schema.types?.[type];
    if (named && named !== 'native') return read(schema, named, buf, off, ctx);
    throw new Error(`unsupported protodef type ${JSON.stringify(type)}`);
  }
  const [kind, args] = type;
  if (kind === 'container') {
    const value = {};
    let o = off;
    for (const f of args) {
      const r = read(schema, f.type, buf, o, value);
      value[f.name] = r.value;
      o = r.off;
    }
    return { value, off: o };
  }
  if (kind === 'array') {
    let o = off;
    let count;
    if (typeof args.count === 'number') count = args.count;
    else if (typeof args.count === 'string') count = Number(ctx?.[args.count]);
    else if (args.countType) { const r = read(schema, args.countType, buf, o, ctx); count = Number(r.value); o = r.off; }
    else throw new Error('array with no count');
    const value = [];
    for (let i = 0; i < count; i++) { const r = read(schema, args.type, buf, o, ctx); value.push(r.value); o = r.off; }
    return { value, off: o };
  }
  if (kind === 'buffer') {
    const n = args.count;
    if (typeof n !== 'number') throw new Error('buffer without a fixed count');
    return { value: buf.subarray(off, off + n), off: off + n };
  }
  throw new Error(`unsupported protodef kind ${kind}`);
}

// Decode a message body by its schema name, e.g. decode(sid, 'toClient', 'SID_AUTH_INFO', body).
// Throws if the decode does not consume the body exactly -- a short or long read means the layouts
// disagree, which is the whole point of doing this.
export function decode(schema, side, name, body) {
  const type = schema[side]?.types?.[name];
  if (!type) throw new Error(`${side}.${name} not in schema`);
  const { value, off } = read(schema, type, body, 0, null);
  if (off !== body.length) throw new Error(`${side}.${name}: decoded ${off} of ${body.length} bytes -- layouts disagree`);
  return value;
}

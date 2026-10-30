// Importing a .d2s the player owns into the browser character store.
//
// Validation is the file format's own: signature at 0, the size field at 8 equal to the file length,
// and the checksum at 12 (PLRSAVE2's rotate-left-and-add over the file with that field zeroed).
// A file that fails any of them is never stored. The store write is createSaveStore().put(), the same
// call a native Save and Exit makes, so the record shape is identical.
import { readSaveHeader } from './save-store.mjs';

export function d2sChecksum(bytes) {
  let sum = 0;
  for (let i = 0; i < bytes.length; i++) {
    const b = i >= 12 && i < 16 ? 0 : bytes[i];
    sum = (((sum << 1) | (sum >>> 31)) + b) >>> 0;
  }
  return sum;
}

/** Returns { ok: true, header } or { ok: false, error } -- never throws. */
export function validateD2s(bytes) {
  if (!(bytes instanceof Uint8Array) || bytes.length < 0x30) return { ok: false, error: 'not a .d2s file (too small)' };
  const header = readSaveHeader(bytes);
  if (!header) return { ok: false, error: 'not a .d2s file (bad signature)' };
  if (header.size !== bytes.length) return { ok: false, error: `bad .d2s size (header says ${header.size}, file is ${bytes.length})` };
  const stored = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(12, true);
  if (stored !== d2sChecksum(bytes)) return { ok: false, error: 'bad .d2s checksum' };
  if (!header.name) return { ok: false, error: 'the .d2s has no character name' };
  return { ok: true, header };
}

/**
 * Validate and store. `files` is [{ name, bytes }]; the .d2s is the main file, .d2x/.sss ride along as
 * aux records when present. `confirmOverwrite(name)` is asked only when the name already exists.
 */
export async function importCharacterFiles(files, { store, confirmOverwrite = () => true } = {}) {
  const ext = (f) => String(f.name).toLowerCase().replace(/^.*\./, '');
  const d2s = files.find((f) => ext(f) === 'd2s');
  if (!d2s) return { status: 'error', error: 'no .d2s file was provided' };
  const checked = validateD2s(d2s.bytes);
  if (!checked.ok) return { status: 'error', error: checked.error };
  const { name } = checked.header;
  if ((await store.get(name)) && !confirmOverwrite(name)) return { status: 'cancelled', name };
  await store.put(d2s.bytes);
  for (const f of files) {
    if (ext(f) === 'd2x') await store.putAux(`_LOD_${name}.d2x`, f.bytes);
    else if (ext(f) === 'sss') await store.putAux('_LOD_SharedStashSave.sss', f.bytes);
  }
  return { status: 'imported', name, header: checked.header };
}

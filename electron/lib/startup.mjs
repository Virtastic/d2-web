// The desktop app's startup decisions, pure (no Electron): which folder a launch uses, what a picked folder or a
// set of picked .mpq files means, and the words of the modal. main.mjs does the dialogs and the window;
// test/desktop-startup.test.mjs runs this file on fake folders.
import { statSync } from 'node:fs';
import { dirname, resolve } from 'node:path';

export const NOT_FOUND = {
  title: 'Diablo II files not found',
  body: 'D2-Web needs your own copy of Diablo II (1.13c). Choose the folder or the .mpq files; they stay on this computer and nothing is uploaded or copied into the app.',
  choose: 'Choose files…',
  notNow: 'Not now',
};

/**
 * Every startup: the saved folder first, then the usual install places. check is checkRetailDir.
 * Returns { dir, source: 'saved' | 'detected' } or null. A saved folder that no longer validates is skipped, not fatal.
 */
export function resolveStartup({ saved, detect, check }) {
  if (saved) { let r; try { r = check(saved); } catch { r = null; } if (r && r.ok) return { dir: saved, source: 'saved' }; }
  const found = detect();
  return found ? { dir: found, source: 'detected' } : null;
}

/**
 * What the player picked becomes the one folder the shell serves from.
 * paths is what the native chooser returned: one folder, or one or more files. Files must be .mpq and share a folder
 * (the shell serves a folder; it never copies an archive). Returns { dir } or { error: string }.
 */
export function folderFromPick(paths, { isDir = (p) => { try { return statSync(p).isDirectory(); } catch { return false; } } } = {}) {
  const list = (paths || []).filter(Boolean);
  if (!list.length) return { error: 'Nothing was chosen.' };
  if (list.length === 1 && isDir(list[0])) return { dir: resolve(list[0]) };
  if (list.some((p) => isDir(p))) return { error: 'Choose one folder, or the .mpq files, not both.' };
  if (list.some((p) => !/\.mpq$/i.test(p))) return { error: 'Choose a folder, or .mpq files only.' };
  const dirs = new Set(list.map((p) => dirname(resolve(p))));
  if (dirs.size > 1) return { error: 'The .mpq files must all come from one folder.' };
  return { dir: [...dirs][0] };
}

/** A picked folder or files give { ok: true, dir } or { ok: false, message } with every problem the 1.13c check found. */
export function validatePick(paths, check, opts) {
  const f = folderFromPick(paths, opts);
  if (f.error) return { ok: false, message: f.error };
  const r = check(f.dir);
  if (r.ok) return { ok: true, dir: f.dir };
  return { ok: false, message: `${r.problems.join('\n')}\n\nThe game needs a Diablo II 1.13c install: d2data.mpq and the 1.13c Patch_D2.mpq must be together in the folder.` };
}

/** The saved config for a chosen or found folder. */
export const configFor = (dir, source) => ({ retailDir: dir, ...(source === 'detected' ? { detected: true } : {}) });

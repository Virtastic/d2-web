// Retail's Resolution option saves the selected mode. The browser keeps the same two-choice
// preference in its own storage namespace; no native registry path is inferred here.
export const VIDEO_RESOLUTION_STORAGE_KEY = 'd2web.video.resolution';
export const VIDEO_RESOLUTIONS = Object.freeze([
  Object.freeze({ name: '640x480', width: 640, height: 480 }),
  Object.freeze({ name: '800x600', width: 800, height: 600 }),
]);

export function createVideoResolutionSettings(options = {}) {
  let index = 1, selected = false;
  let storage = null;
  try {
    storage = Object.hasOwn(options, 'storage') ? options.storage : globalThis.localStorage;
    const raw = storage?.getItem(VIDEO_RESOLUTION_STORAGE_KEY);
    if (raw !== null && raw !== undefined) {
      const saved = JSON.parse(raw);
      if (Number.isInteger(saved) && saved >= 0 && saved < VIDEO_RESOLUTIONS.length) {
        index = saved; selected = true;
      }
    }
  } catch { /* blocked or malformed browser storage falls back to the default */ }
  return {
    get index() { return index; },
    get selected() { return selected; },
    get mode() { return VIDEO_RESOLUTIONS[index]; },
    set(nextIndex) {
      if (!Number.isInteger(nextIndex) || nextIndex < 0 || nextIndex >= VIDEO_RESOLUTIONS.length)
        throw new RangeError(`invalid video resolution index ${nextIndex}`);
      index = nextIndex; selected = true;
      try { storage?.setItem(VIDEO_RESOLUTION_STORAGE_KEY, JSON.stringify(index)); }
      catch { /* the live option still applies when persistence is unavailable */ }
      return this.mode;
    },
  };
}

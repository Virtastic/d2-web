import { decodeWav } from './wav.mjs';

export const TITLE_MUSIC = 'introedit.wav';
const MUSIC_VOLUME_MAX = 255;

function tableRows(text, tableName) {
  if (typeof text !== 'string') throw new TypeError(`${tableName} must be text`);
  const lines = text.replace(/\r/g, '').split('\n').filter((line) => line.trim());
  if (!lines.length) throw new Error(`${tableName} is empty`);
  const columns = lines[0].split('\t').map((value) => value.trim().toLowerCase());
  return { columns, rows: lines.slice(1).map((line) => line.split('\t')) };
}

function column(columns, name, tableName) {
  const index = columns.indexOf(name.toLowerCase());
  if (index < 0) throw new Error(`${tableName} is missing ${name}`);
  return index;
}

function integerField(row, index) {
  const value = row[index]?.trim() ?? '';
  return /^\d+$/.test(value) ? Number(value) : -1;
}

function integerInput(value, label) {
  if (typeof value === 'number' && Number.isSafeInteger(value)) return value;
  if (typeof value === 'string' && /^-?\d+$/.test(value.trim())) {
    const parsed = Number(value);
    if (Number.isSafeInteger(parsed)) return parsed;
  }
  throw new TypeError(`${label} must be an integer`);
}

/** Follow the native Levels -> SoundEnviron -> Sounds selector for one retail level. */
export function resolveLevelMusic(levelId, levelsTxt, soundEnvironTxt, soundsTxt) {
  const level = Number(levelId);
  if (!Number.isSafeInteger(level) || level < 0) throw new TypeError('retail level id is required');
  const levels = tableRows(levelsTxt, 'Levels.txt');
  const levelIdColumn = column(levels.columns, 'Id', 'Levels.txt');
  const levelEnvColumn = column(levels.columns, 'SoundEnv', 'Levels.txt');
  const levelRows = levels.rows.filter((row) => integerField(row, levelIdColumn) === level);
  if (levelRows.length !== 1) throw new Error(`expected one Levels.txt row for level ${level}, found ${levelRows.length}`);
  const environmentId = integerField(levelRows[0], levelEnvColumn);
  if (environmentId <= 0) return null;

  const environments = tableRows(soundEnvironTxt, 'SoundEnviron.txt');
  const envIdColumn = column(environments.columns, 'Index', 'SoundEnviron.txt');
  const envSongColumn = column(environments.columns, 'Song', 'SoundEnviron.txt');
  const envRows = environments.rows.filter((row) => integerField(row, envIdColumn) === environmentId);
  if (envRows.length !== 1) throw new Error(`expected one SoundEnviron.txt row for environment ${environmentId}, found ${envRows.length}`);
  const soundId = integerField(envRows[0], envSongColumn);
  if (soundId <= 0) return null;

  const sounds = tableRows(soundsTxt, 'Sounds.txt');
  const soundIdColumn = column(sounds.columns, 'Index', 'Sounds.txt');
  const soundNameColumn = column(sounds.columns, 'Sound', 'Sounds.txt');
  const soundFileColumn = column(sounds.columns, 'FileName', 'Sounds.txt');
  const soundVolumeColumn = column(sounds.columns, 'Volume', 'Sounds.txt');
  const musicColumn = column(sounds.columns, 'Music Vol', 'Sounds.txt');
  const soundRows = sounds.rows.filter((row) => integerField(row, soundIdColumn) === soundId);
  if (soundRows.length !== 1) throw new Error(`expected one Sounds.txt row for sound ${soundId}, found ${soundRows.length}`);
  const [row] = soundRows;
  const path = row[soundFileColumn]?.trim() ?? '';
  const name = row[soundNameColumn]?.trim() ?? '';
  const volume = integerField(row, soundVolumeColumn);
  if (!path || !name || volume < 0 || volume > MUSIC_VOLUME_MAX || integerField(row, musicColumn) !== 1) {
    throw new Error(`invalid music Sounds.txt row for sound ${soundId}`);
  }
  return { levelId: level, environmentId, soundId, name, path, volume };
}

/** Select the native day/night ambience and event rows for a level's SoundEnviron. */
export function resolveLevelSoundEnvironment(levelId, cycleState, levelsTxt, soundEnvironTxt) {
  const level = integerInput(levelId, 'retail level id'), cycle = integerInput(cycleState, 'native environment cycle state');
  if (level < 0) throw new TypeError('retail level id must be non-negative');
  const levels = tableRows(levelsTxt, 'Levels.txt');
  const levelIdColumn = column(levels.columns, 'Id', 'Levels.txt');
  const levelEnvColumn = column(levels.columns, 'SoundEnv', 'Levels.txt');
  const levelRows = levels.rows.filter((row) => integerField(row, levelIdColumn) === level);
  if (levelRows.length !== 1) throw new Error(`expected one Levels.txt row for level ${level}, found ${levelRows.length}`);
  const environmentId = integerField(levelRows[0], levelEnvColumn);
  if (environmentId < 0) throw new Error(`invalid SoundEnv for level ${level}`);

  const environments = tableRows(soundEnvironTxt, 'SoundEnviron.txt');
  const envIdColumn = column(environments.columns, 'Index', 'SoundEnviron.txt');
  const envRows = environments.rows.filter((row) => integerField(row, envIdColumn) === environmentId);
  if (envRows.length !== 1) throw new Error(`expected one SoundEnviron.txt row for environment ${environmentId}, found ${envRows.length}`);
  const [row] = envRows;
  const ambienceColumn = column(environments.columns, cycle >= 1 && cycle <= 3 ? 'Night Ambience' : 'Day Ambience', 'SoundEnviron.txt');
  const eventColumn = column(environments.columns, cycle >= 1 && cycle <= 3 ? 'Night Event' : 'Day Event', 'SoundEnviron.txt');
  const delayColumn = column(environments.columns, 'Event Delay', 'SoundEnviron.txt');
  const ambienceId = integerField(row, ambienceColumn), eventId = integerField(row, eventColumn), eventDelay = integerField(row, delayColumn);
  if (ambienceId < 0 || eventId < 0 || eventDelay < 0) throw new Error(`invalid ambience/event row for SoundEnviron ${environmentId}`);
  return { levelId: level, environmentId, cycleState: cycle, ambienceId, eventId, eventDelay };
}

/** Native FUN_6FB30450 gate over two raw values; neither input's semantic name is recovered. */
export function nativeAmbienceEnabled(levelPredicateValue, suppressionGlobalValue) {
  const levelValue = integerInput(levelPredicateValue, 'native level predicate value');
  const suppressionValue = integerInput(suppressionGlobalValue, 'native suppression global value');
  return levelValue !== 0 && suppressionValue === 0;
}

/** 1.13c FUN_6FAF5660: a new level's song is adopted once more than 0x4a ticks have passed since the level changed (or nothing plays yet). */
export function levelMusicDue(level, activeLevel, changedTick, tick) {
  return level !== activeLevel && (activeLevel < 0 || tick - changedTick > 0x4a);
}

/** Coalesce level changes, retry failed selection, and invalidate stale in-flight requests. */
export function createLevelMusicSync(playLevel, { now = Date.now, retryDelayMs = 1000 } = {}) {
  if (typeof playLevel !== 'function') throw new TypeError('level music player is required');
  let activeLevel = -1, pending = null, serial = 0, failedLevel = -1, retryAt = 0;
  function start(id) {
    const request = ++serial;
    const entry = { levelId: id, invoked: false, promise: null };
    entry.promise = Promise.resolve().then(() => {
      if (request !== serial) return null;
      entry.invoked = true;
      return playLevel(id);
    }).then((result) => {
      if (request === serial) { activeLevel = id; pending = null; failedLevel = -1; }
      return result;
    }, (error) => {
      if (request === serial) { pending = null; failedLevel = id; retryAt = now() + retryDelayMs; }
      throw error;
    });
    pending = entry;
    return entry.promise;
  }
  return {
    sync(levelId, { force = false } = {}) {
      const id = Number(levelId);
      if (!Number.isInteger(id) || id < 0) return Promise.reject(new TypeError('retail level id must be a non-negative integer'));
      if (!force && id === activeLevel) {
        if (pending) {
          const stale = pending;
          serial++; pending = null;
          if (stale.invoked) return start(id);
        }
        return Promise.resolve(null);
      }
      if (pending?.levelId === id) return pending.promise;
      if (!force && failedLevel === id && now() < retryAt) return Promise.resolve(null);
      return start(id);
    },
    reset() { serial++; activeLevel = -1; pending = null; failedLevel = -1; retryAt = 0; },
    get activeLevel() { return activeLevel; },
  };
}

/** Resolve the native source's Sounds.txt base volume for one retail music file. */
export function resolveActMusicSourceVolume(soundsTxt, path) {
  if (typeof soundsTxt !== 'string' || typeof path !== 'string') throw new TypeError('Sounds.txt and music path are required');
  const lines = soundsTxt.split(/\r?\n/);
  const columns = lines[0]?.split('\t') ?? [];
  const fileColumn = columns.indexOf('FileName'), volumeColumn = columns.indexOf('Volume'), nameColumn = columns.indexOf('Sound'), musicColumn = columns.indexOf('Music Vol');
  if (fileColumn < 0 || volumeColumn < 0 || nameColumn < 0 || musicColumn < 0) throw new Error('Sounds.txt is missing music source columns');
  const wanted = path.replaceAll('\\', '/').toLowerCase();
  const rows = lines.slice(1).map((line) => line.split('\t')).filter((row) =>
    row[fileColumn]?.replaceAll('\\', '/').toLowerCase() === wanted && row[nameColumn]?.startsWith('music_'));
  if (rows.length !== 1) throw new Error(`expected one music Sounds.txt row for ${path}, found ${rows.length}`);
  const volume = Number(rows[0][volumeColumn]);
  if (!Number.isInteger(volume) || volume < 0 || volume > MUSIC_VOLUME_MAX || Number(rows[0][musicColumn]) !== 1) {
    throw new Error(`invalid music source row for ${path}`);
  }
  return volume;
}

/** D2Client applies Music then Master with signed integer division before D2Sound's /255 scale. */
export function retailMusicChannelGain(sourceVolume, music, master) {
  const percent = (value) => Math.max(0, Math.min(100, Math.trunc(Number(value) * 100 + 1e-7)));
  let volume = Math.trunc(sourceVolume * percent(music) / 100);
  volume = Math.trunc(volume * percent(master) / 100);
  return volume / MUSIC_VOLUME_MAX;
}

// Every browser starts a fresh AudioContext 'suspended' until a user gesture (
// "Support matrix": iOS always; Chrome/Firefox once autoplay heuristics decide there was none). This
// resumes it on the FIRST pointer, key, touch or gamepad activity anywhere on the page, rather than waiting for whatever later UI click happens to call `play()` --
// so town music can start the instant a level loads, not on some second, unrelated click.
function unlockOnGesture(context, target = globalThis.window) {
  if (typeof target?.addEventListener !== 'function' || context.state !== 'suspended') return;
  const controller = new AbortController();
  let resuming = false;
  const unlock = () => {
    if (resuming || context.state !== 'suspended') return;
    resuming = true;
    let resumed;
    try { resumed = Promise.resolve(context.resume()); }
    catch { resuming = false; return; }
    resumed.then(() => {
      resuming = false;
      // A browser can reject resume() when the first event was not a qualifying user gesture.
      // Keep the listeners installed in that case so a later real click/key/touch can unlock audio.
      if (context.state !== 'suspended') controller.abort();
    }, () => { resuming = false; });
  };
  const opts = { signal: controller.signal, passive: true };
  for (const type of ['pointerdown', 'keydown', 'touchstart', 'gamepadconnected']) target.addEventListener(type, unlock, opts);
}

export function createActMusicPlayer(read, AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window) {
  if (typeof read !== 'function') throw new TypeError('retail asset reader is required');
  if (!AudioContextClass) throw new Error('Web Audio is unavailable');
  const context = new AudioContextClass(), buffers = new Map();
  unlockOnGesture(context, gestureTarget);
  // F235: the Music slider's value reaches the loop through this one node. Web Audio's GainNode is
  // the only way to attenuate it; a context without createGain (a minimal implementation, or a unit
  // test's double) still plays, at full gain.
  const gain = typeof context.createGain === 'function' ? context.createGain() : null;
  if (gain) gain.connect(context.destination);
  const out = gain ?? context.destination;
  let volume = 1, sourceVolume = 255, sliders = { sound: 1, music: 1 };
  let levelsTxt = null, soundEnvironTxt = null, soundsTxt = null;
  const applyGain = () => { if (gain) gain.gain.value = retailMusicChannelGain(sourceVolume, sliders.music, sliders.sound); };
  let source = null, activeSoundId = -1, activeLevelId = -1, titlePreparation = null, requestSerial = 0;
  async function prepare(levelId) {
    if (levelsTxt === null) levelsTxt = new TextDecoder('latin1').decode(read('data\\global\\excel\\Levels.txt'));
    if (soundEnvironTxt === null) soundEnvironTxt = new TextDecoder('latin1').decode(read('data\\global\\excel\\SoundEnviron.txt'));
    if (soundsTxt === null) soundsTxt = new TextDecoder('latin1').decode(read('data\\global\\excel\\Sounds.txt'));
    const selection = resolveLevelMusic(levelId, levelsTxt, soundEnvironTxt, soundsTxt);
    if (!selection) return { levelId: Number(levelId), soundId: 0, path: null, volume: 0, buffer: null };
    sourceVolume = selection.volume; applyGain();
    let buffer = buffers.get(selection.soundId);
    if (!buffer) {
      const bytes = read(`data\\global\\music\\${selection.path}`);
      decodeWav(bytes);
      const copy = bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
      buffer = await context.decodeAudioData(copy); buffers.set(selection.soundId, buffer);
    }
    return { ...selection, buffer };
  }
  async function prepareTitle() {
    if (titlePreparation) return titlePreparation;
    const key = 'title';
    titlePreparation = (async () => {
      let buffer = buffers.get(key);
      if (!buffer) {
        const bytes = read(`data\\global\\music\\${TITLE_MUSIC}`);
        decodeWav(bytes);
        const copy = bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
        buffer = await context.decodeAudioData(copy); buffers.set(key, buffer);
      }
      return { soundId: key, levelId: null, name: 'title', path: TITLE_MUSIC, volume: MUSIC_VOLUME_MAX, buffer };
    })();
    try { return await titlePreparation; }
    finally { titlePreparation = null; }
  }
  async function playPrepared(preparedPromise) {
    const request = ++requestSerial;
    const resumed = context.resume();
    let prepared;
    try { [prepared] = await Promise.all([preparedPromise, resumed]); }
    catch (error) { if (request !== requestSerial) return null; throw error; }
    if (request !== requestSerial) return prepared;
    if (prepared.soundId === 0) {
      if (source) { source.stop(); source.disconnect(); source = null; }
      sourceVolume = 0; applyGain(); activeSoundId = 0; activeLevelId = prepared.levelId;
      return prepared;
    }
    sourceVolume = prepared.volume; applyGain();
    if (activeSoundId === prepared.soundId && source) { activeLevelId = prepared.levelId ?? -1; return prepared; }
    if (source) { source.stop(); source.disconnect(); }
    source = context.createBufferSource(); source.buffer = prepared.buffer; source.loop = true; source.connect(out); source.start();
    activeSoundId = prepared.soundId; activeLevelId = prepared.levelId ?? -1;
    return prepared;
  }
  return {
    play(levelId) { return playPrepared(prepare(levelId)); },
    playLevel(levelId) { return playPrepared(prepare(levelId)); },
    async playTitle() {
      const prepared = prepareTitle();
      const request = ++requestSerial;
      const resumed = context.resume();
      let track;
      try { [track] = await Promise.all([prepared, resumed]); }
      catch (error) { if (request !== requestSerial) return null; throw error; }
      if (request !== requestSerial) return track;
      sourceVolume = track.volume; applyGain();
      if (activeSoundId === track.soundId && source) { activeLevelId = -1; return track; }
      if (source) { source.stop(); source.disconnect(); }
      source = context.createBufferSource(); source.buffer = track.buffer; source.loop = true; source.connect(out); source.start();
      activeSoundId = track.soundId; activeLevelId = -1;
      return track;
    },
    prepare,
    /** The Music slider's value, 0..1 (F235). */
    setVolume(v, nextSliders) {
      volume = Math.max(0, Math.min(1, Number(v) || 0));
      if (nextSliders) sliders = { sound: nextSliders.sound, music: nextSliders.music };
      else sliders = { sound: 1, music: volume };
      applyGain();
    },
    /** The live gain node's own value -- what the loop is actually played through, not what was asked. */
    get volume() { return gain ? gain.gain.value : volume; },
    stop() { requestSerial++; if (source) { source.stop(); source.disconnect(); source = null; } activeSoundId = -1; activeLevelId = -1; },
    close() { this.stop(); return context.close?.(); },
    get evidence() { return { activeSoundId, activeLevelId, decodedSounds: buffers.size, sourceVolume, sourceGain: sourceVolume / MUSIC_VOLUME_MAX, channelGain: gain ? gain.gain.value : null, state: context.state }; },
  };
}

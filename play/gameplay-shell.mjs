import { COMMAND, CommandInput, DEFAULT_SESSION_FIXTURES, hudLines, normalizeSnapshot, parseSessionRooms, parseSessionSnapshot, parseSessionUnits, parseSessionWarps, selectPrimaryTarget, viewportToWorld } from './gameplay-model.mjs';
import { createMpqAssetReader } from './assets/mpq-assets.mjs';
import { createActTileAtlas, decodeActTileAssets } from './assets/retail-tile-view.mjs';
import { createDs1Canvas, loadActPresetComposition } from './assets/ds1-compositor.mjs';
import { createPlayerRenderer, playerClassInfo } from './assets/unit-renderer.mjs';
import { createMonsterRenderer, parseMonsterAnimations } from './assets/monster-renderer.mjs';
import { createObjectRenderer, parseObjectPresentations } from './assets/object-renderer.mjs';
import { createItemRenderer, parseItemPresentations } from './assets/item-renderer.mjs';
import { createControlPanelRenderer } from './assets/control-panel.mjs';
import { createInventoryPanelRenderer } from './assets/inventory-panel.mjs';
import { createBeltRenderer } from './assets/belt-renderer.mjs';
import { createActMusicPlayer } from './assets/act-music.mjs';

const canvas = document.querySelector('#game');
const hud = document.querySelector('#hud');
const status = document.querySelector('#session-status');
const actSelect = document.querySelector('#act-select');
const classSelect = document.querySelector('#class-select');
const ctx = canvas.getContext('2d', { alpha: false });
const input = new CommandInput();
let snapshot = normalizeSnapshot(null);
let last = performance.now(), accumulator = 0, frames = 0, fps = 0, fpsAt = last;
const STEP = 1000 / 60;
let session = null, bootstrapStarted = false, sessionError = '';
let fixtures = [];
const tileViews = new Map();
let tileView = null, tileViewLoading = 0, tileViewError = '';
let playerRenderer = null, playerRenderError = '', motion = {};
let monsterRenderer = null;
let objectRenderer = null;
let itemRenderer = null;
let inventoryPanelRenderer = null, inventoryPanelEvidence = null;
let beltRenderer = null, beltEvidence = null;
let controlPanelRenderer = null, controlPanelEvidence = null;
let actMusic = null, musicError = '';
let selectedSkill = 0, lastSkillCommand = null;
const PLAY_MODE = new URLSearchParams(location.search).get('play') === '1';
const TERRAIN_ENABLED = new URLSearchParams(location.search).get('terrain') !== '0';
let inventoryVisible = new URLSearchParams(location.search).get('inventory') === '1';

async function loadActTileView(act0) {
  const request = ++tileViewLoading;
  tileView = null; tileViewError = '';
  try {
    let view = tileViews.get(act0);
    if (!view) {
      await new Promise((resolve) => setTimeout(resolve, 0));
      const read = createMpqAssetReader(Module);
      const decoded = decodeActTileAssets(read, act0);
      view = { ...decoded, atlas: createActTileAtlas(decoded) };
      try {
        const composition = loadActPresetComposition(read, act0);
        view.composition = composition;
        view.mapCanvas = createDs1Canvas(composition);
      } catch (error) { view.compositionError = String(error); }
      tileViews.set(act0, view);
    }
    if (request === tileViewLoading) tileView = view;
  } catch (error) {
    if (request === tileViewLoading) { tileView = null; tileViewError = String(error); }
  }
}

function sessionAdapter() {
  if (window.Module?._d2_session_create && window.Module?._d2_session_snapshot) {
    return {
      create(f, playerClass = 0) { return Module.ccall('d2_session_create_class', 'number', ['number', 'number', 'number', 'number', 'number'], [f.seed, f.difficulty, f.act0, f.level, playerClass]); },
      move(x, y) { return Module.ccall('d2_session_command_move', 'number', ['number', 'number'], [x, y]); },
      attack(targetId) { return Module.ccall('d2_session_command_primary_attack', 'number', ['number'], [targetId]); },
      throwSkill(targetId) { return Module.ccall('d2_session_command_throw', 'number', ['number'], [targetId]); },
      kick(targetId) { return Module.ccall('d2_session_command_kick', 'number', ['number'], [targetId]); },
      pickup(itemId) { return Module.ccall('d2_session_command_pickup', 'number', ['number'], [itemId]); },
      drop(itemId) { return Module.ccall('d2_session_command_drop', 'number', ['number'], [itemId]); },
      useBelt(slot) { return Module.ccall('d2_session_command_use_belt', 'number', ['number'], [slot]); },
      operate(objectId) { return Module.ccall('d2_session_command_object', 'number', ['number'], [objectId]); },
      talk(npcId) { return Module.ccall('d2_session_command_npc', 'number', ['number'], [npcId]); },
      talkNearest() { return Module.ccall('d2_session_command_nearest_npc', 'number', ['number'], [6]); },
      interact() { return Module.ccall('d2_session_command_nearest_warp', 'number', ['number'], [6]); },
      respawn() { return Module.ccall('d2_session_command_respawn', 'number', [], []); },
      tick(n) { return Module.ccall('d2_session_tick', 'number', ['number'], [n]); },
      motion() {
        if (!Module._d2_session_motion) return {};
        const p = Number(Module.ccall('d2_session_motion', 'number', [], []));
        if (!p) return {};
        const view = new DataView(Module.HEAPU8.buffer, p, 52);
        if (view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 52) return {};
        return { framePrecise: view.getUint32(40, true), frameCountPrecise: view.getUint32(44, true),
          direction: view.getUint8(50), moving: view.getUint8(51) !== 0 };
      },
      snapshot() {
        const p = Number(Module.ccall('d2_session_snapshot', 'number', [], []));
        if (!p) throw new Error('d2_session_snapshot returned null');
        const base = parseSessionSnapshot(Module.HEAPU8.subarray(p, p + 68));
        if (Module._d2_session_resources) {
          const rp = Number(Module.ccall('d2_session_resources', 'number', [], [])), rv = new DataView(Module.HEAPU8.buffer, rp, 24);
          if (rv.getUint32(0, true) === 1 && rv.getUint32(4, true) === 24) {
            base.player.hp = rv.getInt32(8, true); base.player.hpMax = rv.getInt32(12, true);
            base.player.mana = rv.getInt32(16, true); base.player.manaMax = rv.getInt32(20, true);
          }
        }
        if (!Module._d2_session_rooms || !Module._d2_session_units) return base;
        const roomCount = Module.ccall('d2_session_room_count', 'number', [], []);
        const roomPtr = Number(Module.ccall('d2_session_rooms', 'number', [], []));
        const unitCount = Module.ccall('d2_session_unit_count', 'number', [], []);
        const unitPtr = Number(Module.ccall('d2_session_units', 'number', [], []));
        const entities = parseSessionUnits(Module.HEAPU8.subarray(unitPtr, unitPtr + unitCount * 44), unitCount);
        if (Module._d2_session_monster_animations) {
          const animationCount = Module.ccall('d2_session_monster_animation_count', 'number', [], []);
          const animationPtr = Number(Module.ccall('d2_session_monster_animations', 'number', [], []));
          const byId = new Map(parseMonsterAnimations(Module.HEAPU8.subarray(animationPtr, animationPtr + animationCount * 92), animationCount).map((a) => [a.id, a]));
          for (const entity of entities) if (entity.type === 1) entity.animation = byId.get(entity.id);
        }
        let objects = [];
        if (Module._d2_session_object_presentations) {
          const count = Module.ccall('d2_session_object_presentation_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_object_presentations', 'number', [], []));
          objects = parseObjectPresentations(Module.HEAPU8.subarray(ptr, ptr + count * 40), count);
        }
        let items = [];
        if (Module._d2_session_item_presentations) {
          const count = Module.ccall('d2_session_item_presentation_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_item_presentations', 'number', [], []));
          items = parseItemPresentations(Module.HEAPU8.subarray(ptr, ptr + count * 112), count);
        }
        let warps = [];
        if (Module._d2_session_warps) {
          const count = Module.ccall('d2_session_warp_count', 'number', [], []);
          const ptr = Number(Module.ccall('d2_session_warps', 'number', [], []));
          warps = parseSessionWarps(Module.HEAPU8.subarray(ptr, ptr + count * 68), count);
        }
        return normalizeSnapshot({ ...base,
          rooms: parseSessionRooms(Module.HEAPU8.subarray(roomPtr, roomPtr + roomCount * 36), roomCount),
          entities, objects, items, warps,
        });
      },
      destroy() { Module.ccall('d2_session_destroy', null, [], []); },
    };
  }
  return null;
}

async function bootstrapSession() {
  bootstrapStarted = true;
  try {
    fixtures = DEFAULT_SESSION_FIXTURES;
    if (!fixtures.length) throw new Error('monspawn vectors contain no playable fixtures');
    actSelect.replaceChildren(...fixtures.map((fixture, index) => Object.assign(document.createElement('option'), {
      value: String(index), textContent: fixture.label || `Act ${fixture.act0 + 1} · Level ${fixture.level}`,
    })));
    actSelect.disabled = false;
    classSelect.replaceChildren(...Array.from({ length: 7 }, (_, playerClass) => Object.assign(document.createElement('option'), {
      value: String(playerClass), textContent: playerClassInfo(playerClass).name,
    })));
    classSelect.disabled = false;
    const fixture = fixtures[0];
    const api = sessionAdapter();
    if (!api?.create(fixture, Number(classSelect.value))) throw new Error('d2_session_create failed');
    session = api; snapshot = api.snapshot();
    motion = api.motion();
    const assetReader = createMpqAssetReader(Module);
    playerRenderer = createPlayerRenderer(assetReader); playerRenderError = '';
    monsterRenderer = createMonsterRenderer(createMpqAssetReader(Module));
    objectRenderer = createObjectRenderer(createMpqAssetReader(Module));
    itemRenderer = createItemRenderer(createMpqAssetReader(Module));
    inventoryPanelRenderer = createInventoryPanelRenderer(createMpqAssetReader(Module));
    beltRenderer = createBeltRenderer(createMpqAssetReader(Module));
    controlPanelRenderer = createControlPanelRenderer(assetReader);
    try { actMusic = createActMusicPlayer(assetReader); await actMusic.prepare(fixture.act0); musicError = ''; } catch (error) { musicError = String(error); }

    if (TERRAIN_ENABLED) loadActTileView(fixture.act0);
    window.__d2.session = { fixture, snapshot: () => session.snapshot(), move: (x, y) => session.move(x, y), attack: (targetId) => session.attack(targetId), throwSkill: (targetId) => session.throwSkill(targetId), kick: (targetId) => session.kick(targetId), pickup: (itemId) => session.pickup(itemId), drop: (itemId) => session.drop(itemId), useBelt: (slot) => session.useBelt(slot), operate: (objectId) => session.operate(objectId), talk: (npcId) => session.talk(npcId), respawn: () => session.respawn(),
      interact: () => { const previousAct = snapshot.act, accepted = session.interact(); if (accepted) { snapshot = session.snapshot(); motion = session.motion(); if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadActTileView(snapshot.act - 1); } return accepted; },
      tick: (n = 1) => session.tick(n) };
  } catch (error) { sessionError = String(error?.stack || error); }
}

actSelect.addEventListener('change', () => {
  if (!session) return;
  const fixture = fixtures[Number(actSelect.value)];
  if (!fixture) return;
  session.destroy();
  if (!session.create(fixture, Number(classSelect.value))) { sessionError = `unable to create Act ${fixture.act0 + 1} session`; session = null; return; }
  snapshot = session.snapshot();
  motion = session.motion();
  actMusic?.prepare(fixture.act0).catch((error) => { musicError = String(error); });
  if (TERRAIN_ENABLED) loadActTileView(fixture.act0);
});

classSelect.addEventListener('change', () => {
  if (!session) return;
  const fixture = fixtures[Number(actSelect.value)];
  if (!fixture) return;
  session.destroy();
  if (!session.create(fixture, Number(classSelect.value))) { sessionError = 'unable to create character session'; session = null; return; }
  snapshot = session.snapshot(); motion = session.motion(); playerRenderError = '';
});

function update() {
  if (!bootstrapStarted && PLAY_MODE && /[?&]retail=1/.test(location.search) && window.__d2?.ready && !window.__d2.error && sessionAdapter()) bootstrapSession();
  const commands = input.drain();
  if (session) {
    for (const command of commands) {
      if (command.type === COMMAND.MOVE) {
        if (command.directional) session.move(snapshot.player.x + command.x, snapshot.player.y + command.y);
        else { const world = viewportToWorld(command.x, command.y, canvas.width, canvas.height, snapshot.player); session.move(world.x, world.y); }
      } else if (command.type === COMMAND.PRIMARY) {
        const point = command.x == null ? snapshot.player : viewportToWorld(command.x, command.y, canvas.width, canvas.height, snapshot.player);
        const ground = command.x == null ? null : snapshot.items.filter((item) => item.mode === 3 && item.invPage === 255)
          .map((item) => ({ item, d: (item.x - point.x) ** 2 + (item.y - point.y) ** 2 })).filter((entry) => entry.d <= 4)
          .sort((a, b) => a.d - b.d || a.item.id - b.item.id)[0]?.item;
        if (ground) { session.pickup(ground.id); continue; }
        const targetId = selectPrimaryTarget(snapshot, point, command.x == null ? Infinity : 2);
        const skill = command.secondary ? selectedSkill : 0;
        const result = targetId ? (skill === 2 ? session.throwSkill(targetId) : skill === 1 ? session.kick(targetId) : session.attack(targetId)) : 0;
        lastSkillCommand = { skill, targetId, result };
      } else if (command.type === COMMAND.SKILL) {
        const skills = [0, 2, 1]; if (command.slot >= 1 && command.slot <= skills.length) selectedSkill = skills[command.slot - 1];
      } else if (command.type === COMMAND.CANCEL) {
        selectedSkill = 0;
      } else if (command.type === COMMAND.INTERACT) {
        const previousAct = snapshot.act;
        let accepted = session.talkNearest();
        let nearest = null, distance = 7;
        for (const object of snapshot.objects) { const candidate = Math.max(Math.abs(object.x - snapshot.player.x), Math.abs(object.y - snapshot.player.y));
          if (candidate < distance || candidate === distance && (!nearest || object.id < nearest.id)) { nearest = object; distance = candidate; } }
        if (!accepted) accepted = nearest ? session.operate(nearest.id) : 0;
        if (!accepted) accepted = session.interact();
        if (accepted) {
          snapshot = session.snapshot(); motion = session.motion();
          if (TERRAIN_ENABLED && snapshot.act !== previousAct) loadActTileView(snapshot.act - 1);
        }
      } else if (command.type === COMMAND.RESPAWN) {
        if (snapshot.mode === 17 && session.respawn()) {
          snapshot = session.snapshot(); motion = session.motion();
        }
      }
    }
    try { session.tick(1); snapshot = session.snapshot(); motion = session.motion(); }
    catch (error) { sessionError = String(error); session = null; }
  }
  status.title = [tileViewError, musicError].filter(Boolean).join('\n');
  status.textContent = session
    ? `retail-backed simulation connected · ${tileView ? `retail tiles: Act ${tileView.act0 + 1}` : tileViewError ? 'retail tile fallback' : 'loading retail tiles…'} · ${musicError ? 'audio unavailable' : 'retail music ready'}`
    : sessionError || (PLAY_MODE ? 'engine loaded; creating simulation…' : 'open ?retail=1&play=1 to create a gameplay session');
}

function render() {
  const w = canvas.width, h = canvas.height;
  ctx.fillStyle = '#080a08'; ctx.fillRect(0, 0, w, h);
  if (tileView?.atlas) {
    if (tileView.mapCanvas) {
      const map = tileView.mapCanvas;
      const cameraX = w / 2 - map.width / 2 - (snapshot.player.x % 5) * 16;
      const cameraY = h / 2 - map.height / 2 - (snapshot.player.y % 5) * 8;
      const sourceX = Math.max(0, Math.floor(-cameraX));
      const sourceY = Math.max(0, Math.floor(-cameraY));
      const drawX = Math.max(0, Math.round(cameraX));
      const drawY = Math.max(0, Math.round(cameraY));
      const drawWidth = Math.min(w - drawX, map.width - sourceX);
      const drawHeight = Math.min(h - drawY, map.height - sourceY);
      ctx.save(); ctx.globalAlpha = .78;
      if (drawWidth > 0 && drawHeight > 0) {
        ctx.drawImage(map, sourceX, sourceY, drawWidth, drawHeight, drawX, drawY, drawWidth, drawHeight);
      }
      ctx.restore();
    } else {
      const pattern = tileView.pattern || (tileView.pattern = ctx.createPattern(tileView.atlas, 'repeat'));
      if (pattern) {
      ctx.save(); ctx.globalAlpha = .48;
      pattern.setTransform(new DOMMatrix().translate((w / 2 - snapshot.player.x * 16) % tileView.atlas.width, (h / 2 - snapshot.player.y * 16) % tileView.atlas.height));
      ctx.fillStyle = pattern; ctx.fillRect(0, 0, w, h); ctx.restore();
      }
    }
  }
  ctx.strokeStyle = '#182018'; ctx.lineWidth = 1;
  const grid = 32, ox = w / 2 - snapshot.player.x * grid, oy = h / 2 - snapshot.player.y * grid;
  for (let x = ((ox % grid) + grid) % grid; x < w; x += grid) { ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, h); ctx.stroke(); }
  for (let y = ((oy % grid) + grid) % grid; y < h; y += grid) { ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(w, y); ctx.stroke(); }
  for (const room of snapshot.rooms) {
    ctx.fillStyle = 'rgba(37,45,31,.28)'; ctx.strokeStyle = '#42503a';
    ctx.fillRect(ox + room.x * grid, oy + room.y * grid, room.width * grid, room.height * grid);
    ctx.strokeRect(ox + room.x * grid, oy + room.y * grid, room.width * grid, room.height * grid);
  }
  for (const warp of snapshot.warps) {
    if (!warp.enabled) continue;
    const x = ox + warp.x * grid, y = oy + warp.y * grid;
    ctx.save(); ctx.translate(x, y); ctx.rotate(Math.PI / 4);
    ctx.fillStyle = 'rgba(82,156,220,.34)'; ctx.strokeStyle = '#8bc6ff'; ctx.lineWidth = 2;
    ctx.fillRect(-9, -9, 18, 18); ctx.strokeRect(-9, -9, 18, 18); ctx.restore();
  }
  for (const entity of snapshot.entities) {
    const screenX = ox + entity.x * grid, screenY = oy + entity.y * grid;
    if (entity.hostile && entity.animation && monsterRenderer?.draw(ctx, screenX, screenY, entity.animation, snapshot.act)) continue;
    ctx.fillStyle = entity.hostile ? '#b33b2e' : '#958968';
    ctx.beginPath(); ctx.arc(screenX, screenY, 5, 0, Math.PI * 2); ctx.fill();
  }
  for (const object of snapshot.objects) objectRenderer?.draw(ctx, ox + object.x * grid, oy + object.y * grid, object, snapshot.act);
  for (const item of snapshot.items) if (item.mode === 3 && item.invPage === 255) itemRenderer?.draw(ctx, ox + item.x * grid, oy + item.y * grid, item);
  if (playerRenderer) {
    try { playerRenderer.draw(ctx, w / 2, h / 2, { ...motion, playerClass: snapshot.playerClass, mode: snapshot.mode, tick: snapshot.tick, act: snapshot.act }); playerRenderError = ''; }
    catch (error) { playerRenderError = String(error); playerRenderer = null; }
  }
  if (!playerRenderer) {
    ctx.fillStyle = '#d9b85c'; ctx.beginPath(); ctx.arc(w / 2, h / 2, 7, 0, Math.PI * 2); ctx.fill();
  }
  if (!session) {
    ctx.fillStyle = 'rgba(0,0,0,.68)'; ctx.fillRect(0, h / 2 - 44, w, 88);
    ctx.fillStyle = '#c9ad68'; ctx.textAlign = 'center'; ctx.font = '20px Georgia,serif';
    ctx.fillText('Gameplay simulation is not connected', w / 2, h / 2 - 5);
    ctx.fillStyle = '#91866d'; ctx.font = '13px monospace';
    ctx.fillText('The renderer and controls are ready for d2_session_*', w / 2, h / 2 + 22);
  }
  if (session && snapshot.mode === 17) {
    ctx.fillStyle = 'rgba(0,0,0,.68)'; ctx.fillRect(0, h / 2 - 48, w, 96);
    ctx.fillStyle = '#a5160f'; ctx.textAlign = 'center'; ctx.font = '28px Georgia,serif';
    ctx.fillText('YOU HAVE DIED', w / 2, h / 2 - 7);
    ctx.fillStyle = '#c9ad68'; ctx.font = '14px monospace';
    ctx.fillText('Press R to return to town', w / 2, h / 2 + 24);
  }
  if (inventoryPanelRenderer) inventoryPanelEvidence = inventoryPanelRenderer.draw(ctx, w, h, snapshot, inventoryVisible);
  if (controlPanelRenderer) controlPanelEvidence = controlPanelRenderer.draw(ctx, w, h, snapshot);
  if (beltRenderer) beltEvidence = beltRenderer.draw(ctx, snapshot.items);
  hud.textContent = hudLines(snapshot, Boolean(session), fps).join('\n');
}

function frame(now) {
  accumulator = Math.min(accumulator + now - last, STEP * 5); last = now;
  while (accumulator >= STEP) { update(); accumulator -= STEP; }
  render();
  frames++;
  if (now - fpsAt >= 500) { fps = frames * 1000 / (now - fpsAt); frames = 0; fpsAt = now; }
  requestAnimationFrame(frame);
}

function canvasPoint(event) {
  const r = canvas.getBoundingClientRect();
  return [(event.clientX - r.left) * canvas.width / r.width, (event.clientY - r.top) * canvas.height / r.height];
}
addEventListener('keydown', (e) => { actMusic?.play(snapshot.act - 1).catch((error) => { musicError = String(error); }); if (e.code === 'KeyI' && !e.repeat) { inventoryVisible = !inventoryVisible; e.preventDefault(); return; }
  if (input.key(e.code, true, e.repeat)) e.preventDefault(); });
addEventListener('keyup', (e) => { if (input.key(e.code, false)) e.preventDefault(); });
addEventListener('blur', () => { input.keys.clear(); });
canvas.addEventListener('pointerdown', (e) => { const [x, y] = canvasPoint(e);
  actMusic?.play(snapshot.act - 1).catch((error) => { musicError = String(error); });
  const beltItem = beltRenderer?.itemAt(x, y, snapshot.items); if (beltItem && session?.useBelt(beltItem.gridX)) { snapshot = session.snapshot(); canvas.focus(); e.preventDefault(); return; }
  if (!(inventoryVisible && inventoryPanelRenderer?.itemAt(x, y, canvas.width, canvas.height, snapshot))) input.pointer(x, y, e.button);
  canvas.focus(); e.preventDefault(); });
canvas.addEventListener('contextmenu', (e) => { if (session && inventoryVisible && inventoryPanelRenderer) {
    const [x, y] = canvasPoint(e), item = inventoryPanelRenderer.itemAt(x, y, canvas.width, canvas.height, snapshot);
    if (item && session.drop(item.id)) snapshot = session.snapshot();
  } e.preventDefault(); });
window.__d2Shell = { input, get snapshot() { return snapshot; }, get tileView() { return tileView; }, get tileViewError() { return tileViewError; },
  step() { update(); },
  get playerRenderer() { return playerRenderer; }, get monsterRenderer() { return monsterRenderer; },
  get objectRenderer() { return objectRenderer; },
  get itemRenderer() { return itemRenderer; },
  get inventoryPanelEvidence() { return inventoryPanelEvidence && { ...inventoryPanelEvidence }; },
  get beltEvidence() { return beltEvidence && { ...beltEvidence }; },
  get controlPanelEvidence() { return controlPanelEvidence && { ...controlPanelEvidence }; },
  get selectedSkill() { return selectedSkill; }, get lastSkillCommand() { return lastSkillCommand && { ...lastSkillCommand }; },
  get playerRenderError() { return playerRenderError; }, get sessionError() { return sessionError; }, normalizeSnapshot };
Object.defineProperty(window.__d2Shell, 'music', { get: () => actMusic?.evidence || null });
addEventListener('beforeunload', () => { session?.destroy(); actMusic?.close(); });
requestAnimationFrame(frame);

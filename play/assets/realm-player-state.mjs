// Keep D2GS state ingestion and the local-vs-realm hero draw choice shared by the live shell and
// the local protocol scenario. The engine owns the record; this module only selects what to draw.
export function applyRealmPlayerFrame(protocol, bytes) {
  if (!protocol?.d2gsClientPlayerFeed) throw new Error('D2GS player state adapter is unavailable');
  const applied = protocol.d2gsClientPlayerFeed(bytes);
  if (applied < 0) throw new Error(`D2GS player state stream rejected server data (${applied})`);
  const state = protocol.d2gsClientPlayerState?.() ?? null;
  return { applied, state: state?.active ? state : null, players: protocol.d2gsClientPlayers?.() ?? [],
    monsters: protocol.d2gsClientMonsters?.() ?? [] };
}

export function selectPlayerRenderSource(snapshot, motion, realmSessionOpen, realmPlayer, realmAct = null) {
  if (realmSessionOpen && realmPlayer?.active && realmPlayer.clientRoomActive && (realmPlayer.x !== 0 || realmPlayer.y !== 0)) {
    // Draw the client CUnit's current animation, not the fixed assignment record: a server mode
    // change (walk, gethit, death) is written to that unit and must reach the renderer.
    const live = realmPlayer.motion?.active ? realmPlayer.motion : null;
    return {
      source: 'realm', guid: realmPlayer.guid, x: realmPlayer.x, y: realmPlayer.y,
      classId: realmPlayer.classId, reassignments: realmPlayer.reassignments,
      renderState: { playerClass: realmPlayer.classId, mode: live?.mode ?? realmPlayer.initialMode ?? 5,
        direction: live?.direction ?? realmPlayer.initialDirection ?? 0, framePrecise: live?.framePrecise,
        frameCountPrecise: live?.frameCountPrecise, animSpeed: live?.animSpeed, seqMode: live?.seqMode, seqFrame: live?.seqFrame,
        tick: snapshot?.tick ?? 0, act: realmAct ?? snapshot?.act ?? 0,
        weaponClass: 'hth', items: [] },
    };
  }
  if (realmSessionOpen) return { source: 'none', x: 0, y: 0, classId: null, renderState: null };
  if (!snapshot?.player) return { source: 'none', x: 0, y: 0, classId: null, renderState: null };
  return {
    source: 'local', x: snapshot.player.x, y: snapshot.player.y, classId: snapshot.playerClass,
    renderState: { ...motion, playerClass: snapshot.playerClass,
      mode: Number.isInteger(motion?.presentationMode) ? motion.presentationMode : snapshot.mode, tick: snapshot.tick,
      act: snapshot.act, weaponClass: snapshot.player.weaponClass,
      renderIdentity: snapshot.player.renderIdentity ? { ...snapshot.player.renderIdentity } : null,
      primaryWeaponId: snapshot.player.primaryWeaponId, items: snapshot.items },
  };
}

// Native current-player selection defines the active client level. Do not draw a roster unit from
// another still-active level over the selected level's tiles when the client keeps multiple acts
// or rooms resident during a transition.
export function visibleRealmPlayers(players, selectedPlayer, renderLevel) {
  if (!selectedPlayer?.active || !selectedPlayer.clientRoomActive) return [];
  const level = selectedPlayer.clientRoomLevel;
  if (!level || Number(renderLevel) !== level) return [];
  return (players || []).filter((player) => player?.active && player.clientRoomActive &&
    (player.x !== 0 || player.y !== 0) && player.clientRoomLevel === level);
}

// A realm ground click is a request to the server. The local simulation must not advance or
// manufacture a client position here; only a later server 0x15 updates the player record.
export function realmDirectionalAim(player, x, y) {
  if (!player?.active || !Number.isInteger(player.x) || !Number.isInteger(player.y) ||
      !Number.isFinite(x) || !Number.isFinite(y)) return null;
  return { x: player.x + Math.sign(x), y: player.y + Math.sign(y) };
}

export function sendRealmGroundWalk(protocol, transport, sessionOpen, x, y, duplicateState = null, now = 0) {
  return sendRealmCoordinateCommand(protocol?.d2gsMoveTo && (() => protocol.d2gsMoveTo(1,x,y)),
    transport, sessionOpen, x, y, 1, 200, duplicateState, now);
}

export function sendRealmRightSkillAt(protocol, transport, sessionOpen, x, y, held = false, duplicateState = null, now = 0, predictLocal = null) {
  return sendRealmCoordinateCommand(protocol?.d2gsRightSkillAt && (() => protocol.d2gsRightSkillAt(held,x,y)),
    transport, sessionOpen, x, y, held ? 0x0f : 0x0c, 50, duplicateState, now, predictLocal);
}

// A right skill aimed at a server-owned monster carries its unit identity, not projected
// coordinates. Local prediction changes only the canonical client action; acceptance and damage
// remain server-authoritative.
export function sendRealmRightSkillOnMonster(protocol, transport, sessionOpen, guid, held = false,
    duplicateState = null, now = 0, predictLocal = null) {
  if (!sessionOpen || !transport?.isOpen || typeof transport.send !== 'function')
    return { sent: false, reason: 'disconnected', guid };
  if (!Number.isInteger(guid) || guid < 0 || guid > 0xffffffff)
    return { sent: false, reason: 'invalid-target', guid };
  if (typeof protocol?.d2gsRightSkillOnEntity !== 'function')
    return { sent: false, reason: 'composer-unavailable', guid };
  const opcode = held ? 0x10 : 0x0d;
  let predicted = false;
  if (typeof predictLocal === 'function') {
    const result = predictLocal(guid);
    if (result !== 1) return { sent: false,
      reason: result === 0 ? 'prediction-refused' : 'prediction-unsupported',
      guid, opcode, prediction: result };
    predicted = true;
  }
  const packet = protocol.d2gsRightSkillOnEntity(held, 1, guid);
  const time = Number(now) >>> 0;
  if (duplicateState?.packet && ((time - duplicateState.time) >>> 0) < 50 &&
      packet.length === duplicateState.packet.length && packet.every((byte, i) => byte === duplicateState.packet[i]))
    return { sent: false, reason: 'duplicate', guid, opcode, packet: Array.from(packet), predicted };
  if (duplicateState) { duplicateState.packet = Array.from(packet); duplicateState.time = time; }
  if (transport.send(packet) === false) return { sent: false, reason: 'disconnected', guid, predicted };
  return { sent: true, reason: '', guid, opcode, packet: Array.from(packet), predicted };
}

// The loaded native Skills/Missiles tables decide whether the selected right-hand
// skill uses the supported ordinary SC projectile path. Do not keep a JS skill list.
export function realmClientProjectilePredictor(protocol) {
  const selected = protocol?.d2gsSelectedSkills?.();
  if (!Number.isInteger(selected?.right) || selected.right < 0 ||
      typeof protocol?.d2gsClientProjectileSupported !== 'function' ||
      protocol.d2gsClientProjectileSupported(selected.right) !== 1 ||
      typeof protocol?.d2gsClientPredictRightCast !== 'function') return null;
  return (x, y) => protocol.d2gsClientPredictRightCast(x, y);
}

// Resolve the server GUID in native client CUnits; never substitute the rendered coordinates.
export function realmFireballMonsterPredictor(protocol) {
  const selected = protocol?.d2gsSelectedSkills?.();
  if ((selected?.right !== 36 && selected?.right !== 47) ||
      typeof protocol?.d2gsClientPredictRightCastOnMonster !== 'function') return null;
  return (guid) => protocol.d2gsClientPredictRightCastOnMonster(guid);
}

function sendRealmCoordinateCommand(compose, transport, sessionOpen, x, y, opcode, window, duplicateState, now, predictLocal = null) {
  if (!sessionOpen || !transport?.isOpen || typeof transport.send !== 'function')
    return { sent: false, reason: 'disconnected', x, y };
  if (!Number.isInteger(x) || !Number.isInteger(y) || x < 0 || y < 0 || x > 0xffff || y > 0xffff)
    return { sent: false, reason: 'out-of-range', x, y };
  if (typeof compose !== 'function') return { sent: false, reason: 'composer-unavailable', x, y };
  const time = Number(now) >>> 0;
  // Retail applies the local client action before its sender checks the shared duplicate
  // packet cache. For the supported selected Fireball path, a refused native prediction
  // rejects this action and therefore must not emit a request.
  let predicted = false;
  if (typeof predictLocal === 'function') {
    const result = predictLocal(x, y);
    if (result !== 1) return { sent: false,
      reason: result === 0 ? 'prediction-refused' : 'prediction-unsupported',
      x, y, opcode, prediction: result };
    predicted = true;
  }
  const packet = compose();
  // Native 6FAC43E0 uses a shared last packet cache, with an opcode-specific window.
  if (duplicateState?.packet && ((time - duplicateState.time) >>> 0) < window &&
      packet.length === duplicateState.packet.length && packet.every((byte, i) => byte === duplicateState.packet[i]))
    return { sent: false, reason: 'duplicate', x, y, opcode, packet: Array.from(packet), predicted };
  if (duplicateState) { duplicateState.packet = Array.from(packet); duplicateState.time = time; }
  if (transport.send(packet) === false) return { sent: false, reason: 'disconnected', x, y, predicted };
  return { sent: true, reason: '', x, y, opcode, packet: Array.from(packet), predicted };
}

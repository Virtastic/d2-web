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

// The server's chunks arrive many to a game tick, and each used to re-read every player and monster record
// from the engine although the 25 Hz unit tick re-reads the same records before anything is drawn. Every
// chunk is still fed to the engine at once and in order; the records are read back at most once per
// `maxAge` ms while the scene is live (`live`), and a chunk inside that window hands back the views the
// caller already holds (`current`). Before the scene is live every chunk is read, as the join needs.
export const REALM_READ_MAX_AGE_MS = 1000 / 25;
export function createRealmFrameFeed({ now = () => performance.now(), maxAge = REALM_READ_MAX_AGE_MS } = {}) {
  let readAt = -Infinity;
  const stats = { chunks: 0, reads: 0 };
  return {
    stats,
    reset() { readAt = -Infinity; },
    feed(protocol, bytes, { live = false, current = null } = {}) {
      stats.chunks++;
      const at = now();
      if (live && current && at - readAt < maxAge) {
        if (!protocol?.d2gsClientPlayerFeed) throw new Error('D2GS player state adapter is unavailable');
        const applied = protocol.d2gsClientPlayerFeed(bytes);
        if (applied < 0) throw new Error(`D2GS player state stream rejected server data (${applied})`);
        return { applied, read: false, state: current.state ?? null, players: current.players ?? [], monsters: current.monsters ?? [] };
      }
      const update = applyRealmPlayerFrame(protocol, bytes);
      readAt = at; stats.reads++;
      return { ...update, read: true };
    },
  };
}

// The camera follows the selected player's path position at 16.16 precision, as a local game's does; a
// whole-subtile centre steps the world a subtile at a time under a walking hero. At rest both are x.5.
export function realmCameraCentre(source) {
  if (source?.preciseX != null && source?.preciseY != null) return { x: source.preciseX, y: source.preciseY };
  return { x: (source?.x ?? 0) + 0.5, y: (source?.y ?? 0) + 0.5 };
}

export function selectPlayerRenderSource(snapshot, motion, realmSessionOpen, realmPlayer, realmAct = null, worn = null) {
  if (realmSessionOpen && realmPlayer?.active && realmPlayer.clientRoomActive && (realmPlayer.x !== 0 || realmPlayer.y !== 0)) {
    // Draw the client CUnit's current animation, not the fixed assignment record: a server mode
    // change (walk, gethit, death) is written to that unit and must reach the renderer.
    const live = realmPlayer.motion?.active ? realmPlayer.motion : null;
    return {
      source: 'realm', guid: realmPlayer.guid, x: realmPlayer.x, y: realmPlayer.y,
      preciseX: realmPlayer.preciseX ?? null, preciseY: realmPlayer.preciseY ?? null,
      classId: realmPlayer.classId, reassignments: realmPlayer.reassignments,
      renderState: { playerClass: realmPlayer.classId, mode: live?.mode ?? realmPlayer.initialMode ?? 5,
        direction: live?.direction ?? realmPlayer.initialDirection ?? 0, framePrecise: live?.framePrecise,
        frameCountPrecise: live?.frameCountPrecise, animSpeed: live?.animSpeed, seqMode: live?.seqMode, seqFrame: live?.seqFrame,
        tick: snapshot?.tick ?? 0, act: realmAct ?? snapshot?.act ?? 0,
        weaponClass: worn?.weaponClass ?? 'hth', items: worn?.items ?? [], form: realmPlayer.form ?? null },
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
      form: snapshot.player.form ?? null,
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

// The players to draw: the level-matched roster, and always the selected player when the camera
// follows it. The camera is placed on the selected player whenever it is active with a position, so
// a roster whose level field disagrees with the world's must not leave the camera on an empty spot.
export function realmScenePlayers(roster, selectedPlayer, renderLevel) {
  const players = visibleRealmPlayers(roster, selectedPlayer, renderLevel);
  const drawable = selectedPlayer?.active && selectedPlayer.clientRoomActive && (selectedPlayer.x !== 0 || selectedPlayer.y !== 0);
  if (drawable && !players.some((player) => player.guid === selectedPlayer.guid)) players.push(selectedPlayer);
  return players;
}

// Why the selected player is not drawn, for the scene evidence ('' when it is).
export function realmPlayerGate(player) {
  if (!player) return 'no-player-state';
  if (!player.active) return 'inactive';
  if (!player.clientRoomActive) return 'client-room-inactive';
  if (player.x === 0 && player.y === 0) return 'no-position';
  return '';
}

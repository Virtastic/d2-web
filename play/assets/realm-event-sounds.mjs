// A game read from packets has no engine session, so the client-side event sounds (item pickup, gold, drop, the hero's footfalls,
// object mode sounds) are fed from the client state the packets built: the snapshot the shell draws, the cursor item and the gold.
// Retail submits these from the client's own frame (D2Client+0x23C00 footstep, +0x88A70 item drop, +0x237E0 object and monster sound);
// this only dispatches the state to the event player, it decides nothing.
export function createRealmEventSoundFeed(eventSounds) {
  let gold = null;
  return {
    reset() { gold = null; eventSounds?.resetObjects?.(); },
    /** One client tick. `world` is the realm world state ({ cursorId, gold }); `level` the hero's level from the packets. */
    tick(snapshot, level, world) {
      if (!eventSounds || !snapshot?.player) return;
      const now = world?.gold ?? 0, goldGained = gold !== null && now > gold;
      gold = now;
      // The hero's own unit mode drives the footfall; a click-move is explicit (the cast-started run keeps the run group: open).
      const player = { ...snapshot.player, mode: snapshot.mode, classId: snapshot.player.classId ?? snapshot.playerClass, explicitMove: true };
      eventSounds.update({ ...snapshot, player }, level, { cursorId: world?.cursorId ?? 0, goldGained });
    },
  };
}

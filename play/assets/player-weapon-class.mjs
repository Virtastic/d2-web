// D2Common's D2WeaponClasses enum, in the order returned by
// COMPOSIT_GetWeaponClassId. Keep the numeric-to-COF-name bridge here; the
// class decision itself is owned by the engine.
const WEAPON_CLASS = Object.freeze([
  'hth', 'bow', '1hs', '1ht', 'stf', '2hs', '2ht', 'xbw',
  '1js', '1jt', '1ss', '1st', 'ht1', 'ht2',
]);

export function playerWeaponClassToken(id) {
  const token = WEAPON_CLASS[Number(id)];
  if (!token) throw new RangeError(`unknown engine weapon class id ${id}`);
  return token;
}

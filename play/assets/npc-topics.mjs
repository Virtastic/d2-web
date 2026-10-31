// Which quest a message string id belongs to: D2MOO's StringIds.h names them A<act>Q<n><Init|AfterInit|EarlyReturn|Successful><NPC>
// (src/core/d2moo/include/DataTbls/StringIds.h). Generated -- [first id, last id, act, quest].
// A menu-2 message of the engine's 0x27 list is that quest's Talk topic; its label is the quest's own name string 'qsts a<act> q<n>'.
export const QUEST_MESSAGE_RANGES = [[64,80,1,1],[81,96,1,2],[97,126,1,4],[127,145,1,5],[146,165,1,3],[166,184,1,6],[304,334,2,1],[335,339,2,2],[340,372,2,3],[373,407,2,4],[408,429,2,5],[430,452,2,6],[527,542,3,4],[543,548,3,2],[549,570,3,1],[571,593,3,3],[594,627,3,5],[628,663,3,6],[670,677,4,1],[678,680,4,3],[681,685,4,2],[20000,20001,4,2],[20077,20095,5,1],[20096,20115,5,2],[20116,20136,5,3],[20137,20152,5,4],[20153,20168,5,5],[20169,20180,5,6]];
export const questOfMessage = (id) => { const r = QUEST_MESSAGE_RANGES.find(([a, b]) => id >= a && id <= b); return r ? { act: r[2], quest: r[3] } : null; };
// P7b: an NPC's Introduction and Gossip lines (StringIds.h: <NPC>IntroGossip1 [+ the class variant], <NPC>Gossip5: the line retail's captures speak). Retail speaks ONE
// fixed gossip line per NPC (talk-*-r22: the same on 3-4 clicks). Measured for Kashya and Akara only; keyed by MonStats class id.
export const NPC_SPEECH = { 150: { intro: 24, gossip: 30 }, 148: { intro: 11, gossip: 17 } };
export const npcSpeech = (classId) => NPC_SPEECH[classId] || null;
// F21: native D2Client speech timing is implemented in speech-box.mjs from the numeric D2Lang
// header and FUN_6FB2BB40's fixed-point tick formula; vectors still supply the captured text identity.
// Introduction / Gossip lines for the NPCs whose classes retail captured nothing for, keyed by the NPC's English display name (lowercase)
// and, where one name serves several acts, the act. Ids are D2MOO StringIds.h's own <NPC>IntroGossip1 and <NPC>Gossip5: the two NPCs measured
// (Kashya, Akara) both speak their fifth Gossip line, so the rest follow it (the last line where an NPC has fewer). UNMEASURED for these NPCs:
// a retail capture of their Gossip click would settle which line it is. The engine's own first-click message still wins for Introduction.
const A = (intro, gossip) => ({ intro, gossip });
const CAIN = { 1: A(null, 58), 2: A(null, 301), 3: A(458, 463), 4: A(666, 667), 5: A(20003, 20008) };
export const NPC_SPEECH_BY_NAME = Object.freeze({
  warriv: { 1: A(0, 6), 2: A(215, 220) }, akara: A(11, 17), kashya: A(24, 30), charsi: A(36, 42), gheed: A(45, 51),
  cain: CAIN, 'deckard cain': CAIN,
  greiz: A(190, 195), elzix: A(203, 209), atma: A(221, 226), geglash: A(230, 236), jerhyn: A(255, 260), fara: A(263, 269),
  lysander: A(274, 279), drognan: A(285, 291), meshif: { 2: A(241, 247), 3: A(478, 484) }, natalya: A(453, 457),
  hratli: A(465, 471), asheara: A(490, 496), alkor: A(501, 507), ormus: A(514, 520), tyrael: { 2: A(null, 302), 4: A(664, 665) },
  anya: A(20014, 20019), larzuk: A(20025, 20031), malah: A(20037, 20044), nihlathak: A(20053, 20060), 'nihlathak town': A(20053, 20060), 'qual-kehk': A(20065, 20072),
});
export function npcSpeechByName(name, act) {
  const row = NPC_SPEECH_BY_NAME[String(name || '').trim().toLowerCase()];
  if (!row) return null;
  return 'gossip' in row ? row : row[act] || null;
}

// The string id a Talk-box row speaks: a topic its own message; Introduction the engine's plain message, else the one heard on
// the first click, else the NPC's table intro; Gossip the table line. null = this NPC has no such line (the caller ends the talk).
export const talkSpeechId = (action, { stringId, plain, said, sp }) =>
  (action === 'topic' ? stringId : action === 'intro' ? (plain ?? said ?? sp?.intro) : sp?.gossip) ?? null;

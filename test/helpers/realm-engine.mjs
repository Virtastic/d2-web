// Binds the staged engine into play/assets/realm-input.mjs: its packet gate (D2Client 0x143E0), 9-byte builder (0x14910) and
// belt-use gate (0x29920) are the engine's own. Import it first in a test that makes a realm input or a gated packet.
// Without a staged engine nothing is bound and the first use throws (the test fails by name, it does not pass on a stand-in).
import { haveEngine, loadEngine } from './node-engine.mjs';
import { bindRealmEngine, engineSeam } from '../../play/assets/realm-input.mjs';

export const seam = haveEngine() ? engineSeam(await loadEngine()) : null;
if (seam) bindRealmEngine(seam);

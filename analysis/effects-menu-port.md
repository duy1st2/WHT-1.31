# Adaptive-particle menu completion

Reference: actual WHT Graphics control at row420, its English label and Vietnamese/English threshold tooltip. Source hashes in effects-menu-evidence.json. WHT-1.29 lacks the adaptive-particle runtime, so the permitted WHT reference is used.

The 1.31 native particle implementation is already independently profiled and tested for both builds and architectures. This change exposes that existing feature in F8/Graphics. It introduces no new engine offset, hook, frame constructor or frame-level change. The checkbox uses the standard menu control builder and its existing tooltip/animation behavior. Coordinates match WHT; default stays OFF as documented for 1.31.

A shared EffectsSetting131/EffectsSetting64 handler is now used by the chat command and the menu's #toolui path. It retains native-profile validation before enabling, checked INI save/reload, error notification, and immediate restoration of the owned emission setting on OFF. Changing a preference while Command Settings are off does not enable native writes; the existing authorized Local-mode runtime gate remains in force. Canonical Config.autoEffects supplies checkbox state. Existing control indices are preserved by appending the new row to the catalog rather than inserting into its source order.

Static checks confirm241 rows and unchanged first240 rows, reference placement, OFF fallback and exactly one shared helper per architecture used by both entry points. Existing full build/test suites cover the menu and native-particle controller contracts. No new tests mirror this reversible UI addition. Actual F8 appearance and mouse interaction have not been visually tested in a live game.

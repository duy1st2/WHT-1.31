# Requested remaining three features

User explicitly selected all three on2026-10-28: #sabi, #shareall, #save. Actual reference implementations read and hashes recorded; no unverified native feature enabled.

## #sabi
WHT handler adds absent ability then sets/reads native level; active1.29 add_ability129.hpp wraps pre-add notification2e0d20, creator6f3ba0, post-add2dd220, panel refresh, maximum getter6f5c0, virtual increase/decrease2e4/2e8 and native stored level+50. Creation is a large type dispatcher with ability-data lookup, constructor and owning-unit attach. All callbacks must revalidate original unit and ability registry identity; do not reuse old offsets. Existing1.31 Avul lifecycle has its own special factory sequence (bb1e50 calls metadata3217e0, factorye302f0 and CUnit attachbad840); this is not proof of a generic rawcode factory or ability level method. An identical short prologue search found unrelated functions946160/912ee0 and CRT; none accepted.

## #shareall
Active1.29 ShareControl129 visits active players other than local and sets alliance type6 only when different, with native readback. WHT reference supports the same switch. Need independently verified1.31 alliance query/set, identity and game synchronization behavior. Do not substitute a raw alliance-mask write.

## #save
Active1.29 save_game129.hpp queues native SaveGame using managed-string ABI object+8 -> storage+1c and confirms pending world+6d4; this layout is unverified for1.31. Parser defaults wht-save, allows64 ASCII letters/digits/_/-. Native save output must resolve beneath this project runtime-test before any invocation, per AGENTS.md. Do not fabricate string objects or assume an external Documents save path.

Current blocker is missing live1.31 match for native binding/configuration inspection; no WarcraftIII game process was found during inspection. Existing source/build/client artifacts untouched by this audit.

Update2026-10-28: active x86 clients8740(12164),32116(12173) inspected read-only, main images and selected native-name heap contexts saved. #sabi native creation/level mappings verified and implemented; see add-ability-port.md. #shareall binding SetPlayerAlliance91b640/8e8cc0 is protected; GetPlayerAlliance8dec60/8ad690 confirms typed alliance property+38+type*10 on12164x86 and native managed integer getter. Further verification needed for setter lifecycle/synchronization and x64 bindings. #save binding8f4ca0/8c2cf0 recovered; current calling sequence consumes two stack words and invokes native string getter on second word, unlike old1.29 one-word ABI. Do not call with fabricated1.29 object or claim saves stay in runtime-test until output resolver verified. Requested an additional x64 match for binding verification; existing x86 clients left running.

Implementation follow-up: #shareall now uses independently mapped native CAlliance enable/disable lifecycle functions for type6 on all four profiles; see share-control-port.md. #save now bypasses the JASS-context wrapper through the verified native SaveGame serialization worker, supplying a bounded absolute runtime-test/WHT-Saves destination and confirming nonempty file output; see save-game-port.md. Both built/tested/installed with #sabi retained. Live sharing and save/reload compatibility remain untested. Earlier unresolved-binding notes describe the investigation stage and no longer describe current implementation status.

# Map Size automatic launch default

Explicit user request: start Map Size ON each time the tool opens, for1.31 and1.29. WHT1.29 native/common.cpp already implements sessionUnlockMap initializedtrue, LoadConfig reads that session state rather than the persisted preference, and successful SaveSetting updates the current process state. Its runtime feature implementation is retained unchanged. The same narrow policy is added to1.31 common.cpp.

Legacy INI OFF does not prevent launch ON. #mapsize0 still disables during the current process; periodic reloads preserve that decision. New process launches resetON. Failed SaveSetting does not change session state. No INI migration or removal of unrelated preferences, native-profile validation or authorization gates. Existing patches/hooks/lifecycle logic are unchanged.1.31 retains its Command Settings gate;1.29 retains its original ToolAuthorized129 gate.

A subprocess regression test in each independent project creates an owned runtime-test INI with OFF, validates initialON, savesOFF, reloads and confirmsOFF, checks failed-save/case-insensitive updates, then launches a hidden independent test process against the same OFF INI to confirm startupON. Parent remainsOFF. Only the owned fixture is removed afterwards; user INI/log/macro snapshots are hash-verified around deployment.

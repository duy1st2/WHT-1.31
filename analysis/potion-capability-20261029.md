# Automatic potion discovery (2026-10-29)

Scope: WHT 1.24/1.26, independent WHT-1.29, and WHT-1.31 x86/x64.

Previous selection required ItemTypes rawcodes. AutoPotionSample now carries per-slot detected and unavailable masks. A detected healing/mana native implementation is eligible even with an empty configured list. Native class identity takes priority over a conflicting configured HP/Mana category. Unavailable slots are skipped before selecting an item. Existing explicit registrations remain compatible for scripted/ambiguous effects.

Detection reads the selected inventory button's bound ability and its in-image MSVC RTTI. Recognized implementations: CAbilityItemHeal / ItemHealAoe (HP), ItemManaRestore / ItemManaRestoreAoe (mana), ItemRestore / ItemRestoreAoe (both). Map-created item and ability rawcodes do not participate in this classification. No trial clicks, item creation, simulated resource writes, trigger execution, or worker reads of engine objects are used.

Native binding sources: WHT cooldown_hotkey_overlay.hpp uses button+190 / data+6D4; WHT-1.29 cooldown129.hpp uses button+194 / data+6D4. WHT-1.31 cooldown131.hpp and gameplay64.hpp already use the per-profile bound-ability getter and RTTI checks; profile64.hpp verifies the x64 getter for data+2D78. AutoHeal131 retains native inventory usability and source revalidation. These are independent implementations and builds, not an inherited 1.29 runtime enabled on 1.31.

Reads remain on the existing 200 ms low-resource sampling path. Existing 1500 ms attempt backoff and HP/Mana mutual scheduling remain. Recognized potions use the existing native UI/self-cast command path, not direct resource mutation. Native cooldown checks skip cooling candidates; native activation still decides charges, targeting and other map restrictions.

Limitations: generic Regeneration may restore HP, mana or both depending on object data; it is not guessed. Arbitrary map triggers and Channel abilities do not expose their recovery semantics through RTTI. Those items still require explicit registration. A native restore implementation can also have zero recovery values in custom object data; class detection alone does not prove a positive configured amount. Live map behavior and multiplayer synchronization must not be claimed verified from contract tests alone.

Tests cover arbitrary item codes without registration, unavailable first-slot fallback, unknown item rejection, category classification, and valid/invalid RTTI on both pointer widths, in addition to existing health thresholds, identity and scheduling contracts.

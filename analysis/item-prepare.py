from pathlib import Path
r=Path('WHT-1.31/analysis');s=(r/'fog-modifier-live-bindings.py').read_text();start=s.index('names=[');end=s.index('\nwhile a<',start);s=s[:start]+"names=[b'CreateItem\\0',b'GetItemTypeId\\0',b'RemoveItem\\0']"+s[end:];s=s.replace('fog-modifier-live-','item-live-');(r/'item-live-bindings.py').write_text(s)
s=(r/'fog-modifier-extract-bindings.py').read_text().replace('fog-modifier-live-','item-live-').replace('fog-modifier-resolved-','item-resolved-');(r/'item-extract-bindings.py').write_text(s)

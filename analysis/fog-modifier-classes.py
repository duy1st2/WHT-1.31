from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);print('CLASSES',pid)
 for m in re.finditer(rb'\.\?AV[^\x00]{0,90}Fog[^\x00]{0,90}\x00',b):print(hex(m.start()),m.group().decode(errors='replace'))

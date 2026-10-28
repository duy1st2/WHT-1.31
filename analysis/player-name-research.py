from port3_research import *
b,base,bits=image(8740)
for a,n in [(0x8deec0,100),(0x91bb50,120)]: print('NEW',hex(a));dis(b,base,bits,a,n)
b,base=pe(root.parent/'Game.dll')
for a,n in [(0x53f900,75),(0x53f8d0,60)]: print('OLD',hex(a));dis(b,base,32,a,n)

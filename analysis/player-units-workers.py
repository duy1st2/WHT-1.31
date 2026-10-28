from port3_research import *
b,base,bits=image(24984)
for a,n in [(0xd09a90,400),(0xac59d0,100)]: print('WORKER',hex(a));dis(b,base,bits,a,n)
row=json.loads((root/'analysis/player-units-live-18240.json').read_text());print([h for h in row['hits'] if h['name']=='GroupEnumUnitsOfPlayer'])

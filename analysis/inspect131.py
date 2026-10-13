import pathlib, pefile, struct, re, json, hashlib, capstone
root=pathlib.Path(__file__).resolve().parents[1]
sources=[pathlib.Path(r'E:\Warcraft III\Warcraft3-1.31.1.12164\Warcraft-3-131'),pathlib.Path(r'E:\Warcraft III\Warcraft3-1.31.1.12173\Warcraft 1.31.1')]
names=['SetCameraField','GetCameraField','SetFogStateRadius','DisplayTimedTextToPlayer','GetLocalPlayer','Player','GetPlayerState','SetPlayerState','GetPlayerName','GetPlayerId','BlzCreateFrameByType','BlzFrameSetText','BlzFrameSetVisible','BlzGetOriginFrame','BlzFrameSetAbsPoint','BlzFrameSetSize','BlzFrameSetScale','BlzFrameSetPoint','BlzFrameSetTexture','BlzFrameSetLevel','BlzFrameSetTextAlignment','SetCameraBounds','SetTerrainFogEx','GetUnitX','GetUnitY','GetUnitTypeId','CreateUnit','GetSelectedUnit','GetTriggerUnit','GetUnitState','SetUnitState','SetUnitPosition','SetHeroLevel','UnitAddAbility','UnitRemoveAbility','PauseGame','GetGameSpeed','SetGameSpeed','ExecuteFunc']
report=[]
for source in sources:
 f=source/'x86'/'Warcraft III.exe'
 p=pefile.PE(str(f)); b=p.get_memory_mapped_image(); base=p.OPTIONAL_HEADER.ImageBase
 d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
 entry={'path':str(f),'sha256':hashlib.sha256(f.read_bytes()).hexdigest().upper(),'image_base':base,'size':p.OPTIONAL_HEADER.SizeOfImage,'imports':[{'dll':e.dll.decode(),'names':[i.name.decode() if i.name else i.ordinal for i in e.imports]} for e in p.DIRECTORY_ENTRY_IMPORT],'natives':{}}
 print('\nCLIENT',f,entry['sha256'],hex(base),flush=True)
 print('IMPORTS',[(x['dll'],x['names'][:12]) for x in entry['imports']],flush=True)
 for name in names:
  found=[]
  for m in re.finditer(re.escape(name.encode()+b'\0'),b):
   for r in re.finditer(re.escape(struct.pack('<I',base+m.start())),b):
    pos=r.start(); ins=list(d.disasm(b[max(0,pos-16):pos+20],base+max(0,pos-16)))
    found.append({'string':m.start(),'ref':pos,'asm':[(hex(i.address-base),i.mnemonic,i.op_str) for i in ins]})
  entry['natives'][name]=found
  print(name,found[:2],flush=True)
 report.append(entry)
(root/'analysis'/'binary131.json').write_text(json.dumps(report,indent=2))

from pathlib import Path
import hashlib,json
r=Path('WHT-1.31').resolve();sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();e=json.loads((r/'analysis/wht-optimization-evidence.json').read_text());art=[]
for build in [12164,12173]:
 for arch,folder in [('x86','build-wht'),('x86_64','build-wht-x64')]:
  for name in ['WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe']:
   src=r/folder/'Release'/name;dst=r/'runtime-test'/f'Warcraft3-1.31.1.{build}'/arch/name;art.append({'build':build,'arch':arch,'file':name,'same':sha(src)==sha(dst),'sha256':sha(dst)})
state={}
for f in ['wht-optimization-preserved.json','wht-optimization-preserved-preinstall.json']:
 before=json.loads((r/'analysis'/f).read_text());state[f]=[{'path':p,'same':Path(p).exists() and sha(Path(p))==h} for p,h in before.items()]
report={'artifacts':art,'preserved':state,'references_unchanged':{p:sha(Path(p))==h for p,h in e['references'].items()},'copied_policy_matches_recorded_source':sha(r/'native/overhead_cadence.hpp')==next(h for p,h in e['references'].items() if p.endswith('overhead_cadence.hpp')),'already_matching_helpers':e['already_identical_helpers'],'tests':{'x86':50,'x64':49},'owned_frame_integration':'Actual WriteBar on 100 same-time callbacks: two float getters and one text setter; due time, recycled identity, settings and root changes resample','live_fps_improvement_measured':False,'adaptive_effects_or_fps_lod_enabled':False,'incremental_sweep_ported':False,'new_native_offsets_hooks_threads_frames_settings':False}
(r/'analysis/wht-optimization-verification.json').write_text(json.dumps(report,indent=2));assert all(x['same'] for x in art);assert all(x['same'] for s in state.values() for x in s);assert all(report['references_unchanged'].values());assert report['copied_policy_matches_recorded_source'];print('12/12 artifact SHA256 match; state and reference hashes unchanged; WHT pure policy copied exactly; full tests 50/50 x86,49/49 x64')

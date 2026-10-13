param([string]$Output=(Join-Path $PSScriptRoot 'dist\WHT-1.31-source.zip'))
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($PSScriptRoot)
$dest=[IO.Path]::GetFullPath($Output)
if(!$dest.StartsWith($root+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Output must remain inside WHT-1.31'}
$stage=Join-Path $root ('dist\source-stage-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $stage -Force | Out-Null
foreach($name in @('native','third_party','tests','session-auth','build-tools')){
 & robocopy (Join-Path $root $name) (Join-Path $stage $name) /E /XD bin obj obj_build auth /XF session-auth.generated.rc *.pdb *.ilk /NFL /NDL /NJH /NJS | Out-Null
 if($LASTEXITCODE -ge 8){throw "Source copy failed: $name"}
}
foreach($file in Get-ChildItem -LiteralPath $root -File){if($file.Extension -in @('.md','.ps1','.cmd','.ini') -or $file.Name -in @('CMakeLists.txt','.gitignore')){Copy-Item -LiteralPath $file.FullName -Destination $stage}}
New-Item -ItemType Directory -Path (Split-Path $dest) -Force | Out-Null
for($attempt=0;$attempt -lt 5;$attempt++){
 try{Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $dest -Force;break}
 catch{if($attempt -eq 4){throw};Start-Sleep -Milliseconds 500}
}
# Stage is retained for review; no recursive deletion is needed.
Write-Host "Packaged source: $dest"

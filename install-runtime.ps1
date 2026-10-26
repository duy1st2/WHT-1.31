param([ValidateSet("x86","x64")][string]$Architecture="x86",[string]$BuildDir=(Join-Path $PSScriptRoot 'build-wht'),[string[]]$Targets=@((Join-Path $PSScriptRoot 'runtime-test\Warcraft3-1.31.1.12164'),(Join-Path $PSScriptRoot 'runtime-test\Warcraft3-1.31.1.12173')))
$ErrorActionPreference='Stop'
$clientArch=if($Architecture -eq 'x64'){'x86_64'}else{'x86'}
if($Architecture -eq 'x64' -and $BuildDir -eq (Join-Path $PSScriptRoot 'build-wht')){$BuildDir=Join-Path $PSScriptRoot 'build-wht-x64'}
if(![IO.Path]::IsPathRooted($BuildDir)){$BuildDir=Join-Path $PSScriptRoot $BuildDir}
$BuildDir=[IO.Path]::GetFullPath($BuildDir)
$allowedRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'runtime-test')).TrimEnd('\')+'\'
$hashes=@{'12164'='9FEAB92F0D8898904122E25AC0ECE42C2070EBD443E4716A62B4767B0F6D0D72';'12173'='93E3D7B38952D6649383C636AF4F766AA599CA25E10A150774F221F1CB4C3EA1'}
if($Architecture -eq 'x64'){$hashes=@{'12164'='0F82359F25B0C25FED3BD379C482BDD695636F16E064526C2943E453FF4E9AAF';'12173'='BA77193D0C5A94F3EDCFD4933987764C6FAF8AB97CB93D6256B645BBAC8939E1'}}
$running=@(Get-CimInstance Win32_Process -Filter "Name='war3.exe' OR Name='Warcraft III.exe'")
$release=Join-Path $BuildDir 'Release';$stamp=Get-Date -Format 'yyyyMMdd-HHmmss';$report=@()
$assets=Join-Path $PSScriptRoot 'dist\client-assets'
if(!(Test-Path -LiteralPath (Join-Path $assets 'fonts_VN.ttf'))){throw 'Missing extracted assets; run build.ps1 first'}
# Validate every destination and artifact before writing any files.
foreach($name in @('WHT.mix','WHT131Launcher.exe')){if(!(Test-Path -LiteralPath (Join-Path $release $name))){throw "Missing build artifact: $name"}}
foreach($target in $Targets){
 $full=[IO.Path]::GetFullPath($target).TrimEnd('\')
 if(!$full.StartsWith($allowedRoot,[StringComparison]::OrdinalIgnoreCase)){throw "Installation outside runtime-test refused: $full"}
 $current=Get-Item -LiteralPath $full
 while($current.FullName.TrimEnd('\') -ne $allowedRoot.TrimEnd('\')){if($current.Attributes -band [IO.FileAttributes]::ReparsePoint){throw "Reparse-point destination refused: $full"};$current=$current.Parent;if(!$current){throw 'Target validation failed'}}
 $exe=Join-Path $full ($clientArch+'\Warcraft III.exe');$version=(Get-Item -LiteralPath $exe).VersionInfo
 if($version.FileMajorPart -ne 1 -or $version.FileMinorPart -ne 31 -or $version.FileBuildPart -ne 1 -or !$hashes.ContainsKey([string]$version.FilePrivatePart)){throw "Unsupported version: $exe"}
 if((Get-FileHash -LiteralPath $exe).Hash -ne $hashes[[string]$version.FilePrivatePart]){throw "Executable hash mismatch: $exe"}
 $arch=Get-Item -LiteralPath (Join-Path $full $clientArch);if($arch.Attributes -band [IO.FileAttributes]::ReparsePoint){throw 'x86 reparse point refused'}
 foreach($name in @('WHT.mix','WHT131Launcher.exe','WHT.ini')){$existing=Join-Path $arch.FullName $name;if((Test-Path -LiteralPath $existing) -and ((Get-Item -LiteralPath $existing).Attributes -band [IO.FileAttributes]::ReparsePoint)){throw "Reparse-point file refused: $existing"}}
 foreach($base in @($full,$arch.FullName)){
  $assetDest=Join-Path $base 'WHT-assets'
  if(Test-Path -LiteralPath $assetDest){foreach($entry in @((Get-Item -LiteralPath $assetDest)) + @(Get-ChildItem -LiteralPath $assetDest -Recurse -Force)){if($entry.Attributes -band [IO.FileAttributes]::ReparsePoint){throw "Asset reparse point refused: $($entry.FullName)"}}}
 }
}
foreach($target in $Targets){
 $full=[IO.Path]::GetFullPath($target).TrimEnd('\');$arch=Join-Path $full $clientArch
 $run=Join-Path $full $(if($Architecture -eq 'x64'){'Run-WHT-1.31-x64.cmd'}else{'Run-WHT-1.31.cmd'})
 if((Test-Path -LiteralPath $run) -and ((Get-Item -LiteralPath $run).Attributes -band [IO.FileAttributes]::ReparsePoint)){throw 'Run script reparse point refused'}
 # Refresh the build before launching; a loaded MIX is never replaced.
 $commands=@('@echo off',('powershell -NoProfile -ExecutionPolicy Bypass -File "'+(Join-Path $PSScriptRoot 'install-runtime.ps1')+'" -BuildDir "'+$BuildDir+'" -Architecture '+$Architecture+' -Targets "%~dp0."'),'if errorlevel 1 exit /b 1',('"%~dp0'+$clientArch+'\WHT131Launcher.exe" --client "%~dp0."'),'pause')
 [IO.File]::WriteAllText($run,($commands -join "`r`n")+"`r`n")
 if(@($running|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.Equals((Join-Path $arch 'Warcraft III.exe'),[StringComparison]::OrdinalIgnoreCase)}).Count){Write-Warning "Running client; installation pending: $full ($clientArch)";continue}
 $ini=Join-Path $arch 'WHT.ini';$iniHash=if(Test-Path -LiteralPath $ini){(Get-FileHash -LiteralPath $ini).Hash}else{''}
 $backup=Join-Path $arch ('backup-WHT131-'+$stamp);New-Item -ItemType Directory -Path $backup | Out-Null
 foreach($name in @('WHT.mix','WHT131Launcher.exe')){
  $source=Join-Path $release $name;$dest=Join-Path $arch $name
  if(Test-Path -LiteralPath $dest){Copy-Item -LiteralPath $dest -Destination $backup}
  Copy-Item -LiteralPath $source -Destination $dest -Force
  $built=(Get-FileHash -LiteralPath $source).Hash;if($built -ne (Get-FileHash -LiteralPath $dest).Hash){throw "Installed hash mismatch: $dest"}
  $report+=@{client=$full;file=$name;sha256=$built;backup=$backup}
 }
 # 1.31 local lookup may use either the client root or the x86 working dir.
 foreach($base in @($full,$arch)){
  $assetDest=Join-Path $base 'WHT-assets';New-Item -ItemType Directory -Path $assetDest -Force | Out-Null
  foreach($obsolete in @('AARDV.ttf','WHTUI_FRIZQT__.ttf')){
   $oldAsset=Join-Path $assetDest $obsolete
   if(Test-Path -LiteralPath $oldAsset){
    $old=Join-Path $backup ((Split-Path $base -Leaf)+'\assets\'+$obsolete)
    New-Item -ItemType Directory -Path (Split-Path $old) -Force | Out-Null
    Copy-Item -LiteralPath $oldAsset -Destination $old -Force
    if((Get-FileHash -LiteralPath $oldAsset).Hash -ne (Get-FileHash -LiteralPath $old).Hash){throw 'Obsolete asset backup mismatch'}
    Remove-Item -LiteralPath $oldAsset
   }
  }
  foreach($asset in Get-ChildItem -LiteralPath $assets -Recurse -File){
   $relative=$asset.FullName.Substring($assets.Length+1);$dest=Join-Path $assetDest $relative
   if(Test-Path -LiteralPath $dest){$old=Join-Path $backup ((Split-Path $base -Leaf)+'\assets\'+$relative);New-Item -ItemType Directory -Path (Split-Path $old) -Force | Out-Null;Copy-Item -LiteralPath $dest -Destination $old}
   New-Item -ItemType Directory -Path (Split-Path $dest) -Force | Out-Null;Copy-Item -LiteralPath $asset.FullName -Destination $dest -Force
   if((Get-FileHash -LiteralPath $dest).Hash -ne (Get-FileHash -LiteralPath $asset.FullName).Hash){throw "Asset hash mismatch: $dest"}
  }
 }
 if($iniHash){if($iniHash -ne (Get-FileHash -LiteralPath $ini).Hash){throw "INI changed: $ini"}}
 else{Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'WHT.ini') -Destination $ini}
 Write-Host "Installed and hash verified: $full"
}
New-Item -ItemType Directory -Path (Join-Path $PSScriptRoot 'analysis') -Force | Out-Null
$report|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $PSScriptRoot "analysis\deployment-$stamp.json") -Encoding UTF8

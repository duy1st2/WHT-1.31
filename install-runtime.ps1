param([string]$BuildDir=(Join-Path $PSScriptRoot 'build-wht'),[string[]]$Targets=@((Join-Path $PSScriptRoot 'runtime-test\Warcraft3-129'),'E:\Warcraft III\Warcraft3-129'))
$ErrorActionPreference='Stop'
$release=Join-Path $BuildDir 'Release'
$stamp=Get-Date -Format 'yyyyMMdd-HHmmss'
$running=@(Get-CimInstance Win32_Process -Filter "Name='war3.exe' OR Name='Warcraft III.exe'")
foreach($target in $Targets){
 if(!(Test-Path -LiteralPath (Join-Path $target 'war3.exe'))){Write-Warning "Client not found; skipped: $target";continue}
 $version=(Get-Item -LiteralPath (Join-Path $target 'war3.exe')).VersionInfo
 if($version.FileMajorPart -ne 1 -or $version.FileMinorPart -ne 29 -or $version.FileBuildPart -ne 2 -or $version.FilePrivatePart -ne 9231){throw "Not the supported 1.29.2.9231 client; installation refused: $target"}
 $destination=[IO.Path]::GetFullPath($target).TrimEnd('\')
 if(@($running | Where-Object {$_.ExecutablePath -and [IO.Path]::GetFullPath((Split-Path $_.ExecutablePath -Parent)).TrimEnd('\') -eq $destination}).Count){Write-Warning "Running client; installation pending: $target";continue}
 $ini=Join-Path $target 'WHT.ini'
 $iniHash=if(Test-Path -LiteralPath $ini){(Get-FileHash -LiteralPath $ini).Hash}else{''}
 $backup=Join-Path $target ('backup-WHT129-'+$stamp)
 New-Item -ItemType Directory -Path $backup | Out-Null
 foreach($name in @('WHT.mix','mss32.dll')){
  $source=Join-Path $release $name;$output=Join-Path $target $name
  if(!(Test-Path -LiteralPath $source)){throw "Missing build output: $source"}
  if(!(Test-Path -LiteralPath (Join-Path $target 'WHT_MSS32_Original.dll'))){throw "Original Miles DLL missing: $target"}
  if(Test-Path -LiteralPath $output){Copy-Item -LiteralPath $output -Destination $backup}
  Copy-Item -LiteralPath $source -Destination $output -Force
  if((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $output).Hash){throw "Hash mismatch: $output"}
 }
 if($iniHash){if($iniHash -ne (Get-FileHash -LiteralPath $ini).Hash){throw "INI changed: $target"}}
 else{Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'WHT.ini') -Destination $ini}
 Write-Host "Installed and verified WHT 1.29: $target"
}

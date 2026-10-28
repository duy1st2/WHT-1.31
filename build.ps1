param([ValidateSet("x86","x64")][string]$Architecture="x86",[string]$BuildDir='build-wht',[string]$Generator='Visual Studio 18 2026',[switch]$WithTests,[switch]$Install)
$ErrorActionPreference='Stop'
if($Architecture -eq 'x64' -and $BuildDir -eq 'build-wht'){$BuildDir='build-wht-x64'}
$platform=if($Architecture -eq 'x64'){'x64'}else{'Win32'}
function CheckExit {if($LASTEXITCODE -ne 0){throw "Build tool failed: $LASTEXITCODE"}}
$root=[IO.Path]::GetFullPath($PSScriptRoot)
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildDir))
if(!$build.StartsWith($root+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'BuildDir must stay inside WHT-1.31'}
& (Join-Path $root 'build-session-auth.ps1');CheckExit
$cmake=(Get-Command cmake -ErrorAction SilentlyContinue).Source
if(!$cmake){$cmake='C:\Program Files\CMake\bin\cmake.exe'}
$testing=if($WithTests){'ON'}else{'OFF'}
& $cmake -S $root -B $build -G $Generator -A $platform "-DBUILD_TESTING=$testing";CheckExit
& $cmake --build $build --config Release --parallel 4;CheckExit
if($Architecture -eq 'x86'){& (Join-Path $build 'Release\WHT131ExtractAssets.exe') (Join-Path $root 'native\assets\tl_overlay.mpq') (Join-Path $root 'tests\support\Storm.dll') (Join-Path $root 'dist\client-assets');CheckExit}
if($WithTests){& (Join-Path (Split-Path $cmake) 'ctest.exe') --test-dir $build -C Release --output-on-failure;CheckExit}
$dist=Join-Path $root ('dist\client-'+$Architecture)
New-Item -ItemType Directory -Path $dist -Force | Out-Null
foreach($name in @('WHT.mix','WHT131Launcher.exe','WHT.ExternalHud.exe')){Copy-Item -LiteralPath (Join-Path $build "Release\$name") -Destination (Join-Path $dist $name) -Force}
Copy-Item -LiteralPath (Join-Path $root 'WHT.ini') -Destination (Join-Path $dist 'WHT.example.ini') -Force
if($Install){& (Join-Path $root 'install-runtime.ps1') -BuildDir $build -Architecture $Architecture}
Write-Host "WHT 1.31 $Architecture build complete: $build\Release"

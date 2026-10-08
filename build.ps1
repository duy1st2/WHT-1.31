param([string]$BuildDir='build-wht',[string]$Generator='Visual Studio 18 2026',[string]$PythonExecutable='',[switch]$WithTests,[switch]$Install)
$ErrorActionPreference='Stop'
$root=$PSScriptRoot
function CheckExit {if($LASTEXITCODE -ne 0){throw "Build tool failed: $LASTEXITCODE"}}
$cmake=(Get-Command cmake -ErrorAction SilentlyContinue).Source
if(!$cmake){$cmake='C:\Program Files\CMake\bin\cmake.exe'}
$ctest=Join-Path (Split-Path $cmake) 'ctest.exe'
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildDir))
if(!$build.StartsWith($root+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'BuildDir must be inside WHT-1.29.'}
if(!$PythonExecutable){
 $cached=Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
 if(Test-Path -LiteralPath $cached){$PythonExecutable=$cached}
}
$pythonArgs=@();if($PythonExecutable){$pythonArgs+="-DPython3_EXECUTABLE=$PythonExecutable"}
$testing=if($WithTests){'ON'}else{'OFF'}
& $cmake -S $root -B $build -G $Generator -A Win32 "-DBUILD_TESTING=$testing" @pythonArgs
CheckExit
& $cmake --build $build --config Release --parallel 4
CheckExit
if($WithTests){& $ctest --test-dir $build -C Release --output-on-failure;CheckExit}
$release=Join-Path $build 'Release'
$dist=Join-Path $root 'dist\client'
New-Item -ItemType Directory -Path $dist -Force | Out-Null
foreach($name in @('WHT.mix','mss32.dll')){Copy-Item -LiteralPath (Join-Path $release $name) -Destination (Join-Path $dist $name) -Force}
Copy-Item -LiteralPath (Join-Path $root 'WHT.ini') -Destination (Join-Path $dist 'WHT.example.ini') -Force
if(!(Test-Path -LiteralPath (Join-Path $dist 'WHT.ini'))){Copy-Item -LiteralPath (Join-Path $root 'WHT.ini') -Destination (Join-Path $dist 'WHT.ini')}
Copy-Item -LiteralPath (Join-Path $root 'third_party\minhook\LICENSE.txt') -Destination (Join-Path $dist 'MinHook.LICENSE.txt') -Force
if($Install){& (Join-Path $root 'install-runtime.ps1') -BuildDir $build}
Write-Host "WHT 1.29 build complete: $release" -ForegroundColor Green

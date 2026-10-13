param([string]$AppSource='D:\project-w3\LoadFileMapWar3 - Copy\LoadFileMapWar3',[string]$Output='dist\session-auth-bin')
$ErrorActionPreference='Stop'
$target=Join-Path $PSScriptRoot $Output
$generated=Join-Path $PSScriptRoot 'session-auth\auth'
& 'C:\Program Files\dotnet\dotnet.exe' run --project (Join-Path $PSScriptRoot 'build-tools\SealConfiguration\SealConfiguration.csproj') -c Release -- $AppSource $generated
if($LASTEXITCODE -ne 0){throw 'Configuration sealing failed'}
$versionLine=(Get-Content -LiteralPath (Join-Path $PSScriptRoot 'native\tool_version.hpp') | Where-Object {$_ -match '^#define WHT_VERSION_STRING '})
$toolVersion=[regex]::Match($versionLine,'\d+\.\d+\.\d+').Value
if(!$toolVersion){throw 'Missing tool version'}
& 'C:\Program Files\dotnet\dotnet.exe' publish (Join-Path $PSScriptRoot 'session-auth\WHT.SessionAuth.csproj') -c Release -r win-x64 --self-contained false -p:PublishSingleFile=true -p:DebugType=None "-p:Version=$toolVersion" -o $target --nologo
if($LASTEXITCODE -ne 0){throw 'Session helper build failed'}
& (Join-Path $target 'WHT.SessionAuth.exe') --self-test
if($LASTEXITCODE -ne 0){throw 'Session helper tests failed'}
# Remove files left over from the former multi-file publish.
foreach($file in Get-ChildItem -LiteralPath $target -File){if($file.Name -ne 'WHT.SessionAuth.exe'){Remove-Item -LiteralPath $file.FullName}}
$resource=Join-Path $target 'WHT.SessionAuth.exe'
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'native\session-auth.generated.rc'),('102 RCDATA "'+$resource.Replace('\','/')+'"'))

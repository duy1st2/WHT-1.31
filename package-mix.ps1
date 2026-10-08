param([Parameter(Mandatory=$true)][string]$Dll,[Parameter(Mandatory=$true)][string]$Archive,[Parameter(Mandatory=$true)][string]$Output)
$ErrorActionPreference='Stop'
$pe=[IO.File]::ReadAllBytes($Dll)
$mpq=[IO.File]::ReadAllBytes($Archive)
if($pe[0] -ne 77 -or $pe[1] -ne 90 -or $mpq[0] -ne 77 -or $mpq[1] -ne 80 -or $mpq[2] -ne 81 -or $mpq[3] -ne 26){throw 'Invalid PE or MPQ input'}
# Storm scans for an MPQ header at 512-byte boundaries. Windows loads the PE prefix.
$offset=[int]([Math]::Ceiling($pe.Length/512.0)*512)
$bytes=New-Object byte[] ($offset+$mpq.Length)
[Array]::Copy($pe,0,$bytes,0,$pe.Length)
[Array]::Copy($mpq,0,$bytes,$offset,$mpq.Length)
[IO.File]::WriteAllBytes($Output,$bytes)
Write-Host "Packaged WHT.mix: $($bytes.Length) bytes; MPQ offset $offset"

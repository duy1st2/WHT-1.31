param([string]$Output='')
$ErrorActionPreference='Stop'
if(!$Output){$Output=Join-Path $PSScriptRoot ('backup\WHT-1.29-source-'+(Get-Date -Format 'yyyyMMdd-HHmmss')+'.zip')}
if(Test-Path -LiteralPath $Output){throw 'Archive already exists.'}
New-Item -ItemType Directory -Path (Split-Path $Output) -Force | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive=[IO.Compression.ZipFile]::Open([IO.Path]::GetFullPath($Output),[IO.Compression.ZipArchiveMode]::Create)
try{
 $files=@(Get-ChildItem -LiteralPath $PSScriptRoot -File | Where-Object {$_.Extension -in @('.txt','.ps1','.cmd','.py','.ini','.md')})
 foreach($folder in @('native','tests','third_party')){$files+=Get-ChildItem -LiteralPath (Join-Path $PSScriptRoot $folder) -Recurse -File}
 foreach($file in $files){
  $relative=$file.FullName.Substring($PSScriptRoot.Length+1).Replace('\','/')
  [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive,$file.FullName,'WHT-1.29/'+$relative,[IO.Compression.CompressionLevel]::Optimal)|Out-Null
 }
}finally{$archive.Dispose()}
Write-Output $Output

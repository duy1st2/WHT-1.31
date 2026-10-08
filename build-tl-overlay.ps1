param([string]$MpqEditor = "E:\tool war3\mpqeditor_en_v3.6.0.868\Win32\MPQEditor.exe")
$ErrorActionPreference='Stop'
$root=$PSScriptRoot
$output=Join-Path $root 'native/assets/tl_overlay.mpq'
$work=Join-Path $root ('analysis/overlay-clean-'+(Get-Date -Format yyyyMMdd-HHmmss))
New-Item -ItemType Directory -Path $work|Out-Null
foreach($dir in @('source','staging','verify')){New-Item -ItemType Directory -Path (Join-Path $work $dir)|Out-Null}
Copy-Item -LiteralPath $output -Destination (Join-Path $work 'original.mpq')
function RunEditor([string]$commands,[string]$cwd){
 $script=Join-Path $cwd 'commands.txt';[IO.File]::WriteAllText($script,$commands+"`r`nexit`r`n")
 $p=Start-Process -FilePath $MpqEditor -ArgumentList ('/console "'+$script+'"') -WorkingDirectory $cwd -WindowStyle Hidden -PassThru
 if(!$p.WaitForExit(30000)){throw 'MPQEditor timed out'}
 if($p.ExitCode -ne 0){throw 'MPQEditor failed'}
}
RunEditor ('extract "'+$output+'" * source'+"`r`n"+'extract "'+$output+'" UI\Widgets\EscMenu\Human\human-options-button-border-up.blp source'+"`r`n"+'extract "'+$output+'" UI\Widgets\EscMenu\Human\human-options-menu-background.blp source') $work
# Only runtime resources. Explicit relative archive names prevent machine paths.
$names=@('AARDV.ttf','fonts_VN.ttf','FRIZQT__.ttf','WHTUI_FRIZQT__.ttf',
'BTNAllSkills.blp','BTNAllSkillsDow.blp','BTNAutoPick.blp','BTNAutoPickDow.blp',
'BTNDropItem.blp','BTNDropItemDow.blp','BTNSellItem.blp','BTNSellItemDow.blp','BTNWHT_Menu.blp',
'BTNWHT_Rec.blp','CommandCardHuman.tga','InventoryHuman.tga','Transparent.tga','WHTUI_GreyBackground.tga',
'UI\Widgets\EscMenu\Human\human-options-button-border-up.blp','UI\Widgets\EscMenu\Human\human-options-menu-background.blp')
$stage=Join-Path $work 'staging'
foreach($name in $names){
 $leaf=[IO.Path]::GetFileName($name)
 $src=Join-Path $work ('source/'+$leaf)
 if(!(Test-Path -LiteralPath $src)){$src=Join-Path $root ("native/assets/"+$leaf)}
 if(!(Test-Path -LiteralPath $src)){throw "Missing source $name"}
 $dest=Join-Path $stage $name
 New-Item -ItemType Directory -Path ([IO.Path]::GetDirectoryName($dest)) -Force|Out-Null
 Copy-Item -LiteralPath $src -Destination $dest
}
$new=Join-Path $work 'clean.mpq'
RunEditor ('new "'+$new+'" 64') $stage
$commands=''
foreach($name in $names){$commands+='add "'+$new+'" "'+$name+'" /c'+"`r`n"}
$commands+='compact "'+$new+'"'+"`r`n"
for($i=0;$i -lt $names.Count;$i++){
 $v=Join-Path $work ('verify/'+$i);New-Item -ItemType Directory -Path $v|Out-Null
 $commands+='extract "'+$new+'" "'+$names[$i]+'" "..\verify\'+$i+'"'+"`r`n"
}
$commands+='extract "'+$new+'" (listfile) ..\verify'+"`r`n"
RunEditor $commands $stage
for($i=0;$i -lt $names.Count;$i++){
 $files=@(Get-ChildItem -LiteralPath (Join-Path $work ('verify/'+$i)) -Recurse -File)
 if($files.Count -ne 1 -or (Get-FileHash -LiteralPath $files[0].FullName).Hash -ne (Get-FileHash -LiteralPath (Join-Path $stage $names[$i])).Hash){throw "Read-back mismatch: $($names[$i])"}
}
$list=@(Get-Content -LiteralPath (Join-Path $work 'verify/(listfile)'))
if($list.Count -ne $names.Count -or @(Compare-Object $list $names).Count){throw 'Archive contains unexpected files'}
$before=(Get-Item -LiteralPath $output).Length
Copy-Item -LiteralPath $new -Destination $output -Force
[pscustomobject]@{Before=$before;After=(Get-Item -LiteralPath $output).Length;Resources=$names.Count;Backup=$work;SHA256=(Get-FileHash -LiteralPath $output).Hash}|ConvertTo-Json|Tee-Object -FilePath (Join-Path $work 'report.json')

$UAT = 'C:\UnrealEngine\5.4\Engine\Build\BatchFiles\RunUAT.bat'
$ProjectPath = "$pwd\..\..\VC.uproject"
$OutputDirectory = "$pwd\..\"

Write-Output "========== Pulling code =========="
& git pull
Write-Output "========== Building client ========"
& $UAT BuildCookRun -project="$ProjectPath" platform=Win64 -configuration=Development -build -cook -stage -archive -pak -archivedirectory="$OutputDirectory" 
Write-Output "========== Done building client! =========="
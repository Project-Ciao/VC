$UAT = 'C:\UnrealEngine\5.4\Engine\Build\BatchFiles\RunUAT.bat'
$ProjectPath = "$pwd\..\..\VC.uproject"
$OutputDirectory = "$pwd\..\"

# Build the server
& $UAT BuildCookRun -project="$ProjectPath" platform=Win64 -configuration=Development -client -build -cook -stage -archive -archivedirectory="$OutputDirectory" 
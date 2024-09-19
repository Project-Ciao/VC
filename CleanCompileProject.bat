del VC.sln
RMDIR ".vs" /S /Q
RMDIR ".idea" /S /Q
RMDIR "Binaries" /S /Q
RMDIR "Build" /S /Q
RMDIR "Intermediate" /S /Q
RMDIR "DerivedDataCache" /S /Q

REM RMDIR "Plugins/YOURPLUGIN/Intermediate" /S /Q
REM RMDIR "Plugins/YOURPLUGIN/Intermediate" /S /Q

set MyUVS="C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
set MyUBT="C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
set MyFullPath="%cd%\VC"

%MyUVS% /projectfiles %MyFullPath%.uproject

%MyUBT% Development Win64 -Project=%MyFullPath%.uproject -TargetType=Editor -Progress -NoEngineChanges -NoHotReloadFromIDE

%MyFullPath%.uproject
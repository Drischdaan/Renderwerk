. .\Build\Scripts\Utility.ps1

$OriginalDirectory = Get-Location
Push-Location $PSScriptRoot\..\..

./Binaries/Tools/premake/premake5.exe --file="./Build/Config/premake5.lua" vs2022

Push-Location $OriginalDirectory

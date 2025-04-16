. .\Build\Scripts\Utility.ps1

$SkipConfirm = $false

for ($Index = 0; $Index -lt $args.count; $Index++) {
	if ($args[$Index] -eq "-Skip-Confirm") {
		$SkipConfirm = $true
	}
}

$OriginalDirectory = Get-Location
Push-Location $PSScriptRoot\..\..

function Download-Premake() {
	$PremakeDirectory = "Binaries\Tools\premake"
	$PremakeZipFile = $PremakeDirectory + "\premake.zip"

	Log-Info "Downloading premake..."
	Invoke-WebRequest https://github.com/premake/premake-core/releases/download/v5.0.0-beta6/premake-5.0.0-beta6-windows.zip -OutFile (New-Item -Path $PremakeZipFile -Force)
	Expand-Archive $PremakeZipFile -DestinationPath $PremakeDirectory -Force
	Remove-Item $PremakeZipFile
	Log-Info "Premake downloaded"
}

function Install-GitHooks() {
	$CustomHooksDirectory = "Build\GitHooks\*"
	$GitHooksDirectory = ".git\hooks"

	Log-Info "Install custom git hooks..."
	Copy-Item -Path $CustomHooksDirectory -Destination $GitHooksDirectory -Recurse -Force
	Log-Info "Custom git hooks installed"
}

if (Ask-For-Confirm $SkipConfirm "This script will download needed dependencies in order to setup the workspace. Continue?") {
	Log-Info "Setting up workspace..."

	Download-Premake
	Install-GitHooks

	Log-Info "Workspace is ready!"
}

Push-Location $OriginalDirectory

. .\Build\Scripts\Utility.ps1

$SkipConfirm = $false

for ($Index = 0; $Index -lt $args.count; $Index++) {
	if ($args[$Index] -eq "-Skip-Confirm") {
		$SkipConfirm = $true
	}
}

$OriginalDirectory = Get-Location
Push-Location $PSScriptRoot\..\..

function Install-Premake() {
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

function Install-AgilitySDK() {
	$AgilitySDKDirectory = "Binaries\ThirdParty\AgilitySDK"
	$AgilitySDKZipFile = $AgilitySDKDirectory + "\sdk.zip"

	Log-Info "Downloading Agility SDK..."
	if (Test-Path $AgilitySDKDirectory) {
		Remove-Item -Recurse -Force $AgilitySDKDirectory
	}
	Invoke-WebRequest https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/1.615.1 -OutFile (New-Item -Path $AgilitySDKZipFile -Force)
	Expand-Archive $AgilitySDKZipFile -DestinationPath $AgilitySDKDirectory
	if (Test-Path $AgilitySDKZipFile) {
		Remove-Item -Path $AgilitySDKZipFile
	}
	Log-Info "Agility SDK downloaded"
}

if (Ask-For-Confirm $SkipConfirm "This script will download needed dependencies in order to setup the workspace. Continue?") {
	Log-Info "Setting up workspace..."

	Install-Premake
	Install-AgilitySDK
	Install-GitHooks

	Log-Info "Workspace is ready!"
}

Push-Location $OriginalDirectory

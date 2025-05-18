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

function Install-VulkanSDK() {
	$VulkanSDKDownloadDirectory = "Binaries\ThirdParty\VulkanSDK"
	$VulkanSDKFile = $VulkanSDKDownloadDirectory + "\VulkanSDK.exe"
	$VulkanSDKVersion = "1.4.313.0"
	$VulkanSDKUrl = "https://sdk.lunarg.com/sdk/download/$VulkanSDKVersion/windows/vulkansdk-windows-X64-$VulkanSDKVersion.exe"
	$DefaultVulkanSDKPath = "C:\VulkanSDK\$VulkanSDKVersion"

	$VulkanSDKInstalled = $false
	$EnvVulkanSDK = [System.Environment]::GetEnvironmentVariable("VULKAN_SDK", [System.EnvironmentVariableTarget]::Machine)

	if ($EnvVulkanSDK) {
		$PathVersion = Split-Path $EnvVulkanSDK -Leaf
		if ($PathVersion -like "1.4.*") {
			$VulkanSDKInstalled = $true
			Log-Info "Vulkan SDK 1.4.x is already installed (Version: $PathVersion)"
		}
	}

	if (-not $VulkanSDKInstalled) {
		Log-Info "Vulkan SDK 1.4.x not found. Downloading Vulkan SDK..."

		if (-not (Test-Path $VulkanSDKDownloadDirectory)) {
			New-Item -ItemType Directory -Path $VulkanSDKDownloadDirectory -Force | Out-Null
		}

		Invoke-WebRequest -Uri $VulkanSDKUrl -OutFile $VulkanSDKFile

		Log-Info "Starting Vulkan SDK installer..."

		try {
			$Process = Start-Process -FilePath $VulkanSDKFile -ArgumentList "install", "com.lunarg.vulkan.core", "com.lunarg.vulkan.debug", "--accept-licenses", "--default-answer", "--confirm-command" -Wait -PassThru -Verb runAs

			if ($Process.ExitCode -eq 0) {
				Log-Info "Vulkan SDK installation completed successfully"

				Log-Info "Setting VULKAN_SDK environment variable to: $DefaultVulkanSDKPath"

				$ScriptBlock = {
					param($Path)
					[System.Environment]::SetEnvironmentVariable("VULKAN_SDK", $Path, [System.EnvironmentVariableTarget]::Machine)
				}
				Start-Process powershell -ArgumentList "-Command", "& {$ScriptBlock} $DefaultVulkanSDKPath" -Verb RunAs -Wait

				$env:VULKAN_SDK = $DefaultVulkanSDKPath
				Log-Info "VULKAN_SDK environment variable has been set"
			} else {
				Log-Error "Vulkan SDK installation failed with exit code: $($process.ExitCode)"
				Log-Info "Please run the installer manually and then rerun this script"
				exit 1
			}
		} catch {
			Log-Error "Failed to start Vulkan SDK installer automatically"
			Log-Info "Please run the installer manually from: $VulkanSDKFile"
			Log-Info "After installation is complete, rerun this script"
			exit 1
		} finally {
			if (Test-Path $VulkanSDKFile) {
				Remove-Item -Path $VulkanSDKFile -Force
			}
		}
	}
}

if (Ask-For-Confirm $SkipConfirm "This script will download needed dependencies in order to setup the workspace. Continue?") {
	Log-Info "Setting up workspace..."

	Install-Premake
	Install-AgilitySDK
	Install-GitHooks
	Install-VulkanSDK

	Log-Info "Workspace is ready!"
}

Push-Location $OriginalDirectory

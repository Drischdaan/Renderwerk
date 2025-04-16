function Log-Debug([String] $Message) {
	Write-Host -Foreground Magenta -NoNewLine "[DEBUG] "
	Write-Host $Message
}

function Log-Info([String] $Message) {
	Write-Host -Foreground Green -NoNewLine "[INFO] "
	Write-Host $Message
}

function Log-Warn([String] $Message) {
	Write-Host -Foreground Yellow -NoNewLine "[WARN] "
	Write-Host $Message
}

function Log-Error([String] $Message) {
	Write-Host -Foreground Red -NoNewLine "[ERROR] "
	Write-Host $Message
}

function Ask-For-Confirm([bool] $SkipConfirm, [String] $Description) {
	if($SkipConfirm) {
		return $true
	}
	$Choices  = '&Yes', '&No'
	$Decision = $Host.UI.PromptForChoice("Confirmation", $Description, $Choices, 1)
	Write-Host ""
	return $Decision -eq 0
}

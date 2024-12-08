#pragma once

/**
 * Source location struct that supports wide strings.
 */
struct FSourceLocation
{
	FString File;
	uint32 Line;
	FString Function;

	constexpr FSourceLocation() = default;

	constexpr FSourceLocation(const FString& File, const uint32 Line, const FString& Function)
		: File(File), Line(Line), Function(Function)
	{
	}

	[[nodiscard]] FString GetFormattedFile() const
	{
		FString FormattedFile = File;
		const size64 LastSlashIndex = File.find_last_of(TEXT("\\"));
		if (LastSlashIndex != FString::npos)
			FormattedFile = File.substr(LastSlashIndex + 1);
		return FormattedFile;
	}
};

#define CURRENT_SOURCE_LOCATION FSourceLocation(__FILEW__, __LINE__, __FUNCTIONW__)

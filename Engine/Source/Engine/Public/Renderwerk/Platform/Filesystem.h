#pragma once

#include <filesystem>

using FPath = std::filesystem::path;

struct ENGINE_API FFile
{
public:
	FFile() = default;
	FFile(const FPath& InPath);
	~FFile() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FFile);

public:
	[[nodiscard]] bool8 IsValid() const;
	[[nodiscard]] bool8 Exists() const;

	void CreateIfNotExists() const;

	[[nodiscard]] FString Read() const;
	void Write(const FStringView& Buffer) const;

public:
	[[nodiscard]] FPath GetPath() const { return Path; }

private:
	FPath Path;
};

struct ENGINE_API FDirectory
{
public:
	FDirectory() = default;
	FDirectory(const FPath& InPath);
	~FDirectory() = default;

	DEFINE_DEFAULT_COPY_AND_MOVE(FDirectory);

public:
	[[nodiscard]] bool8 IsValid() const;
	[[nodiscard]] bool8 Exists() const;

	void CreateIfNotExists() const;

	[[nodiscard]] TVector<FFile> GetFiles() const;
	[[nodiscard]] TVector<FDirectory> GetDirectories() const;

public:
	[[nodiscard]] FPath GetPath() const { return Path; }

private:
	FPath Path;
};

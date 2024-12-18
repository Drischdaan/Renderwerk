#include "pch.h"

#include "Renderwerk/Platform/Filesystem.h"

FFile::FFile(const FPath& InPath)
	: Path(InPath)
{
}

bool8 FFile::IsValid() const
{
	return !Path.empty();
}

bool8 FFile::Exists() const
{
	return std::filesystem::exists(Path);
}

void FFile::CreateIfNotExists() const
{
	if (Exists())
		return;
	std::wofstream File(Path);
	File.close();
}

TVector<uint32> FFile::Read() const
{
	std::ifstream File(Path, std::ios::binary | std::ios::ate);
	const std::streamsize Size = File.tellg();
	File.seekg(0, std::ios::beg);
	TVector<uint32> Buffer(Size / sizeof(uint32));
	VERIFY(File.read(reinterpret_cast<char*>(Buffer.data()), Size), "Failed to read file: {}", Path.string());
	File.close();
	return Buffer;
}

FAnsiString FFile::ReadAnsi() const
{
	std::ifstream File(Path, std::ios::binary | std::ios::ate);
	const std::streamsize Size = File.tellg();
	File.seekg(0, std::ios::beg);
	TVector<FAnsiChar> Buffer(Size);
	VERIFY(File.read(Buffer.data(), Size), "Failed to read file: {}", Path.string());
	File.close();
	return FAnsiString(Buffer.data(), Size);
}

FWideString FFile::ReadWide() const
{
	std::wifstream File(Path, std::ios::binary | std::ios::ate);
	const std::streamsize Size = File.tellg();
	File.seekg(0, std::ios::beg);
	TVector<FWideChar> Buffer(Size);
	VERIFY(File.read(Buffer.data(), Size), "Failed to read file: {}", Path.string());
	File.close();
	return FWideString(Buffer.data(), Size);
}

void FFile::Write(const FWideStringView& Buffer) const
{
	std::wofstream File(Path);
	VERIFY(File.write(Buffer.data(), Buffer.size()), "Failed to write file: {}", Path.string());
	File.close();
}

void FFile::Write(const FAnsiStringView& Buffer) const
{
	std::ofstream File(Path);
	VERIFY(File.write(Buffer.data(), Buffer.size()), "Failed to write file: {}", Path.string());
	File.close();
}

FDirectory::FDirectory(const FPath& InPath)
	: Path(InPath)
{
}

bool8 FDirectory::IsValid() const
{
	return !Path.empty();
}

bool8 FDirectory::Exists() const
{
	return std::filesystem::exists(Path);
}

void FDirectory::CreateIfNotExists() const
{
	if (Exists())
		return;
	std::filesystem::create_directory(Path);
}

TVector<FFile> FDirectory::GetFiles() const
{
	TVector<FFile> Files;
	for (const auto& Entry : std::filesystem::directory_iterator(Path))
	{
		if (Entry.is_regular_file())
			Files.emplace_back(FFile(Entry.path()));
	}
	return Files;
}

TVector<FDirectory> FDirectory::GetDirectories() const
{
	TVector<FDirectory> Directories;
	for (const auto& Entry : std::filesystem::directory_iterator(Path))
	{
		if (Entry.is_directory())
			Directories.emplace_back(FDirectory(Entry.path()));
	}
	return Directories;
}

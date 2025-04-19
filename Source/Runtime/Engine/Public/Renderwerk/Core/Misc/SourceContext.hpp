#pragma once

#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/String.hpp"

struct FSourceContext
{
	FStringView File;
	uint32 Line;
	FStringView Function;

	constexpr FSourceContext(const FStringView& InFile, const uint32 InLine, const FStringView& InFunction)
		: File(InFile), Line(InLine), Function(InFunction)
	{
	}

	[[nodiscard]] constexpr FStringView GetFormattedFile() const
	{
		const size64 LastSlashIndex = File.find_last_of('\\');
		if (LastSlashIndex != FStringView::npos)
		{
			return File.substr(LastSlashIndex + 1);
		}
		return File;
	}
};

#define CURRENT_CONTEXT() FSourceContext(__FILEW__, __LINE__, __FUNCTIONW__)

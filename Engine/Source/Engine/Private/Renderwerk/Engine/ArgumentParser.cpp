#include "pch.h"

#include "Renderwerk/Engine/ArgumentParser.h"

bool8 FArgument::IsValid() const
{
	return Name.IsValid() && !Value.empty();
}

FArgumentParser::FArgumentParser(TVector<FString>&& InArguments)
{
	const TVector<FString> Arguments = std::move(InArguments);
	for (size64 Index = 0; Index < Arguments.size(); ++Index)
	{
		FStringView Argument = Arguments[Index];
		if (Argument.empty()) // Skip empty arguments
			continue;
		if (Argument.starts_with(TEXT("--")) && Argument.size() > 2) // Named argument
		{
			const FStringView Name = Argument.substr(2);
			if (Index + 1 < Arguments.size()) // Has value
			{
				FStringView Value = Arguments[Index + 1];
				if (Value.starts_with(TEXT("--"))) // Current index is a flag
				{
					FName ArgumentName = FName(FString(Name));
					ArgumentMap.insert_or_assign(ArgumentName, FArgument(ArgumentName, TEXT("true")));
				}
				else // Is a value
				{
					FName ArgumentName = FName(FString(Name));
					ArgumentMap.insert_or_assign(ArgumentName, FArgument(ArgumentName, FString(Value)));
					Index++;
				}
			}
			else // Is last argument, so it's treated as a flag
			{
				FName ArgumentName = FName(FString(Name));
				ArgumentMap.insert_or_assign(ArgumentName, FArgument(ArgumentName, TEXT("true")));
			}
		}
		else // Positional argument
		{
			INTERRUPT(IR_UNKNOWN, "Positional arguments are not supported");
		}
	}
}

FArgumentParser::~FArgumentParser()
{
}

bool8 FArgumentParser::HasArgument(const FName& Name) const
{
	return ArgumentMap.contains(Name);
}

FArgument& FArgumentParser::GetArgument(const FName& Name)
{
	VERIFY(HasArgument(Name), "Trying to access non-existing argument");
	return ArgumentMap.at(Name);
}

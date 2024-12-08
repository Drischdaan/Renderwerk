#pragma once

#include "Renderwerk/Core/CoreAPI.h"

/**
 * Utility class for hashing data.
 */
class ENGINE_API FHasher
{
public:
	/**
	 * Hashes the given string. Implementation based on https://gist.github.com/hwei/1950649d523afd03285c
	 * @param InString The string to hash.
	 * @return The hash of the data.
	 */
	static constexpr uint32 HashString(const FStringView InString)
	{
		const FChar* Data = InString.data();
		const size64 Length = InString.size();
		uint32 Hash = FNVOffsetBasis;
		for (size64 Index = 0; Index < Length; ++Index)
		{
			Hash ^= *Data++;
			Hash *= FNVPrime;
		}
		// Apply the null terminator to the hash
		Hash ^= '\0';
		Hash *= FNVPrime;
		return Hash;
	}

private:
	static constexpr uint32 FNVPrime = 16777619u;
	static constexpr uint32 FNVOffsetBasis = 2166136261u;
};

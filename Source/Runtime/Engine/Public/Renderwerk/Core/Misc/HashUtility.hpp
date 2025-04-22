#pragma once

#include "Renderwerk/Core/CoreAPI.hpp"
#include "Renderwerk/Core/PrimitiveTypes.hpp"
#include "Renderwerk/Core/Containers/String.hpp"

class ENGINE_API FHashUtility
{
public:
	/**
	 * Hashes the given string. Implementation based on https://gist.github.com/hwei/1950649d523afd03285c
	 * @param InString The string to hash.
	 * @return The hash of the data.
	 */
	[[nodiscard]] static constexpr uint32 HashString(const FStringView InString)
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

	[[nodiscard]] static constexpr size64 CombineHash(const uint64 First, const uint64 Second)
	{
		size64 Hash = FNVOffsetBasis;
		Hash ^= First;
		Hash *= FNVPrime;
		Hash ^= Second;
		Hash *= FNVPrime;
		return Hash;
	}

private:
	static constexpr uint32 FNVPrime = 16777619u;
	static constexpr uint32 FNVOffsetBasis = 2166136261u;
};

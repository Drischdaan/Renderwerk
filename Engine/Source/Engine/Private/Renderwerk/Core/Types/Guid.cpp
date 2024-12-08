#include "pch.h"

#include "Renderwerk/Core/Types/Guid.h"

#include <random>

namespace
{
	std::random_device GRandomDevice;
	std::mt19937_64 GRandomEngine(GRandomDevice());
	std::uniform_int_distribution<uint64> GRandomDistribution;
}

const FGuid InvalidGuid = FGuid(INVALID_GUID_ID);

FGuid::FGuid()
	: Id(GRandomDistribution(GRandomEngine))
{
}

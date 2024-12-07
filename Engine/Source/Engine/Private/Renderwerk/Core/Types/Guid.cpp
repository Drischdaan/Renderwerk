#include "pch.h"

#include "Renderwerk/Core/Types/Guid.h"

#include <random>

enum
{
	INVALID_GUID_ID = 0
};

static std::random_device GRandomDevice;
static std::mt19937_64 GRandomEngine(GRandomDevice());
static std::uniform_int_distribution<uint64> GRandomDistribution;

const FGuid InvalidGuid = FGuid(INVALID_GUID_ID);

FGuid::FGuid()
	: Id(GRandomDistribution(GRandomEngine))
{
}

FGuid::FGuid(const uint64 InId)
	: Id(InId)
{
}

FGuid::~FGuid() = default;

bool8 FGuid::IsValid() const
{
	return Id != INVALID_GUID_ID;
}

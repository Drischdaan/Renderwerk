#pragma once

#include "Renderwerk/Core/CoreMinimal.h"

namespace Vulkan
{
	template <typename TStructureInfo>
	[[nodiscard]] ENGINE_API TStructureInfo CreateStructure(TStructureInfo Instance = {});
}

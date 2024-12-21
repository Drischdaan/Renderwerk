#pragma once

#include "Renderwerk/Graphics/GraphicsCommon.h"

namespace VulkanUtility
{
	[[nodiscard]] uint32 GetInstanceVersion();
	[[nodiscard]] TVector<FString> GetInstanceLayers();
	[[nodiscard]] TVector<FString> GetInstanceExtensions();
}

#pragma once

#include <nlohmann/json.hpp>

#define DEFINE_JSON_SERIALIZATION(Type, ...) NLOHMANN_DEFINE_TYPE_INTRUSIVE(Type, __VA_ARGS__)

using FJsonObject = nlohmann::json;

namespace JSON
{
	template <typename T>
	[[nodiscard]] FJsonObject Serialize(const T& Object)
	{
		return Object;
	}

	template <typename T>
	[[nodiscard]] T Deserialize(const FJsonObject& Json)
	{
		return Json.get<T>();
	}
}

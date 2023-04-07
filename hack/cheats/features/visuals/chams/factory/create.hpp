#pragma once

#include "../mat.hpp"

#include <string_view>

class IMaterial;

struct MaterialData
{
	std::string_view name;
	std::string_view key;
	std::string_view buffer;
};

enum class CreationType
{
	BY_BUFFER,
	FROM_STRING
};

// wrappers for safe material handling
namespace material
{
	namespace factory
	{
		IMaterial* createMaterial(const MaterialData& data, CreationType type);
	}
}
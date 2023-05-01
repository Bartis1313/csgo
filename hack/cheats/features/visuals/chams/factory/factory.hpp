#pragma once

#include <string_view>

class IMaterial;

enum class CreationType
{
	BY_BUFFER,
	FROM_STRING
};

// extenmd if specific values are needed
enum class MaterialTypes
{
	DEFAULT,
	GLOW
};

struct MaterialData
{
	std::string name{ };
	std::string key{ };
	std::string buffer{ };

	CreationType createType{ CreationType::BY_BUFFER };
	MaterialTypes materialType{ MaterialTypes::DEFAULT };

	IMaterial* material{ };
};

// wrappers for safe material handling
namespace material
{
	namespace factory
	{
		IMaterial* createMaterial(const MaterialData& data);
		void shutdown();

		inline std::vector<MaterialData> materials;
	}
}
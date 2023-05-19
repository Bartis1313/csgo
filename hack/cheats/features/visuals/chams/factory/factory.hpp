#pragma once

#include <string_view>

class IMaterial;
class ITexture;

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
		IMaterial* findMaterial(char const* name, const char* group, bool complain = true, const char* prefix = nullptr);
		ITexture* findTexture(char const* name, const char* group, bool complain = true, int additionalFlags = 0);
		void shutdown();

		inline std::vector<MaterialData> materials;
	}
}
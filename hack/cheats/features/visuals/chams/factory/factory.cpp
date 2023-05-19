#include "factory.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/KeyValuesSys.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/console/console.hpp>

#include <cheats/hooks/drawModelExecute.hpp>

namespace
{
	struct MaterialsShutdown : hooks::DrawModelExecute
	{
		MaterialsShutdown()
		{
			this->registerShutdown(material::factory::shutdown);
		}
	} materialShutdown;
}

IMaterial* material::factory::createMaterial(const MaterialData& data)
{
	KeyValues* keys{ nullptr };
	if (data.createType == CreationType::BY_BUFFER)
		keys = new KeyValues{ data.key.c_str() };
	else
	{
		constexpr auto toRemove = [](char c) { return c == '\n' || c == '\t'; };

		std::string fixedBuffer{ data.buffer };
		fixedBuffer.erase(std::remove_if(fixedBuffer.begin(), fixedBuffer.end(), toRemove), fixedBuffer.end());

		keys = KeyValues::fromString(data.key.c_str(), fixedBuffer.c_str());
	}
	assert(keys);

	const auto material = memory::interfaces::matSys->createMaterial(data.name.data(), keys);

	assert(material);
	assert(material->isError() == false); // handle on own?

	material->addRefCount();

	MaterialData dataToPush{ data };
	dataToPush.material = material;

	materials.push_back(dataToPush);

	console::debug("Added {} material", data.name);

	return material;
}

IMaterial* material::factory::findMaterial(char const* name, const char* group, bool complain, const char* prefix)
{
	IMaterial* mat = memory::interfaces::matSys->findMaterial(name, group, complain, prefix);
	assert(mat);

	return mat;
}

ITexture* material::factory::findTexture(char const* name, const char* group, bool complain, int additionalFlags)
{
	ITexture* tex = memory::interfaces::matSys->findTexture(name, group, complain, additionalFlags);
	assert(tex);

	return tex;
}

void material::factory::shutdown()
{
	for (const auto& materialData : materials)
	{
		materialData.material->removeRefCount();
	}
}
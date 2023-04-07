#include "create.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/KeyValuesSys.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/console/console.hpp>

IMaterial* material::factory::createMaterial(const MaterialData& data, CreationType type)
{
	const auto keys = type == CreationType::BY_BUFFER
		? new KeyValues{ data.key.data() }
	: KeyValues::fromString(data.key.data(), data.buffer.data()); // amazing formatting VS

	assert(keys);

	const auto material = memory::interfaces::matSys->createMaterial(data.name.data(), keys);

	assert(material);
	assert(material->isError() == false); // handle on own?

	material->addRefCount();

	console::debug("Added {} material", data.name);

	return material;
}
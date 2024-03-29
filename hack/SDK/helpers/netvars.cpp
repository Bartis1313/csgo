#include "netvars.hpp"

#include <SDK/Recv.hpp>
#include <SDK/IBaseClientDll.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/config.hpp>
#include <utilities/utilities.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>

#include <array>
#include <ranges>
#include <fstream>
#include <unordered_map>

namespace netvars
{
	std::unordered_map<std::string_view, RecvTable*> m_Tables{ };
	std::ofstream file{ };

	[[nodiscard]] uintptr_t getProp(const std::string_view tableName, const std::string_view propName, RecvProp** prop = nullptr);
	[[nodiscard]] uintptr_t getProp(RecvTable* recvTable, const std::string_view propName, RecvProp** prop = nullptr);
	[[nodiscard]] RecvTable* getTable(const std::string_view tableName);
	[[nodiscard]] std::string getType(RecvProp* recvTable);
	void dump(RecvTable* recvTable);
}

void netvars::init()
{
	auto clientClass = memory::interfaces::client->getAllClasses();
	if (!clientClass)
		return;

	while (clientClass)
	{
		auto recvTable = clientClass->m_recvTable;
		m_Tables.emplace(recvTable->m_netTableName, recvTable);

		clientClass = clientClass->m_next;
	}

	dump();

	console::debug("Netvars inited");
}

uintptr_t netvars::getNetvar(const std::string_view tableName, const std::string_view propName)
{
	auto offset = getProp(tableName, propName);
	if (!offset)
		return 0;

	return offset;
}

uintptr_t netvars::getProp(const std::string_view tableName, const std::string_view propName, RecvProp** prop)
{
	auto recvTable = getTable(tableName);
	if (!recvTable)
		return 0;

	auto offset = getProp(recvTable, propName, prop);
	if (!offset)
		return 0;

	return offset;
}

uintptr_t netvars::getProp(RecvTable* recvTable, const std::string_view propName, RecvProp** prop)
{
	uintptr_t extraOffset = 0;

	for (auto i : std::views::iota(0, recvTable->m_propsNum))
	{
		auto recvProp = &recvTable->m_props[i];
		auto recvChild = recvProp->m_dataTable;

		if (recvChild && (recvChild->m_propsNum > 0))
		{
			auto temp = getProp(recvChild, propName, prop);

			if (temp)
				extraOffset += (recvProp->m_offset + temp);
		}

		if (std::string_view name = recvProp->m_varName; name != propName)
			continue;

		if (prop)
			*prop = recvProp;

		return (recvProp->m_offset + extraOffset);
	}

	return extraOffset;
}

RecvTable* netvars::getTable(const std::string_view tableName)
{
	if (m_Tables.empty())
		return nullptr;

	for(const auto& [name, recv] : m_Tables)
	{
		if (name == tableName)
			return recv;
	}

	return nullptr;
}

std::string netvars::getType(RecvProp* recvTable)
{
	static std::unordered_map<SendPropType, std::string> props =
	{
		{ SendPropType::DPT_INT, "int"},
		{ SendPropType::DPT_FLOAT, "float"},
		{ SendPropType::DPT_VECTOR, "Vec3"},
		{ SendPropType::DPT_VECTOR2D, "Vec2"},
		{ SendPropType::DPT_STRING, "char"},
		{ SendPropType::DPT_ARRAY, "array"},
		{ SendPropType::DPT_DATATABLE, "datatable (void*)"},
		{ SendPropType::DPT_INT64, "__int64"},
	};

	// buffers
	if (auto type = recvTable->m_recvType; type == DPT_STRING)
		return std::format("{}[{}]", props.at(recvTable->m_recvType), recvTable->m_stringBufferSize);
	else if(type == DPT_ARRAY)
		return std::format("{}[{}]", props.at(recvTable->m_recvType), recvTable->m_elements);

	return props.at(recvTable->m_recvType);
}

void netvars::dump(RecvTable* recvTable)
{
	for (auto i : std::views::iota(0, recvTable->m_propsNum))
	{
		auto recvProp = &recvTable->m_props[i];

		if (!recvProp)
			continue;

		std::string_view recvName = recvProp->m_varName;
		if (recvName.find("baseclass") != std::string::npos)
			continue;

		if (::isdigit(recvProp->m_varName[0]))
			continue;

		file << std::format("[{}::{}] -> 0x{:X} -> ({})",
			recvTable->m_netTableName,
			recvName, recvProp->m_offset,
			getType(recvProp))
			<< '\n';

		if (recvProp->m_dataTable)
			dump(recvProp->m_dataTable);
	}
}

#include <SDK/datamap.hpp>

uintptr_t netvars::getDataMap(DataMap_t* map, const std::string_view name)
{
	while (map)
	{
		for (auto i : std::views::iota(0, map->m_dataFields))
		{
			const auto descriptor = map->m_dataDescription[i];

			if (descriptor.m_name == nullptr)
				continue;

			if (name == std::string_view{ descriptor.m_name })
				return descriptor.m_offset[TD_OFFSET_NORMAL];

			if (descriptor.m_type == FIELD_EMBEDDED)
			{
				if (descriptor.m_dataMap)
				{
					if (const auto offset = getDataMap(descriptor.m_dataMap, name))
						return offset;
				}
			}
		}
		map = map->m_baseMap;
	}

	return 0U;
}

#include <utilities/simpleTimer.hpp>

void netvars::dump()
{
	file = std::ofstream{ api::getHackPath() / "netvarsDump.txt" };
	file << std::format("Netvars from: {}", utilities::getTime()) << "\n\n";

	TimeCount timer{};
	size_t count{ 0U };

	auto client = memory::interfaces::client->getAllClasses();
	do {
		const auto recvTable = client->m_recvTable;
		dump(recvTable);
		client = client->m_next;
		++count;
	} while (client);

	timer.end();

	file << '\n' << std::format("Found {} netvars\nFinished in {:.2f} secs", count, timer.getTime());
	file.close();
}
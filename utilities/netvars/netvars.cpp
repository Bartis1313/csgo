#include "netvars.hpp"

#include <SDK/Recv.hpp>
#include <SDK/IBaseClientDll.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/config.hpp>
#include <utilities/utilities.hpp>
#include <utilities/tools/wrappers.hpp>

#include <array>
#include <ranges>

void NetvarManager::init()
{
	m_Tables.clear();

	auto clientClass = interfaces::client->getAllClasses();
	if (!clientClass)
		return;

	while (clientClass)
	{
		auto recvTable = clientClass->m_recvTable;
		m_Tables.emplace(std::string{ recvTable->m_netTableName }, recvTable);

		clientClass = clientClass->m_next;
	}	
}

uintptr_t NetvarManager::getNetvar(const char* tableName, const char* propName) const
{
	auto offset = getProp(tableName, propName);
	if (!offset)
		return 0;

	return offset;
}

uintptr_t NetvarManager::getProp(const char* tableName, const char* propName, RecvProp** prop) const
{
	auto recvTable = getTable(tableName);
	if (!recvTable)
		return 0;

	auto offset = getProp(recvTable, propName, prop);
	if (!offset)
		return 0;

	return offset;
}

uintptr_t NetvarManager::getProp(RecvTable* recvTable, const char* propName, RecvProp** prop) const
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

RecvTable* NetvarManager::getTable(const char* tableName) const
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

static std::string propToStr(SendPropType type)
{
	// not needed, just for always being sure
	if (type >= DPT_NUMSendPropTypes)
		return "";

	static std::array<std::string, DPT_NUMSendPropTypes> arr =
	{
		XOR("int"), XOR("float"),
		XOR("Vector"), XOR("Vector2D"),
		XOR("string*"), XOR("array"),
		XOR("datatable (void*)"), XOR("__int64")
	};

	return arr.at(type);
}

std::string NetvarManager::getType(RecvProp* recvTable) const
{
	return propToStr(recvTable->m_recvType);
}

void NetvarManager::dump(RecvTable* recvTable)
{
	for (auto i : std::views::iota(0, recvTable->m_propsNum))
	{
		auto recvProp = &recvTable->m_props[i];

		if (!recvProp)
			continue;

		std::string recvName = recvProp->m_varName;

		if (recvName.find(XOR("baseclass")) != std::string::npos)
			continue;

		if (::isdigit(recvProp->m_varName[0]))
			continue;

		file << FORMAT(XOR("[{}::{}] -> 0x{:X} -> ({})"),
			recvTable->m_netTableName,
			recvName, recvProp->m_offset,
			getType(recvProp))
			<< '\n';

		if (recvProp->m_dataTable)
			dump(recvProp->m_dataTable);
	}
}

#include "../simpleTimer.hpp"

void NetvarManager::dump()
{
	file = std::ofstream{ config.getPathForSave(XOR("netvarsDump.txt")), std::ofstream::out | std::ofstream::trunc };
	file << FORMAT(XOR("Netvars from: {}"), utilities::getTime()) << "\n\n";

	TimeCount timer{};
	auto client = interfaces::client->getAllClasses();
	do {
		const auto recvTable = client->m_recvTable;
		dump(recvTable);
		client = client->m_next;
	} while (client);
	timer.end();

	file << '\n' << FORMAT(XOR("Finished in {:.4f} secs"), timer.getSec());

	file.close();
}
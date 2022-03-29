#include "netvars.hpp"

#include "../../SDK/Recv.hpp"
#include "../../SDK/IBaseClientDll.hpp"
#include "../../SDK/ClientClass.hpp"
#include "../../SDK/interfaces/interfaces.hpp"
#include "../../config/config.hpp"

#include <iomanip>

void NetvarManager::init()
{
	m_Tables.clear();

	auto clientClass = interfaces::client->getAllClasses();
	if (!clientClass)
		return;

	while (clientClass)
	{
		auto recvTable = clientClass->m_recvTable;
		m_Tables.emplace(std::string(recvTable->m_netTableName), recvTable);

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

	for (int i = 0; i < recvTable->m_propsNum; i++)
	{
		auto recvProp = &recvTable->m_props[i];
		auto recvChild = recvProp->m_dataTable;

		if (recvChild && (recvChild->m_propsNum > 0))
		{
			auto temp = getProp(recvChild, propName, prop);

			if (temp)
				extraOffset += (recvProp->m_offset + temp);
		}

		if (strcmp(recvProp->m_varName, propName))
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
		if (!strcmp(name.c_str(), tableName))
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
	for (int i = 0; i < recvTable->m_propsNum; i++)
	{
		auto recvProp = &recvTable->m_props[i];

		if (!recvProp)
			continue;

		std::string recvName = recvProp->m_varName;

		if (recvName.find(XOR("m_")) == std::string::npos) // not starting with m_
			continue;

		// offsets for set are hardcoded- simply no need for this
		file << std::setfill(' ') << std::setw(30) << recvTable->m_netTableName
			<< std::setfill('_') << std::setw(80 - recvName.length()) << recvName
			<< std::setfill(' ') << std::setw(recvName.length() + 2) << " -> "
			<< "0x" << std::setfill('0') << std::setw(8) << std::hex
			<< std::uppercase << recvProp->m_offset
			<< " -> " << getType(recvProp) << "\n";

		if (recvProp->m_dataTable)
			dump(recvProp->m_dataTable);
	}
}

void NetvarManager::dump()
{
	file = std::ofstream{ config.getPathForSave(XOR("netvarsDump.txt")), std::ofstream::out | std::ofstream::trunc};

	file << XOR("Netvars from: ") << utilities::getTime() << "\n\n";

	auto client = interfaces::client->getAllClasses();
	do {
		const auto recvTable = client->m_recvTable;
		dump(recvTable);
		client = client->m_next;
	} while (client);

	file.close();
}
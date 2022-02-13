#include <iomanip>
#include "netvars.hpp"
#include "../../SDK/interfaces/interfaces.hpp"
#include "../../config/config.hpp"

void NetvarManager::init()
{
	m_Tables.clear();

	auto clientClass = interfaces::client->getAllClasses();
	if (!clientClass)
		return;

	do {
		auto recvTable = clientClass->m_recvTable;
		m_Tables.emplace(std::string(recvTable->m_netTableName), recvTable);

		clientClass = clientClass->m_next;
	} while (clientClass);
}

uintptr_t NetvarManager::getNetvar(const char* tableName, const char* propName)
{
	auto offset = getProp(tableName, propName);
	if (!offset)
		return 0;

	return offset;
}

uintptr_t NetvarManager::getProp(const char* tableName, const char* propName, RecvProp** prop)
{
	auto recvTable = getTable(tableName);
	if (!recvTable)
		return 0;

	auto offset = getProp(recvTable, propName, prop);
	if (!offset)
		return 0;

	return offset;
}

uintptr_t NetvarManager::getProp(RecvTable* recvTable, const char* propName, RecvProp** prop)
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

RecvTable* NetvarManager::getTable(const char* tableName)
{
	if (m_Tables.empty())
		return 0;

	for(auto table : m_Tables)
	{
		if (!strcmp(table.first.c_str(), tableName))
			return table.second;
	}

	return 0;
}

std::string NetvarManager::getType(RecvProp* recvTable)
{
	switch (recvTable->m_recvType)
	{
	case DPT_Int:
		return XOR("int");
	case DPT_Float:
		return XOR("float");
	case DPT_Vector:
		return XOR("Vector");
	case DPT_VectorXY:
		return XOR("Vector2D");
	case DPT_String:
		return XOR("char*");
	case DPT_Array:
		return XOR("array*");
	case DPT_DataTable:
		return XOR("void*");
	case DPT_Int64:
		return XOR("__int64");
	default:
		return "";
	}
}

void NetvarManager::dump(RecvTable* recvTable)
{
	for (int i = 0; i < recvTable->m_propsNum; i++)
	{
		auto recvProp = &recvTable->m_props[i];

		if (!recvProp)
			continue;

		std::string recvName = recvProp->m_varName;

		if (!recvName.find(XOR("baseclass")))
			continue;

		if (recvName.find(XOR("m_")))
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
	} while (client != 0);

	file.close();
}
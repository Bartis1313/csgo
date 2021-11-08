#include "Item.hpp"
#pragma warning(disable: 26495)

int Item::count = 0;

Item::Item(const std::string& id, int* ref, std::array<int, 2> limits) : 
	m_ID{std::cref(id)},
	m_iVal{ref},
	m_Limits{limits}
{
	m_isInt = true;
	count++;
}

Item::Item(const std::string& id, bool* ref) :
	m_ID{std::cref(id)},
	m_bVal{ref}
{
	m_isbool = true;
	count++;
}

Item::Item(const std::string& id, int* ref, const std::vector<std::string>& vec) :
	m_ID{std::cref(id)},
	m_Options{std::cref(vec)},
	m_iVal{ref}
{
	m_isVec = true;
	count++;
}

void Item::chnageIntRefp()
{
	if (*m_iVal < m_Options.size())
		*m_iVal += 1;
	if (*m_iVal >= m_Options.size())
		*m_iVal = 0;
}

void Item::chnageIntRefm()
{
	if (*m_iVal < m_Options.size())
		*m_iVal -= 1;
	if (*m_iVal <= -1)
		*m_iVal = m_Options.size() - 1;
}

void Item::chnageAddableRefp()
{
	if (*m_iVal < m_Limits.at(1))
		*m_iVal += 1;
}

void Item::chnageAddableRefm()
{
	if (*m_iVal > m_Limits.at(0))
		*m_iVal -= 1;
}

void Item::changeBoolRef()
{
	*m_bVal = !*m_bVal;
}


#include "Item.hpp"
#pragma warning(disable: 26495)

int Item::count = 0;

Item::Item(const std::string& id, int* ref, const std::pair<float, float>& limits) :
	m_ID{ id },
	m_iVal{ ref },
	m_Limits{ limits }
{
	m_isInt = true;
	count++;
}

Item::Item(const std::string& id, float* ref, const std::pair<float, float>& limits) :
	m_ID{ id },
	m_fVal{ ref },
	m_Limits{ limits }
{
	m_isFloat = true;
	count++;
}

Item::Item(const std::string& id, bool* ref) :
	m_ID{ id },
	m_bVal{ ref }
{
	m_isbool = true;
	count++;
}

Item::Item(const std::string& id, int* ref, const std::vector<std::string>& vec) :
	m_ID{ id },
	m_Options{ vec },
	m_iVal{ ref }
{
	m_isVec = true;
	count++;
}

// about plus and minus - this would never be handled like this. It is because there had to be a trick to manipulate indexes and not giving OFF string

void Item::changeVectorIndexByPlus()
{
	if (*m_iVal < m_Options.size() + 1 || *m_iVal == -1)
	{
		*m_iVal += 1;
	}	

	if (*m_iVal >= m_Options.size())
	{
		*m_iVal = -1;
	}
}

void Item::changeVectorIndexByMinus()
{
	if (*m_iVal < m_Options.size() + 1 || *m_iVal == -1)
	{
		*m_iVal -= 1;
	}

	if (*m_iVal < -1)
	{
		*m_iVal = m_Options.size() - 1;
	}
}

void Item::chnageAddableByPlus()
{
	if (m_isInt)
	{
		if (*m_iVal < m_Limits.second)
			*m_iVal += 1;
	}
	else if (m_isFloat)
	{
		if (*m_fVal < m_Limits.second)
			*m_fVal += 1.0f;
	}
}

void Item::chnageAddableByMinus()
{
	if (m_isInt)
	{
		if (*m_iVal > m_Limits.first)
			*m_iVal -= 1;
	}
	else if (m_isFloat)
	{
		if (*m_fVal > m_Limits.first)
			*m_fVal -= 1.0f;
	}
}

void Item::changeBool()
{
	*m_bVal = !*m_bVal;
}


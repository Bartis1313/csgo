#pragma once
#include <string>
#include <vector>
#include <array>
//#include <variant>

class Item
{
public:
	// for me this variant is overkill there, this is slower than normal UNSAFE type checking, but here - whatever
	//using Types = std::variant<std::vector<std::string>, std::array<int, 2>>;
	//using TypesRef = std::variant<int*, bool*>;


	Item() = delete;
	// arr[0] - min, arr[1] - max
	Item(const std::string& id, int* ref, std::array<int, 2>);
	Item(const std::string& id, bool* ref);
	Item(const std::string& id, int* ref, const std::vector<std::string>& vec);
	// constructor with variant example, put it in cpp file if you want to use/practise it
	/*Item(const std::string& id, TypesRef ref)
	{
		if (std::holds_alternative<bool*>(ref))
		{
			m_ID = std::cref(id);
			m_isbool = true;
			m_bVal = std::get<bool*>(ref);
		}
	}
	Item(const std::string& id, TypesRef ref, Types type)
	{
		if (std::holds_alternative<int*>(ref))
		{
			if (std::holds_alternative<std::vector<std::string>>(type))
			{
				m_ID = std::cref(id);
				m_isVec = true;
				m_iVal = std::get<int*>(ref);
			}
			if (std::holds_alternative<std::array<int, 2>>(type))
			{
				m_ID = std::cref(id);
				m_isInt = true;
				m_iVal = std::get<int*>(ref);
			}
		}
	}*/

	bool isBoolOption() const { return m_isbool; }
	bool isVectorOption() const { return m_isVec; }
	bool isAddable() const { return m_isInt; }
	// plus
	void chnageIntRefp();
	// minus
	void chnageIntRefm();
	
	void changeBoolRef();
	// plus
	void chnageAddableRefp();
	// minus
	void chnageAddableRefm();
	
	int getInt() const { return *m_iVal; }
	bool getBool() const { return *m_bVal; }
	std::string getName() const { return m_ID; }
	std::vector<std::string> getVec() const { return m_Options; }

	static void destroyCount() { count = 0; }
	static int count;
private:
	std::string m_ID;
	std::vector<std::string> m_Options;

	bool m_isInt = false;
	bool m_isbool = false;
	bool m_isVec = false;

	std::array<int, 2> m_Limits;

	bool* m_bVal;
	int* m_iVal;
};

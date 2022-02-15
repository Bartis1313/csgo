#pragma once
#include <string>
#include <vector>
#include <utility>

// x88 item class - this could be done better by templates probably
class Item
{
public:
	Item() = delete;
	// pair.first - min, pair.second - max
	Item(const std::string& id, int* ref, const std::pair<float, float>& limits);
	// pair.first - min, pair.second - max
	Item(const std::string& id, float* ref, const std::pair<float, float>& limits);
	Item(const std::string& id, bool* ref);
	Item(const std::string& id, int* ref, const std::vector<std::string>& vec);

	_NODISCARD bool isBoolOption() const { return m_isbool; }
	_NODISCARD bool isVectorOption() const { return m_isVec; }
	_NODISCARD bool isIntOption() const { return m_isInt; }
	_NODISCARD bool isFloatOption() const { return m_isFloat; }
	// plus
	void changeVectorIndexByPlus();
	// minus
	void changeVectorIndexByMinus();
	// bool = !bool
	void changeBool();
	// plus
	void chnageAddableByPlus();
	// minus
	void chnageAddableByMinus();

	_NODISCARD int getInt() const { return *m_iVal; }
	_NODISCARD float getFloat() const { return *m_fVal; }
	_NODISCARD bool getBool() const { return *m_bVal; }
	_NODISCARD std::string getName() const { return m_ID; }
	_NODISCARD std::vector<std::string> getVec() const { return m_Options; }

	static void destroyCount() { count = 0; }
	static int count;
private:
	std::string m_ID;
	std::vector<std::string> m_Options;

	bool m_isInt = false;
	bool m_isFloat = false;
	bool m_isbool = false;
	bool m_isVec = false;

	std::pair<float, float> m_Limits;

	bool* m_bVal;
	int* m_iVal;
	float* m_fVal;
};

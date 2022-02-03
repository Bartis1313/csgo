#pragma once
#include "../Group/Group.hpp"
#include "../CheckBox/CheckBox.hpp"
#include "../Button/Button.hpp"
#include "../ComboBox/ComboBox.hpp"
#include "../MultiBox/MultiBox.hpp"
#include "../TextInput/TextInput.hpp"
#include "../KeyHolder/KeyHolder.hpp"
#include "../Slider/Slider.hpp"
#include "../ColorPicker/ColorPicker.hpp"
#include "../Lining/Lining.hpp"

namespace GUI
{
	// tabs, are unique as they rely on groups
	class Tab final
	{
	public:
		Tab(const std::string& name);

		_NODISCARD Group* addGroup(const std::string& name, Lining lining = Lining::AUTO_PADDING);
		// this will only draw CONTENT on selected tab
		void draw(Vector2D* pos);
		void setParent(Menu* addr) { m_parent = addr; }
		void shutdown();

		_NODISCARD std::string getName() const { return m_name; }
		_NODISCARD Menu* getParent() const { return m_parent; }
	private:
		Menu* m_parent;
		std::string m_name;
		Lining m_lining;
		std::vector<Group*> m_groups;
	};
}
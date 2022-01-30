#pragma once
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
	// represents some set of features
	class Group final
	{
	public:
		// should be only called in tab
		Group(const std::string& name, Lining lining);

		void addCheckBox(const std::string& title, bool* feature);
		void addButton(const std::string& title, const std::function<void()>& lambda);
		void addComboBox(const std::string& title, const std::vector<std::string>& names, int* option);
		void addMultiBox(const std::string& title, const std::vector<std::string>& names, std::vector<bool>* feature);
		void addTextInput(const std::string& title, std::string* text);
		void addKeyHolder(const std::string& title, int* key);
		void addSlider(const std::string& title, std::pair<float, float> minMax, float* value);
		void addColorPicker(const std::string& title, Color* color);

		void draw(Vector2D* pos, Menu* parent);
	private:
		std::string m_name;
		Tab* m_parent;
		// add it later, to overload or do smth
		int m_height;
		// useless ?
		int m_column;
		std::vector<Element*> m_Elements;
		Lining m_lining;
	};
}
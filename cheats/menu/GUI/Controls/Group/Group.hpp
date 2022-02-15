#pragma once
#include "../CheckBox/CheckBox.hpp"
#include "../Button/Button.hpp"
#include "../ComboBox/ComboBox.hpp"
#include "../MultiBox/MultiBox.hpp"
#include "../Text/Text.hpp"
#include "../TextInput/TextInput.hpp"
#include "../KeyHolder/KeyHolder.hpp"
#include "../Slider/Slider.hpp"
#include "../ColorPicker/ColorPicker.hpp"
#include "../Lining/Lining.hpp"
#include "../HelpMarker/HelpMarker.hpp"

class Color;

namespace GUI
{
	// represents some set of features
	class Group final
	{
	public:
		// should be only called in tab
		Group(const std::string& name, Lining lining);

		CheckBox* addCheckBox(const std::string& title, bool* feature);
		Button* addButton(const std::string& title, const std::function<void()>& lambda);
		ComboBox* addComboBox(const std::string& title, const std::vector<std::string>& names, int* option);
		MultiBox* addMultiBox(const std::string& title, const std::vector<std::string>& names, std::vector<bool>* feature);
		TextInput* addTextInput(const std::string& title, std::string* text);
		KeyHolder* addKeyHolder(int* key);
		Slider* addSlider(const std::string& title, std::pair<float, float> minMax, float* value);
		ColorPicker* addColorPicker(Color* color);
		HelpMarker* addHelpMarker(const std::string& text);
		Text* addText(const std::string& text);

		void draw(Vector2D* pos, Menu* parent);
		void shutdown();
	private:
		std::string m_name;
		// add it later, to overload or do smth
		int m_height;
		// useless ?
		int m_column;
		std::vector<Element*> m_Elements;
		Lining m_lining;
	};
}
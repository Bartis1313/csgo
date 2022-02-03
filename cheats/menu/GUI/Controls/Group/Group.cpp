#include "Group.hpp"

GUI::Group::Group(const std::string& name, Lining lining)
	: m_name{ name }, m_lining{ lining }
{}

void GUI::Group::addCheckBox(const std::string& title, bool* feature)
{
	auto t = new CheckBox(title, feature);
	m_Elements.emplace_back(t);
}

void GUI::Group::addButton(const std::string& title, const std::function<void()>& lambda)
{
	auto t = new Button(title, lambda);
	m_Elements.emplace_back(t);
}

void GUI::Group::addComboBox(const std::string& title, const std::vector<std::string>& names, int* option)
{
	auto t = new ComboBox(title, names, option);
	m_Elements.emplace_back(t);
}

void GUI::Group::addMultiBox(const std::string& title, const std::vector<std::string>& names, std::vector<bool>* feature)
{
	auto t = new MultiBox(title, names, feature);
	m_Elements.emplace_back(t);
}

void GUI::Group::addTextInput(const std::string& title, std::string* text)
{
	auto t = new TextInput(title, text);
	m_Elements.emplace_back(t);
}

void GUI::Group::addKeyHolder(int* key)
{
	auto t = new KeyHolder(key);
	m_Elements.emplace_back(t);
}

void GUI::Group::addSlider(const std::string& title, std::pair<float, float> minMax, float* value)
{
	auto t = new Slider(title, minMax, value);
	m_Elements.emplace_back(t);
}

void GUI::Group::addColorPicker(Color* color)
{
	auto t = new ColorPicker(color);
	m_Elements.emplace_back(t);
}

void GUI::Group::addHelpMarker(const std::string& text)
{
	auto t = new HelpMarker(text);
	m_Elements.emplace_back(t);
}

// TODO: add some proper rect, preferably add height as argument or overload this
void GUI::Group::draw(Vector2D* pos, Menu* parent)
{
	if (m_lining == Lining::PAD)
	{
		pos->x += GROUPX_PAD;
		pos->y = parent->getMenuDrawHeight();
	}

	render::text(pos->x, pos->y, fonts::menuFont, m_name, false, Colors::White);
	pos->y += render::getTextSizeXY(fonts::menuFont, m_name).y + 5;

	for (const auto& el : m_Elements)
		el->draw(pos, parent);

	pos->y += GROUPY_PAD;
}

void GUI::Group::shutdown()
{
	for (const auto& el : m_Elements)
	{
		if (el->getType() == COLOR_PICKER)
			el->destroy();
	}
}
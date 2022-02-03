#include "Tab.hpp"

GUI::Tab::Tab(const std::string& name)
	: m_name{ name }
{}

GUI::Group* GUI::Tab::addGroup(const std::string& name, Lining lining)
{
	auto t = new Group(name, lining);
	m_groups.emplace_back(t);
	return t;
}

void GUI::Tab::draw(Vector2D* pos)
{
	for (const auto& el : m_groups)
	{
		el->draw(pos, m_parent);
	}
}

void GUI::Tab::shutdown()
{
	for (const auto& el : m_groups)
	{
		el->shutdown();
	}
}
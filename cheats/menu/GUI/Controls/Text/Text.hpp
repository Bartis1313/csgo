#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	class Text : public Element
	{
	public:
		Text(const std::string& title);
		virtual void draw(Vector2D* pos, Menu* parent, bool SkipCall = false) override;
	private:
		std::string m_title;
	};
}
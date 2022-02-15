#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	class HelpMarker : public Element
	{
	public:
		HelpMarker(const std::string& text);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
	private:
		std::string m_text;
		std::string m_helpString = XOR("(?)");
	};
}
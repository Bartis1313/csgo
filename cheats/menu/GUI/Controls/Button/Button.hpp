#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// button, it has the option to turn off/on the feature
	class Button : public Element
	{
	public:
		Button(const std::string& title, const std::function<void()>& lambda);
		virtual void draw(Vector2D* pos, Menu* parent, bool SkipCall = false) override;
	private:
		std::string m_title;
		std::function<void()> m_lamda;
	};
}
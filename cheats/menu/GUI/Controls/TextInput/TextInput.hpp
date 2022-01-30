#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// write anything to the box
	class TextInput : public Element
	{
	public:
		TextInput(const std::string& title, const int x, const int y, const int width, const int height,
			const Color& color, const Color& secColor, std::string* text);
		TextInput(const std::string& title, std::string* text);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		static void initTabs();
		_NODISCARD bool isActive() const;
	private:
		static std::array<std::string, 256> m_smallLetters;
		static std::array<std::string, 256> m_bigLetters;
		static bool m_inited;
		std::string m_title;
		bool m_active;
		Color m_color;
		Color m_secColor;
		std::string* m_text;
	};
}
#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// color picker with already made all colors by hsl looping. Alternatively you can draw it from file
	class ColorPicker : public Element
	{
	public:
		ColorPicker(const std::string& title, Color* color);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		bool isActive() const;
	private:
		static std::unique_ptr<Color[]> m_gradient;
		static std::unique_ptr<Color[]> m_alphaBar;
		static std::unique_ptr<Color[]> m_hueBar;

		static int m_gradientID;
		static int m_textureALPHAID;
		static int m_textureHUEID;
		std::string m_title;
		bool m_active;
		Color getColorFromPos(const int x, const int y);
		Color* m_colorNow;
	};
}
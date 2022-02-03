#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// color picker with already made all colors by hsl looping. Alternatively you can draw it from file
	class ColorPicker : public Element
	{
	public:
		ColorPicker(Color* color);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		_NODISCARD virtual bool isActive() override;
		virtual void destroy() override;
	private:
		// shouldn't be static for 2nd method
		std::unique_ptr<Color[]> m_gradient = nullptr;
		std::unique_ptr<Color[]> m_alphaBar = nullptr;
		std::unique_ptr<Color[]> m_hueBar = nullptr;

		int m_gradientID;
		int m_alphaID;
		int m_hueID;
		std::string m_title;
		bool m_active = false;
		// those both work like bitmap color detection
		Color getColorFromSpectrum(const int x, const int y);
		Color getColorFromHueBar(const int x, const int y);
		Color* m_colorNow;
		int mousePointX;
		int mousePointY;
		int m_levelHue = 0;
	};
}
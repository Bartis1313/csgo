#include "ColorPicker.hpp"
#include "../../../../globals.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

// because it's square and using this names as height or width can confuse why
#define COLOR_PICKER_SIZE COLORPICKER_HEIGHT
#define SLIDER_WIDTH 20

GUI::ColorPicker::ColorPicker(Color* color)
	: m_colorNow{ color }, m_alphaForColor{ m_colorNow->a() }
{
	setElement(COLORPICKER_WIDTH, COLORPICKER_HEIGHT);
	setPadding(0);
	m_type = COLOR_PICKER;
}

void GUI::ColorPicker::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	// old method can run this as static, because it same for every object
	/* 
	* if (!m_gradient)
	{
		m_gradient = std::make_unique<Color[]>(m_width * m_height);

		float hue = 0.0f;
		float saturation = 0.99f;
		float lightness = 1.0f;

		// loop through all gradient indexes to fill them accordingly
		for (int i = 0; i < m_width; i++)
		{
			for (int j = 0; j < m_height; j++)
			{
				*reinterpret_cast<Color*>(m_gradient.get() + j + i * m_width) = Color::hslToRGB(hue, saturation, lightness);

				hue += (1.0f / m_width);
			}

			lightness -= (1.0f / m_height);
			hue = 0.0f;
		}

		render::initNewTexture(m_gradientID, m_gradient.get(), m_width, m_height);
	}
	*/


	// first init, just pick any from those 3 bitmaps
	if (!m_gradient)
	{
		m_gradient = std::make_unique<Color[]>(m_width * m_height);
		m_hueBar = std::make_unique<Color[]>(m_width * SLIDER_WIDTH);
		//m_alphaBar = std::make_unique<Color[]>(COLOR_PICKER_SIZE * SLIDER_SIZE);

		// init the bars
		for (int i = 0; i < SLIDER_WIDTH; i++)
		{
			for (int j = 0; j < m_height; j++)
			{
				*reinterpret_cast<Color*>(m_hueBar.get() + i + j * SLIDER_WIDTH) =
					Color::fromHSB(j / 150.0f, 1.0f, 1.0f);
				//*reinterpret_cast<Color*>(m_alphaBar.get() + i + j * SLIDER_SIZE) =
				//	Color(0, 0, 0, j / static_cast<float>(COLOR_PICKER_SIZE) * 255.0f);
			}
		}

		for (int i = 0; i < m_width; i++)
		{
			for (int j = 0; j < m_height; j++)
			{
				*reinterpret_cast<Color*>(m_gradient.get() + i + j * m_width) =
					Color::fromHSB(Color::getHueFromColor(*m_colorNow),
						j / static_cast<float>(m_width), i / static_cast<float>(m_width));
			}
		}
		render::initNewTexture(m_gradientID, m_gradient.get(), m_width, m_height);
		render::initNewTexture(m_hueID, m_hueBar.get(), SLIDER_WIDTH, m_height);
		//render::initNewTexture(m_alphaID, m_alphaBar.get(), SLIDER_SIZE, COLOR_PICKER_SIZE);
	}

	// always draw it in the menu
	render::drawGradient(pos->x + PAD_TEXT_X + BOX_WIDTH + 5, pos->y + 2, 15, 8, Color(*m_colorNow, m_alphaForColor), Color(Colors::Black, m_alphaForColor), false, false);
	render::drawOutlineRect(pos->x + PAD_TEXT_X + BOX_WIDTH + 5, pos->y + 2- 1, 15 + 1, 8 + 1, Colors::Black);
	
	bool isInRangeButton = isMouseInRange(pos->x + PAD_TEXT_X + BOX_WIDTH + 5, pos->y, 15, 8);

	auto check = [this]() -> void
	{
		m_active = !m_active;
		m_toChange = false;
	};

	// whenever we click the main box to show list of features
	bool toSkip = false;

	if (isKeyPressed(VK_LBUTTON) && isInRangeButton)
	{
		if (m_toChange)
			check();
		toSkip = true;
	}
	else if (isInRangeButton)
	{
		m_toChange = true;
	}

	// why? idk, I already added the skipCall so you can draw it anywhere you want
	int x = parent->getX() + parent->getWidth() + parent->getThickness() + 7;
	int y = parent->getY() + 5;

	if (this->isActive())
	{
		if (!skipCall)
		{
			parent->skip(this, pos);
		}

		// to move from rect bit to draw
		x += 4;

		bool isInPickerRange = isMouseInRange(x, y - 5, m_width + SLIDER_WIDTH + SLIDER_WIDTH + 12, m_height + 10);

		if (isKeyPressed(VK_LBUTTON) && !(isInRangeButton || isInPickerRange) && !toSkip)
			m_active = false;

		// rects so it doesn't look raw
		render::drawFilledRect(x, y - 5, m_width + SLIDER_WIDTH + SLIDER_WIDTH + 12, m_height + 10, Color(50, 50, 50, 255));
		render::drawOutlineRect(x, y - 5, m_width + SLIDER_WIDTH + SLIDER_WIDTH + 12, m_height + 10, Colors::Black);
		render::drawOutlineRect(x + 1, y - 4, m_width + SLIDER_WIDTH + SLIDER_WIDTH + 10, m_height + 8, Color(80, 80, 80, 255));

		// to move from rect bit to draw
		x += 4;

		render::drawFromTexture(m_gradientID, x, y, m_width, m_height, Colors::White);
		render::drawFromTexture(m_hueID, x + m_width + 2, y, SLIDER_WIDTH, m_height, Colors::White);
		render::drawGradient(x + m_width + SLIDER_WIDTH + 4, y, SLIDER_WIDTH, m_height, Colors::White, Colors::Black, false);
		// should draw it better, easiest is to use gradients from surface
		//render::drawFromTexture(m_textureALPHAID, x + COLOR_PICKER_SIZE + SLIDER_SIZE, y, SLIDER_SIZE, m_height, Colors::White);

		// + 2 look up^
		bool isInHueRange = isMouseInRange(x + 2 + m_width, y, SLIDER_WIDTH, m_height);
		// + 4, same, just look up
		bool isInAlphaRange = isMouseInRange(x + m_width + SLIDER_WIDTH + 4, y, SLIDER_WIDTH, m_height);

		// change BROKEN pos, gotta fix it
		if (isKeyDown(VK_LBUTTON) && isMouseInRange(x, y, m_width, m_height))
		{
			*m_colorNow = Color(
				getColorFromSpectrum(globals::mouseX - x, globals::mouseY - y).r(),
				getColorFromSpectrum(globals::mouseX - x, globals::mouseY - y).g(),
				getColorFromSpectrum(globals::mouseX - x, globals::mouseY - y).b(),
				m_alphaForColor);

			mousePointX = globals::mouseX;
			mousePointY = globals::mouseY;
		}

		// don't go crazy with values, -4 because rect to draw has this width
		mousePointX = std::clamp(mousePointX, x, x + m_width - 4);
		mousePointY = std::clamp(mousePointY, y, y + m_width - 4);

		// render small 4x4 rectangle, this gotta be fixed with pos btw
		render::drawFilledRect(mousePointX, mousePointY, 4, 4, Colors::LightBlue);
		render::drawOutlineRect(mousePointX, mousePointY, 4, 4, Colors::Black);

		// if in hue bar range, then first delete old rgba bitmap or whatever u gonna call it, and init new
		if (isInHueRange && isKeyDown(VK_LBUTTON))
		{
			// should be detected by hue's but my cursor is brokey and gotta fix it
			m_levelHue = 255 * (globals::mouseY - y) / m_height;

			for (int i = 0; i < m_width; i++)
			{
				for (int j = 0; j < m_height; j++)
				{
					*reinterpret_cast<Color*>(m_gradient.get() + i + j * m_width) =
						Color::fromHSB(Color::getHueFromColor(
							Color(
								getColorFromHueBar(globals::mouseX - (x + 2 + m_width), globals::mouseY - y).r(),
								getColorFromHueBar(globals::mouseX - (x + 2 + m_width), globals::mouseY - y).g(),
								getColorFromHueBar(globals::mouseX - (x + 2 + m_width), globals::mouseY - y).b(),
								m_alphaForColor)),
							j / static_cast<float>(m_width), i / static_cast<float>(m_width));
				}
			}
			// then delete old
			interfaces::surface->deleteTextureID(m_gradientID);
			// and create new, this is valid way? I think so
			render::initNewTexture(m_gradientID, m_gradient.get(), m_width, m_height);
		}

		if (isInAlphaRange && isKeyDown(VK_LBUTTON))
		{
			m_levelAlpha = 255 * (globals::mouseY - y) / m_height;

			m_alphaForColor = 255 - m_levelAlpha;
		}

		// to align
		float posYHue = (m_levelHue / 255.0f) * m_height;
		// single check to clamp, to not show it in outlines
		if (posYHue > m_height - 2)
			posYHue -= 2;

		float posYAlpha = (m_levelAlpha / 255.0f) * m_height;
		// single check to clamp, to not show it in outlines
		if (posYAlpha > m_height - 2)
			posYAlpha -= 2;

		//render::drawGradient(vecAlphaPos.x, vecAlphaPos.y, vecAlphaSize.x, vecAlphaSize.y, Color(255, 255, 255), Color(0, 0, 0), false);

		// rgb hue stuff
		render::drawFilledRect(x + m_width + 2 + 1, y + posYHue + 1, SLIDER_WIDTH - 2, 1, Colors::LightBlue);
		// alpha slider stuff
		render::drawFilledRect(x + m_width + SLIDER_WIDTH + 4 + 1, y + posYAlpha + 1, SLIDER_WIDTH - 2, 1, Colors::LightBlue);
		// hue slider outline, + 2 due to rect between them
		render::drawOutlineRect(x + m_width + 2, y + posYHue, SLIDER_WIDTH, 3, Colors::Black);
		// alpha slider
		render::drawOutlineRect(x + m_width + SLIDER_WIDTH + 4, y + posYAlpha, SLIDER_WIDTH, 3, Colors::Black);
		// outline for whole hue rect
		render::drawOutlineRect(x + 2 + m_width, y, SLIDER_WIDTH, m_height, Colors::Black);
		// outline for whole alpha rect
		render::drawOutlineRect(x + 4 + m_width + SLIDER_WIDTH, y, SLIDER_WIDTH, m_height, Colors::Black);
		// color spectrum outline
		render::drawOutlineRect(x, y, m_width, m_height, Colors::Black);
	}
}

Color GUI::ColorPicker::getColorFromSpectrum(const int x, const int y)
{
	return *reinterpret_cast<Color*>(m_gradient.get() + x + y * COLOR_PICKER_SIZE);
}

Color GUI::ColorPicker::getColorFromHueBar(const int x, const int y)
{
	return *reinterpret_cast<Color*>(m_hueBar.get() + x + y * SLIDER_WIDTH);
}

bool GUI::ColorPicker::isActive()
{
	return m_active;
}

// when you use 1st method to draw colorpicker, then make it static and call once
void GUI::ColorPicker::destroy()
{
	interfaces::surface->deleteTextureID(m_gradientID);
	interfaces::surface->deleteTextureID(m_hueID);
	//interfaces::surface->deleteTextureID(m_alphaID);
}
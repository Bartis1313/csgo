#include "ColorPicker.hpp"
#include "../../../../globals.hpp"

std::unique_ptr<Color[]> GUI::ColorPicker::m_gradient = nullptr;
std::unique_ptr<Color[]> GUI::ColorPicker::m_alphaBar = nullptr;
std::unique_ptr<Color[]> GUI::ColorPicker::m_hueBar = nullptr;

int GUI::ColorPicker::m_gradientID = 0;
int GUI::ColorPicker::m_textureALPHAID = 0;
int GUI::ColorPicker::m_textureHUEID = 0;

// because it's rectangle and using this names as height or width can confuse why
#define COLOR_PICKER_SIZE COLORPICKER_HEIGHT
//#define SLIDER_SIZE 20

GUI::ColorPicker::ColorPicker(const std::string& title, Color* color)
	: m_title{ title }, m_colorNow{ color }
{
	setElement(COLORPICKER_WIDTH, COLORPICKER_HEIGHT);
	setPadding(0);
}

void GUI::ColorPicker::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	// ghetto is to spam gradients by multiplying 25.5? probably the same way but less effective and even longer to render

	//happens only once, because static
	if (!m_gradient)
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

	render::drawGradient(pos->x, pos->y, m_width, m_height, *m_colorNow, Colors::Black, false);
	render::drawOutlineRect(pos->x - 1, pos->y - 1, m_width + 1, m_height + 1, Colors::Black);

	bool isInRangeButton = isMouseInRange(pos->x, pos->y, m_width, m_height);

	static bool toChange = false;
	static auto check = [this]() -> void
	{
		m_active = !m_active;
		toChange = false;
	};

	// whenever we click the main box to show list of features
	bool toSkip = false;

	if (isKeyPressed(VK_LBUTTON) && isInRangeButton)
	{
		if (toChange)
			check();
		toSkip = true;
	}
	else if (isInRangeButton)
	{
		toChange = true;
	}

	int x = pos->x + 100;
	int y = pos->y;

	bool isInSpectrumRange = isMouseInRange(x, y, m_width, m_height);

	if (this->isActive())
	{
		if (isKeyPressed(VK_LBUTTON) && !(isInRangeButton || isInSpectrumRange) && !toSkip)
			m_active = !m_active;

		render::drawFromTexture(m_gradientID, x, y, m_width, m_height, Colors::White);

		if (isKeyDown(VK_LBUTTON) && isInSpectrumRange)
		{
			*m_colorNow = getColorFromPos(globals::mouseX - x, globals::mouseY - y);
		}
	}

	// second method, which maybe look nicer, but may cost performance a lot
	/*
	if (!m_gradient)
	{
		m_gradient = std::make_unique<Color[]>(COLOR_PICKER_SIZE * COLOR_PICKER_SIZE);
		m_hueBar = std::make_unique<Color[]>(COLOR_PICKER_SIZE * SLIDER_SIZE);
		m_alphaBar = std::make_unique<Color[]>(COLOR_PICKER_SIZE * SLIDER_SIZE);

		// init the bars
		for (int i = 0; i < SLIDER_SIZE; i++)
		{
			for (int j = 0; j < COLOR_PICKER_SIZE; j++)
			{
				*reinterpret_cast<Color*>(m_hueBar.get() + i + j * SLIDER_SIZE) =
					Color::fromHSB(j / 150.0f, 1.0f, 1.0f);
				*reinterpret_cast<Color*>(m_alphaBar.get() + i + j * SLIDER_SIZE) =
					Color(0, 0, 0, j / static_cast<float>(COLOR_PICKER_SIZE) / 255.0f);
			}
		}

		// init the color pick, unfortunately surface spamming rectangles isn't very good idea, it would need to init once again after each change
		// did not test the performance after the change.
		for (int i = 0; i < COLOR_PICKER_SIZE; i++)
		{
			for (int j = 0; j < COLOR_PICKER_SIZE; j++)
			{
				*reinterpret_cast<Color*>(m_gradient.get() + i + j * COLOR_PICKER_SIZE) =
					Color::fromHSB(Color::getHueFromColor(Colors::Black),
						j / static_cast<float>(COLOR_PICKER_SIZE), i / static_cast<float>(COLOR_PICKER_SIZE));
			}
		}
		render::initNewTexture(m_gradientID, m_gradient.get(), COLOR_PICKER_SIZE, COLOR_PICKER_SIZE);
		render::initNewTexture(m_textureHUEID, m_hueBar.get(), SLIDER_SIZE, COLOR_PICKER_SIZE);
		render::initNewTexture(m_textureALPHAID, m_alphaBar.get(), SLIDER_SIZE, COLOR_PICKER_SIZE);
	}

	render::drawFromTexture(m_gradientID, 900, 300, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE, Colors::White);
	render::drawFromTexture(m_textureALPHAID, 200, 300, SLIDER_SIZE, COLOR_PICKER_SIZE, Colors::White);
	render::drawFromTexture(m_textureHUEID, 100, 300, SLIDER_SIZE, COLOR_PICKER_SIZE, Colors::White);
	*/
}

Color GUI::ColorPicker::getColorFromPos(const int x, const int y)
{
	//printf("x is = %i y is = %i and index to gradient = %i\n", x, y, y * COLOR_PICKER_SIZE);
	return *reinterpret_cast<Color*>(m_gradient.get() + x + y * COLOR_PICKER_SIZE);
}

bool GUI::ColorPicker::isActive() const
{
	return m_active;
}
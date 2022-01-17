#include "gui.hpp"

std::unique_ptr<Color[]> GUI::ColorPicker::m_gradient = nullptr;
std::unique_ptr<Color[]> GUI::ColorPicker::m_alphaBar = nullptr;
std::unique_ptr<Color[]> GUI::ColorPicker::m_hueBar = nullptr;

int GUI::ColorPicker::m_gradientID = 0;
int GUI::ColorPicker::m_textureALPHAID = 0;
int GUI::ColorPicker::m_textureHUEID = 0;

#define COLOR_PICKER_SIZE 150
#define SLIDER_SIZE 20

void GUI::ColorPicker::draw()
{
	// ghetto is to spam gradients by multiplying 25.5? probably the same way but less effective and even longer to render

	//happens only once, because static
	if (!m_gradient)
	{
		m_gradient = std::make_unique<Color[]>(COLOR_PICKER_SIZE * COLOR_PICKER_SIZE);

		float hue = 0.0f;
		float saturation = 0.99f;
		float lightness = 1.0f;

		// loop through all gradient indexes to fill them accordingly
		for (int i = 0; i < COLOR_PICKER_SIZE; i++)
		{
			for (int j = 0; j < COLOR_PICKER_SIZE; j++)
			{
				*reinterpret_cast<Color*>(m_gradient.get() + j + i * COLOR_PICKER_SIZE) = Color::hslToRGB(hue, saturation, lightness);

				hue += (1.0f / COLOR_PICKER_SIZE);
			}

			lightness -= (1.0f / COLOR_PICKER_SIZE);
			hue = 0.0f;
		}

		render::initNewTexture(m_gradientID, m_gradient.get(), COLOR_PICKER_SIZE, COLOR_PICKER_SIZE);
	}

	render::drawFromTexture(m_gradientID, 900, 300, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE, Colors::White);

	if (isMouseInRange(900, 300, COLOR_PICKER_SIZE, COLOR_PICKER_SIZE) && isKeyDown(VK_LBUTTON))
	{
		render::text(900, 250, fonts::tahoma, "Some long text to see", true, getColorFromPos(globals::mouseX - 900, globals::mouseY - 300));
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
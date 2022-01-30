#include "TextInput.hpp"
#include "../../../../../utilities/utilities.hpp"
#include <chrono>

bool GUI::TextInput::m_inited = false;

std::array<std::string, 256> GUI::TextInput::m_smallLetters;
std::array<std::string, 256> GUI::TextInput::m_bigLetters;

GUI::TextInput::TextInput(const std::string& title, std::string* text)
	: m_title{ title }, m_text{ text }
{
	setElement(TEXTINPUT_WIDTH, TEXTINPUT_HEIGHT);
	setPadding(40);
}

constexpr bool isbadKey(const short key)
{
	if (key == VK_ESCAPE || key == VK_RETURN)
		return true;

	return false;
}

// you can use game's interval timers in interfaces::globals for delay too, in way that triggerbot does it
constexpr auto timeSinceEpoch = []() -> long long
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
};

void GUI::TextInput::draw(Vector2D* pos, Menu* parent, bool skipCall)
{
	if (!m_inited)
		return;

	static long long timer = 0LL;
	// static long long timerBackSpace = 0LL; -> problems with making rules for that

	bool isInRange = isMouseInRange(pos->x, pos->y, m_width, m_height);

	// text on up with padding
	render::text(pos->x, pos->y - 15, fonts::menuFont, m_title, false, Colors::White);

	render::drawGradient(pos->x, pos->y, m_width, m_height, m_color,
		(isInRange || this->isActive()) ? Color(m_color.r() + 30, m_color.g() + 30, m_color.b() + 30, m_color.a()) : m_secColor, false);

	render::drawOutlineRect(pos->x, pos->y, m_width, m_height, Colors::Black);

	static bool toChange = false;
	static auto check = [this]() -> void
	{
		m_active = !m_active;
		toChange = false;
	};

	if (isKeyPressed(VK_LBUTTON) && isInRange)
	{
		if (toChange)
			check();
	}
	else if (isInRange)
	{
		toChange = true;
	}

	if (this->isActive())
	{
		// basically when we click anything not in range, aka fast stop
		if (isKeyPressed(VK_LBUTTON) && !isInRange)
		{
			m_active = false;
		}
		// this will not include long pressing interval etc... For that there needs to be a recode and a some checks, delay turned for long pressing only etc...
		if (isKeyPressed(VK_BACK) && !m_text->empty())
		{
			m_text->pop_back();
		}

		for (short i = 0; i < 256; i++)
		{
			// should skip some useless keys here

			// if any key got pressed
			if (isKeyPressed(i))
			{
				// when keys like enter etc are clicked the input are is closing
				if (isKeyPressed(i) && isbadKey(i))
				{
					m_active = false;
					break;
				}
				// some hardcoded check
				if (auto size = render::getTextSize(fonts::menuFont, *m_text); size < m_width + 15 && !m_smallLetters.at(i).empty() && !m_bigLetters.at(i).empty())
				{
					if (isKeyDown(VK_SHIFT))
					{
						*m_text += m_bigLetters.at(i);
					}
					else
					{
						*m_text += m_smallLetters.at(i);
					}
				}
			}
		}
	}

	// "blink" effect
	if (auto now = timeSinceEpoch(); now > timer)
		timer = now + 1000;

	std::string toDraw = *m_text;
	if (auto size = render::getTextSize(fonts::menuFont, *m_text); this->isActive() && timeSinceEpoch() > timer - 500 && size < m_width - 4)
	{
		toDraw += "|";
	}

	// apply when text too long
	if (auto size = render::getTextSize(fonts::menuFont, *m_text); size > m_width - 15)
	{
		// so much hardcoded, dunno how to make it easy
		toDraw = toDraw.substr(0, 18).append("...");
	}

	if (toDraw.empty())
	{
		toDraw = "...";
	}

	render::text(pos->x + (m_width / 2), pos->y + 3, fonts::menuFont, toDraw, true, this->isActive() ? Colors::LightBlue : Colors::White);

	pos->y += this->getPadding();
}

void GUI::TextInput::initTabs()
{
#define FILL_SMALL(idx) m_smallLetters.at(idx) = utilities::toLowerCase(utilities::getKeyName(idx))
#define FILL_BIG(idx) m_bigLetters.at(idx) = utilities::getKeyName(idx)

	std::fill(m_smallLetters.begin(), m_smallLetters.end(), "");
	std::fill(m_bigLetters.begin(), m_bigLetters.end(), "");

	// space
	FILL_SMALL(0x20) = " ";
	FILL_BIG(0x20) = " ";


	// numbers
	for (int i = 0x30; i < 0x3A; i++)
	{
		FILL_SMALL(i);
		// when pressing shift, we have those special signs, so I init them manually
	}

	m_bigLetters.at(0x30) = ")";
	m_bigLetters.at(0x31) = "!";
	m_bigLetters.at(0x32) = "@";
	m_bigLetters.at(0x34) = "#";
	m_bigLetters.at(0x35) = "$";
	m_bigLetters.at(0x36) = "%";
	m_bigLetters.at(0x37) = "^";
	m_bigLetters.at(0x38) = "&";
	m_bigLetters.at(0x39) = "*";

	// numpad keys in text are like normal keys 1- 2- 3...
	for (int i = VK_NUMPAD0; i < VK_NUMPAD9; i++)
	{
		int counter = 0;
		m_smallLetters.at(i) = std::to_string(counter);
		m_bigLetters.at(i) = std::to_string(counter);
		counter++;
	}

	// alphabet
	for (int i = 0x41; i < 0x5B; i++)
	{
		FILL_SMALL(i);
		FILL_BIG(i);
	}

	// signs, don't care for those shifted ones, so inited same
	for (int i = 0xBA; i < 0xC0; i++)
	{
		FILL_SMALL(i);
		FILL_BIG(i);
	}

	// another signs
	for (int i = 0xDB; i < 0xDF; i++)
	{
		FILL_SMALL(i);
	}

	// shift pressed signs
	m_bigLetters.at(0xDB) = "{";
	m_bigLetters.at(0xDC) = "|";
	m_bigLetters.at(0xDD) = "}";
	m_bigLetters.at(0xDE) = "\"";

	m_inited = true;
}

bool GUI::TextInput::isActive() const
{
	return m_active;
}
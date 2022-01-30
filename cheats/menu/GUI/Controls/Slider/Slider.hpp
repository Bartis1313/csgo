#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// a very basic slider that represent some percentage or number to dynamically choice in range between
	// TODO: make it as template for inigers and floating points?
	class Slider : public Element
	{
	public:
		Slider(const std::string& title, const std::pair<float, float>& minMax, float* value);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall) override;
		_NODISCARD bool isActive() const;
	private:
		std::string m_title;
		bool m_active;
		std::pair<float, float> m_minMax;
		float* m_value;
	};
}
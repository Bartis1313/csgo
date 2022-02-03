#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// checkbox, represents a button to change some boolean
	class CheckBox : public Element
	{
	public:
		CheckBox(const std::string& title, bool* feature);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		_NODISCARD virtual bool isActive() override;
	private:
		bool m_active = false;
		bool* m_feature;
		std::string m_title;
	};
}
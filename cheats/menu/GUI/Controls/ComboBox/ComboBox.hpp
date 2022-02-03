#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// button but as a list with options
	class ComboBox : public Element
	{
	public:
		// init some smaller groupbox, this one will only allow to select ONE feature
		ComboBox(const std::string& title, const std::vector<std::string>& names, int* option);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		_NODISCARD virtual bool isActive() override;
		_NODISCARD bool isOptionOn() const;
	private:
		std::string m_title;
		bool m_active = false;
		std::vector<std::string> m_names;
		int* m_option;
	};
}
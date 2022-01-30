#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// only difference between and this and combobox is multiple selection choice
	class MultiBox : public Element
	{
	public:
		// init groupbox by many features listed in array
		MultiBox(const std::string& title, const std::vector<std::string>& names, std::vector<bool>* feature);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		_NODISCARD bool isActive() const;
		_NODISCARD bool isOptionOn(const size_t idx) const;
	private:
		std::string m_title;
		bool m_active;
		std::vector<std::string> m_names;
		std::vector<bool>* m_listedOptions;
	};
}
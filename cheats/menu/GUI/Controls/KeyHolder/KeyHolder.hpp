#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// press anything, few keys are banned. 
	class KeyHolder : public Element
	{
	public:
		KeyHolder(const std::string& title, int* key);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		_NODISCARD bool isActive() const;
	private:
		std::string m_title;
		bool m_active;
		int* m_key;
	};
}
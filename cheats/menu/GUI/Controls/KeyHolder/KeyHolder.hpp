#pragma once
#include "../Element/Element.hpp"

namespace GUI
{
	// press anything, few keys are banned. 
	class KeyHolder : public Element
	{
	public:
		KeyHolder(int* key);
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) override;
		_NODISCARD virtual bool isActive() override;
	private:
		std::string m_title;
		bool m_active = false;
		bool m_isGettingKey = false;
		int* m_key;
	};
}
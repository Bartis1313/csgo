#pragma once

#include <Windows.h> //keys
#include <string>
#include <array>
#include <utility> // pairs 
#include <functional> // lambdas
#include <vector>

#include "../../mainMenu.hpp" // menu class
#include "../../../../../utilities/renderer/renderer.hpp" // drawing
#include "../../Misc/guimisc.hpp" // this detection for keys
#include "../../sizes.hpp" // sizes for elements

namespace GUI
{
	// this enum will be only helpful if you want to edit this gui and detect selected element by type
	// Init this in every class, not base ofc
	enum Type
	{
		BUTTON,
		CHECKBOX,
		COLOR_PICKER,
		COMBOBOX,
		// group should not be there

		HELP_MARKER,
		KEY_HOLDER,
		MULTIBOX,
		SLIDER,
		// tab should not be there

		TEXT_INPUT
	};


	// base class for menu gui
	class Element
	{
	public:
		/// <summary>
		/// Draws, updates keys, and does everything for the class
		/// </summary>
		/// <param name="pos">location to draw</param>
		/// <param name="parent">address to menu object</param>
		/// <param name="skipCall">skip call only used for combo/multi boxes</param>
		virtual void draw(Vector2D* pos, Menu* parent, bool skipCall = false) = 0;
		// default should does not matter at all. If needed, make own version in class
		_NODISCARD virtual bool isActive() { return false; };
		// when it comes to some special stuff init and destroy should be called in main
		virtual void destroy() {};
		virtual void init() {};
		_NODISCARD int getWidth() const { return m_width; }
		_NODISCARD int getHeight() const { return m_height; }
		_NODISCARD int getPadding() const { return m_padding; }
		_NODISCARD Type getType() const { return m_type; }

		void setElement(const int width, const int height) { setWidth(width); setHeight(height); }
		void setPadding(const int padding) { m_padding = padding; }
	protected:
		int m_width, m_height;
		int m_padding;
		// useful for detecting if the element is active, all elements should have this
		bool m_toChange = false;
		Type m_type;
	private:
		void setWidth(const int width) { m_width = width; }
		void setHeight(const int height) { m_height = height; }
	};
}
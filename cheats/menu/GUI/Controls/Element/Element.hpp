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
	// base class for menu gui
	class Element
	{
	public:
		/// <summary>
		/// Draws, updates keys, and does everything for the class
		/// </summary>
		/// <param name="location">location to draw</param>
		/// <param name="parent">address to menu object</param>
		/// <param name="skipCall">skip call only used for combo/multi boxes</param>
		virtual void draw(Vector2D* location, Menu* parent, bool skipCall = false) = 0;

		_NODISCARD int getWidth() const { return m_width; }
		_NODISCARD int getHeight() const { return m_height; }
		_NODISCARD int getPadding() const { return m_padding; }

		void setElement(const int width, const int height) { setWidth(width); setHeight(height); }
		void setPadding(const int padding) { m_padding = padding; }
	protected:
		int m_width, m_height;
		int m_padding;
	private:
		void setWidth(const int width) { m_width = width; }
		void setHeight(const int height) { m_height = height; }
	};
}
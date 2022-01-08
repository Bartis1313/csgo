#include "gui.hpp"
#include "../../../utilities/utilities.hpp"

void GUI::renderGUI::test()
{
	mainWindow.initWindow();
	mainWindow.draw();
	boxik.draw();
	for (auto& tab : windowTabs)
		tab.draw();
}
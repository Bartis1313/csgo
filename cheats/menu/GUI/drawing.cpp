#include "gui.hpp"
#include "../../../utilities/utilities.hpp"

void GUI::renderGUI::test()
{
	mainWindow.initWindow();
	mainWindow.draw();
	windowTabs.draw();
	if (windowTabs.getSelected() == GUI::TabNames::SOMETHING_2)
	{
		somebutton.draw();
		button.draw();
		group.draw();
	}
}
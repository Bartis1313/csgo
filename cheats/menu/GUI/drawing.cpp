#include "drawing.hpp"
#include "../GUI/Controls/Tab/Tab.hpp"
#include "../../../utilities/utilities.hpp"

#define CA const auto

void GUI::initGui()
{
	menu = new Menu(VK_INSERT);
	//fatchad = TextureHolder(IMAGE_PNG1);

	CA legitbot = menu->addTab("aimbot");
	CA visuals = menu->addTab("visuals");
	CA visuals3 = menu->addTab("visuals3");
	CA visuals4 = menu->addTab("visuals4");

	CA legitbot1 = legitbot->addGroup("legitbot1");
	legitbot1->addCheckBox("title", &testBool);
	legitbot1->addComboBox("title", m_vec, &selected);
	legitbot1->addCheckBox("title", &testBool);
	legitbot1->addCheckBox("title", &testBool);

	CA legitbot2 = legitbot->addGroup("legitbot2", Lining::PAD);
	legitbot2->addCheckBox("title", &testBool);
	legitbot2->addCheckBox("title", &testBool);
	legitbot2->addCheckBox("title", &testBool);
	legitbot2->addCheckBox("title", &testBool);

	CA visuals1 = visuals->addGroup("yo");
	visuals1->addCheckBox("title", &testBool);
	visuals1->addCheckBox("title", &testBool);
	visuals1->addCheckBox("title", &testBool);
	visuals1->addCheckBox("title", &testBool);
	visuals1->addCheckBox("title", &testBool);
	visuals1->addCheckBox("title", &testBool);

	CA visuals31 = visuals3->addGroup("yo2");
	visuals31->addCheckBox("title", &testBool);
	visuals31->addCheckBox("title", &testBool);
	visuals31->addCheckBox("title", &testBool);
	visuals31->addCheckBox("title", &testBool);
	visuals31->addCheckBox("title", &testBool);
	visuals31->addCheckBox("title", &testBool);

	CA visuals32 = visuals4->addGroup("yo3");
	visuals32->addCheckBox("title", &testBool);
	visuals32->addCheckBox("title", &testBool);
	visuals32->addCheckBox("title", &testBool);
	visuals32->addCheckBox("title", &testBool);
	visuals32->addCheckBox("title", &testBool);
	visuals32->addCheckBox("title", &testBool);

	LOG(LOG_INFO, "init menu success");
}

void GUI::draw()
{
	menu->draw();
}
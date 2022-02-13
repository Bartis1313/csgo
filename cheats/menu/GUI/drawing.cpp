#include "drawing.hpp"
#include "../GUI/Controls/Tab/Tab.hpp"
#include "../../globals.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../config/vars.hpp"

// because here no need to store this anywhere
namespace vecNames
{
	std::vector<std::string> aimbotHitboxes =
	{
		"NEAREST",
		"HEAD",
		"CHEST"
	};
}

void GUI::initGui()
{
	TextInput::initTabs();

	menu = new Menu(VK_INSERT);
	//fatchad = new TextureHolder(IMAGE_PNG1);

	const auto legitbot = menu->addTab("aimbot");
	const auto visuals = menu->addTab("visuals");
	const auto misc = menu->addTab("misc");
	const auto extra = menu->addTab("extra");

	const auto legitbotSub = legitbot->addGroup("main");
	legitbotSub->addCheckBox("Enabled aimbot", &config.getRef<bool>(vars.bAimbot));
	legitbotSub->addSlider("Aimbot FOV", { 0.0f, 50.0f }, &config.getRef<float>(vars.fFovAimbot));
	legitbotSub->addComboBox("Aimbot selection", vecNames::aimbotHitboxes, &config.getRef<int>(vars.iAimbot));
	legitbotSub->addCheckBox("Enabled RCS", &config.getRef<bool>(vars.bRCS));
	legitbotSub->addSlider("RCS percentage", { 0.0f, 100.0f }, &config.getRef<float>(vars.fRCS));
	legitbotSub->addCheckBox("Triggerbot enabled", &config.getRef<bool>(vars.bTriggerbot));
	legitbotSub->addSlider("Triggerbot ms", { 0.0f, 200.0f }, &config.getRef<float>(vars.fTriggerDelay));

	const auto visualsSub = visuals->addGroup("visuals");
	//visualsSub->addCheckBox

	// add more...

	const auto menuExtra = extra->addGroup("menus");
	menuExtra->addCheckBox("x88 menu", &config.getRef<bool>(vars.bMenuOpenedx88));
	

	LOG(LOG_INFO, "init menu success");
}

void GUI::draw()
{
	menu->draw();

	//fatchad->draw(400, 500);
}
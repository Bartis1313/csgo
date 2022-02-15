#include "drawing.hpp"
#include "../GUI/Controls/Tab/Tab.hpp"
#include "../../globals.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../config/vars.hpp"

// surface menu example, with everything being painted by the game's engine
// I will probably stop updating it, as using ready framework for GUI's is way easier

// because here no need to store this anywhere
namespace vecNames
{
	std::vector<std::string> aimbotHitboxes =
	{
		XOR("NEAREST"),
		XOR("HEAD"),
		XOR("CHEST")
	};

	std::vector<std::string> espNames =
	{
		XOR("2D BOX"),
		XOR("2D FILLED"),
		XOR("3D BOX"),
		XOR("3D FILLED")
	};

	std::vector<std::string> crossHairNames =
	{
		XOR("Static"),
		XOR("Recoil"),
		XOR("Engine")
	};

	std::vector<std::string> chamsNames =
	{
		XOR("Static"),
		XOR("Xyz")
	};

	std::vector<std::string> btChams =
	{
		XOR("Stable"),
		XOR("Last tick"),
		XOR("Rainbow")
	};
}

void GUI::initGui()
{
	TextInput::initTabs();

	menu = new Menu(VK_INSERT);
	//fatchad = new TextureHolder(IMAGE_PNG1);

	const auto legitbot = menu->addTab(XOR("Aimbot"));
	const auto visuals = menu->addTab(XOR("Visuals"));
	const auto misc = menu->addTab(XOR("Misc"));

	const auto legitbotSub = legitbot->addGroup(XOR("Aimbot & triggerbot"));
	legitbotSub->addCheckBox(XOR("Enabled aimbot"), &config.getRef<bool>(vars.bAimbot));
	legitbotSub->addSlider(XOR("Aimbot FOV"), { 0.0f, 50.0f }, &config.getRef<float>(vars.fFovAimbot));
	legitbotSub->addComboBox(XOR("Aimbot selection"), vecNames::aimbotHitboxes, &config.getRef<int>(vars.iAimbot));
	legitbotSub->addSlider(XOR("Smoothing"), { 0.0f, 50.0f }, &config.getRef<float>(vars.fSmooth));
	legitbotSub->addCheckBox(XOR("Enabled RCS"), &config.getRef<bool>(vars.bRCS));
	legitbotSub->addSlider(XOR("RCS percentage"), { 0.0f, 100.0f }, &config.getRef<float>(vars.fRCS));
	legitbotSub->addCheckBox(XOR("Triggerbot enabled"), &config.getRef<bool>(vars.bTriggerbot));
	legitbotSub->addSlider(XOR("Triggerbot ms"), { 0.0f, 200.0f }, &config.getRef<float>(vars.fTriggerDelay));
	legitbotSub->addColorPicker(&config.getRef<Color>(vars.cDrawFov));
	legitbotSub->addCheckBox(XOR("Draw Fov"), &config.getRef<bool>(vars.bDrawFov));

	const auto backTrackingSub = legitbot->addGroup(XOR("Backtrack"), Lining::PAD);
	backTrackingSub->addCheckBox(XOR("Enabled backtrack"), &config.getRef<bool>(vars.bBacktrack));
	backTrackingSub->addSlider(XOR("Backtrack ms"), { 0.0f, 200.0f }, &config.getRef<float>(vars.fBacktrackTick));

	const auto visualsSub = visuals->addGroup(XOR("Players"));
	visualsSub->addCheckBox(XOR("Enabled esp"), &config.getRef<bool>(vars.bEsp));
	visualsSub->addComboBox(XOR("Esp mode"), vecNames::espNames, &config.getRef<int>(vars.iEsp));
	visualsSub->addColorPicker(&config.getRef<Color>(vars.cBox));
	visualsSub->addText(XOR("Box color"));
	visualsSub->addColorPicker(&config.getRef<Color>(vars.cBoxFill));
	visualsSub->addText(XOR("Box filled color"));
	visualsSub->addCheckBox(XOR("Health"), &config.getRef<bool>(vars.bDrawHealth));
	visualsSub->addCheckBox(XOR("Armor"), &config.getRef<bool>(vars.bDrawArmor));
	visualsSub->addCheckBox(XOR("Infos"), &config.getRef<bool>(vars.bDrawInfos));
	visualsSub->addColorPicker(&config.getRef<Color>(vars.cSkeleton));
	visualsSub->addCheckBox(XOR("Skeleton"), &config.getRef<bool>(vars.bDrawSkeleton));
	visualsSub->addColorPicker(&config.getRef<Color>(vars.cDlight));
	visualsSub->addCheckBox(XOR("Dlight"), &config.getRef<bool>(vars.bDLight));
	
	const auto visualsWeapons = visuals->addGroup(XOR("Weapons"), Lining::PAD);
	visualsWeapons->addColorPicker(&config.getRef<Color>(vars.cWeaponText));
	visualsWeapons->addCheckBox(XOR("Weapon"), &config.getRef<bool>(vars.bDrawWeapon));
	visualsWeapons->addColorPicker(&config.getRef<Color>(vars.cReloadbar));
	visualsWeapons->addText(XOR("Reload bar color"));
	visualsWeapons->addColorPicker(&config.getRef<Color>(vars.cDrawDroppedC4));
	visualsWeapons->addCheckBox(XOR("Dropped C4"), &config.getRef<bool>(vars.bDrawDroppedC4));
	visualsWeapons->addColorPicker(&config.getRef<Color>(vars.cDrawBomb));
	visualsWeapons->addCheckBox(XOR("C4 Info"), &config.getRef<bool>(vars.bDrawBomb));
	visualsWeapons->addColorPicker(&config.getRef<Color>(vars.cDropped));
	visualsWeapons->addText(XOR("Dropped weapons"));
	visualsWeapons->addColorPicker(&config.getRef<Color>(vars.cMolotovRange));
	visualsWeapons->addCheckBox(XOR("Molotov range"), &config.getRef<bool>(vars.bDrawmolotovRange));

	const auto visualsNades = visuals->addGroup(XOR("Nades"), Lining::PAD);
	visualsNades->addCheckBox(XOR("Projectiles"), &config.getRef<bool>(vars.bDrawProjectiles));
	visualsNades->addColorPicker(&config.getRef<Color>(vars.cFlashBang));
	visualsNades->addText(XOR("Flashbang color"));
	visualsNades->addColorPicker(&config.getRef<Color>(vars.cGranede));
	visualsNades->addText(XOR("Granede color"));
	visualsNades->addColorPicker(&config.getRef<Color>(vars.cMolotov));
	visualsNades->addText(XOR("Molotov color"));
	visualsNades->addColorPicker(&config.getRef<Color>(vars.cIncediary));
	visualsNades->addText(XOR("Incediary color"));
	visualsNades->addColorPicker(&config.getRef<Color>(vars.cSmoke));
	visualsNades->addText(XOR("Smoke color"));
	visualsNades->addColorPicker(&config.getRef<Color>(vars.cDecoy));
	visualsNades->addText(XOR("Decoy color"));

	const auto chamsSub = visuals->addGroup(XOR("Chams & glow"), Lining::PAD);
	chamsSub->addCheckBox(XOR("Chams enabled"), &config.getRef<bool>(vars.bChams));
	chamsSub->addComboBox(XOR("Chams type"), vecNames::chamsNames, &config.getRef<int>(vars.iChams));
	chamsSub->addColorPicker(&config.getRef<Color>(vars.cGlow));
	chamsSub->addCheckBox(XOR("Enable glow"), &config.getRef<bool>(vars.bGlow));
	chamsSub->addColorPicker(&config.getRef<Color>(vars.cChams));
	chamsSub->addText(XOR("Chams color"));
	chamsSub->addColorPicker(&config.getRef<Color>(vars.cChamsXYZ));
	chamsSub->addText(XOR("Chams XYZ color"));
	chamsSub->addColorPicker(&config.getRef<Color>(vars.cBackTrackChams));
	chamsSub->addComboBox(XOR("Backtrack chams"), vecNames::btChams, &config.getRef<int>(vars.iBacktrackChams));

	const auto miscSub = misc->addGroup(XOR("Misc"));
	miscSub->addColorPicker(&config.getRef<Color>(vars.cSky));
	miscSub->addCheckBox(XOR("Run night"), &config.getRef<bool>(vars.bRunNight));
	miscSub->addSlider(XOR("Brightness world"), { 0.0f, 100.0f }, &config.getRef<float>(vars.fBrightnessWorld));
	miscSub->addColorPicker(&config.getRef<Color>(vars.cZeusRange));
	miscSub->addCheckBox(XOR("Zeus range"), &config.getRef<bool>(vars.bDrawZeusRange));
	miscSub->addCheckBox(XOR("Sound Esp"), &config.getRef<bool>(vars.bSoundEsp));
	miscSub->addSlider(XOR("FOV local"), { -35.0f, 35.0f }, &config.getRef<float>(vars.fFOV));
	miscSub->addCheckBox(XOR("Third person"), &config.getRef<bool>(vars.bThirdp));
	miscSub->addComboBox(XOR("Crosshair"), vecNames::crossHairNames, &config.getRef<int>(vars.iCrosshair));
	miscSub->addCheckBox(XOR("Enemy is aiming warn"), &config.getRef<bool>(vars.bAimingWarn));

	const auto radarSub = misc->addGroup(XOR("Radar"), Lining::PAD);
	radarSub->addCheckBox(XOR("2D Radar enabled"), &config.getRef<bool>(vars.bRadar));
	radarSub->addColorPicker(&config.getRef<Color>(vars.cRadarLine));
	radarSub->addText(XOR("Angle view line"));
	radarSub->addColorPicker(&config.getRef<Color>(vars.cRadarPlayer));
	radarSub->addText(XOR("Radar players color"));
	radarSub->addSlider(XOR("Radar circle thickness"), { 0.0f, 20.0f }, &config.getRef<float>(vars.fRadarThickness));
	radarSub->addSlider(XOR("Radar length"), { 0.0f, 40.0f }, &config.getRef<float>(vars.fRadarLenght));
	radarSub->addSlider(XOR("Radar scale"), { 0.0f, 10.0f }, &config.getRef<float>(vars.fRadarScale));

	const auto miscSubOther = misc->addGroup(XOR("Misc other"), Lining::PAD);
	miscSubOther->addCheckBox(XOR("Bunnyhop"), &config.getRef<bool>(vars.bBunnyHop));
	miscSubOther->addCheckBox(XOR("Autostrafe"), &config.getRef<bool>(vars.bAutoStrafe));
	miscSubOther->addCheckBox(XOR("Hitmarker"), &config.getRef<bool>(vars.bDrawHitmarker));
	miscSubOther->addHelpMarker(XOR("Will play the sound after every kill"));
	miscSubOther->addCheckBox(XOR("Play Hitmarker"), &config.getRef<bool>(vars.bPlayHitmarker));
	miscSubOther->addCheckBox(XOR("No scope overlay"), &config.getRef<bool>(vars.bNoScope));
	miscSubOther->addCheckBox(XOR("FPS plot"), &config.getRef<bool>(vars.bShowFpsPlot));
	miscSubOther->addCheckBox(XOR("Velocity plot"), &config.getRef<bool>(vars.bShowVelocityPlot));
	miscSubOther->addCheckBox(XOR("Draw misc info"), &config.getRef<bool>(vars.bDrawMiscInfo));

	const auto configSub = misc->addGroup(XOR("Config"), Lining::PAD);
	configSub->addCheckBox(XOR("x88 menu"), &config.getRef<bool>(vars.bMenuOpenedx88));
	configSub->addTextInput(XOR("Config name"), &configName);
	// ye this sucks... would need to rewrite whole gui for proper dynamic fields like listing the config files
	configSub->addButton(XOR("Create"), []()
		{
			config.save(configName);
			config.reload();
		});
	configSub->addButton(XOR("Delete"), []()
		{
			config.deleteCfg(configName);
		});
	configSub->addButton(XOR("Save"), []()
		{
			config.save(configName);
		});
	configSub->addButton(XOR("Load"), []()
		{
			config.load(configName);
		});

	LOG(LOG_INFO, XOR("init menu success"));
}

void GUI::draw()
{
	menu->draw();

	//fatchad->draw(400, 500);
}
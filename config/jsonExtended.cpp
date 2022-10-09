#include "jsonExtended.hpp"

#define CHECK_JSON if(!j.contains(key)) return;

void from_json(const json& j, const std::string& key, bool& val)
{
	CHECK_JSON;

	if (auto jval = j.at(key); jval.is_boolean())
		jval.get_to(val);
}

void from_json(const json& j, const std::string& key, int& val)
{
	CHECK_JSON;

	if (auto jval = j.at(key); jval.is_number_integer())
		jval.get_to(val);
}

void from_json(const json& j, const std::string& key, size_t& val)
{
	CHECK_JSON;

	if (auto jval = j.at(key); jval.is_number_unsigned())
		jval.get_to(val);
}

void from_json(const json& j, const std::string& key, float& val)
{
	CHECK_JSON;

	if (auto jval = j.at(key); jval.is_number_float())
		jval.get_to(val);
}

void from_json(const json& j, const std::string& key, std::string& val)
{
	CHECK_JSON;

	if (auto jval = j.at(key); jval.is_string())
		jval.get_to(val);
}

template<typename T, size_t SIZE>
void from_json(const json& j, const std::string& key, std::array<T, SIZE>& val)
{
	CHECK_JSON;

	if (auto jval = j.at(key); jval.is_array())
	{
		// get_to throwing some weird exception to allow only numbers?
		for (size_t i = 0; i < jval.size(); i++)
			val[i] = jval[i];
			/*jval.get_to<T>(val[i]);*/
	}
}

#undef CHECK_JSON

void from_json(const json& j, CfgWeapon& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Fov", val.fov);
	from_json(j, "Smooth", val.smooth);
	from_json(j, "Smooth mode", val.smoothMode);
	from_json(j, "Extra speed", val.useExtraSpeed);
	from_json(j, "Curve", val.curveAim);
	from_json(j, "Curve X", val.curveX);
	from_json(j, "Curve Y", val.curveY);
	from_json(j, "Aim method", val.methodAim);
	from_json(j, "Aim selection", val.aimSelection);
	from_json(j, "Aim delay enabled", val.aimDelay);
	from_json(j, "Aim delay ms", val.aimDelayVal);
	from_json(j, "Rcs enabled", val.rcs);
	from_json(j, "Rcs X", val.rcsX);
	from_json(j, "Rcs Y", val.rcsY);
	from_json(j, "Triggerbot enabled", val.triggerbot);
	from_json(j, "Triggerbot delay", val.triggerbotDelay);
	from_json(j, "Smoke check", val.smokeCheck);
	from_json(j, "Flash alpha limit", val.flashLimit);
}

void to_json(json& j, const CfgWeapon& val)
{
	j["Enabled"] = val.enabled;
	j["Fov"] = val.fov;
	j["Smooth"] = val.smooth;
	j["Smooth mode"] = val.smoothMode;
	j["Extra speed"] = val.useExtraSpeed;
	j["Curve"] = val.curveAim;
	j["Curve X"] = val.curveX;
	j["Curve Y"] = val.curveY;
	j["Aim method"] = val.methodAim;
	j["Aim selection"] = val.aimSelection;
	j["Aim delay enabled"] = val.aimDelay;
	j["Aim delay ms"] = val.aimDelayVal;
	j["Rcs enabled"] = val.rcs;
	j["Rcs X"] = val.rcsX;
	j["Rcs Y"] = val.rcsY;
	j["Triggerbot enabled"] = val.triggerbot;
	j["Triggerbot delay"] = val.triggerbotDelay;
	j["Smoke check"] = val.smokeCheck;
	j["Flash alpha limit"] = val.flashLimit;
}

void from_json(const json& j, const std::string& key, CfgColor& val)
{
	from_json(j[key], "RGBA", val.getColorRef().getRef());
	from_json(j[key], "Rainbow", val.getRainbowRef());
	from_json(j[key], "Speed", val.getSpeedRef());
}
#include <iostream>

void to_json(json& j, const std::string key, const CfgColor& val)
{
	j[key]["RGBA"] = val.getColor().get();
	j[key]["Rainbow"] = val.getRainbow();
	j[key]["Speed"] = val.getSpeed();
}

void from_json(const json& j, const std::string& key, CfgBeam& val)
{
	using s = std::string;

	from_json(j[key], "Index", val.index);
	from_json(j[key], "Flags", val.flags);
	from_json(j[key], "Life", val.life);
	from_json(j, "BeamCol", val.color);
	from_json(j[key], "Width", val.width);
	from_json(j[key], "Fade length", val.fadeLength);
	from_json(j[key], "Amplitude", val.amplitude);
	from_json(j[key], "Speed", val.speed);
	from_json(j[key], "Start frame", val.startFrame);
	from_json(j[key], "Framerate", val.frameRate);
	from_json(j[key], "Segments", val.segments);
}

void to_json(json& j, const std::string key, const CfgBeam& val)
{
	j[key]["Index"] = val.index;
	j[key]["Flags"] = val.flags;
	j[key]["Life"] = val.life;
	to_json(j, "BeamCol", val.color);
	j[key]["Width"] = val.width;
	j[key]["Fade length"] = val.fadeLength;
	j[key]["Amplitude"] = val.amplitude;
	j[key]["Speed"] = val.speed;
	j[key]["Start frame"] = val.startFrame;
	j[key]["Framerate"] = val.frameRate;
	j[key]["Segments"] = val.segments;
}

void from_json(const json& j, const std::string& key, Key& val)
{
	from_json(j[key], "Mode", val.getKeyModeInt());
	from_json(j[key], "Code", val.getKeyCode());
}

void to_json(json& j, const std::string key, const Key& val)
{
	j[key]["Mode"] = (int)val.getKeyMode();
	j[key]["Code"] = val.getKeyCode();
}

void from_json(const json& j, VarAim& val)
{
	from_json(j, "AimbotArr", val.weapons);
	from_json(j, "Use key", val.useKey);
}

void to_json(json& j, const VarAim& val)
{
	j["AimbotArr"] = val.weapons;
	j["Use key"] = val.useKey;
}

void from_json(const json& j, VarAimPaint& val)
{
	from_json(j, "Enabled fov", val.enabledFov);
	from_json(j, "Enabled point", val.enabledPoint);
	from_json(j, "Color Fov", val.colorFov);
	from_json(j, "Color paint", val.colorPoint);
}

void to_json(json& j, const VarAimPaint& val)
{
	j["Enabled fov"] = val.enabledFov;
	j["Enabled point"] = val.enabledPoint;
	to_json(j, "Color Fov", val.colorFov);
	to_json(j, "Color paint", val.colorPoint);
}

void from_json(const json& j, VarVisuals& val)
{
	from_json(j["Chams"], *val.chams);
	from_json(j["Glow"], *val.glow);
	from_json(j["Esp"], *val.esp);
	from_json(j["Sound"], *val.sound);
	from_json(j["Dormacy"], *val.dormacy);
	from_json(j["World"], *val.world);
}

void to_json(json& j, const VarVisuals& val)
{
	to_json(j["Chams"], *val.chams);
	to_json(j["Glow"], *val.glow);
	to_json(j["Esp"], *val.esp);
	to_json(j["Sound"], *val.sound);
	to_json(j["Dormacy"], *val.dormacy);
	to_json(j["World"], *val.world);
}

void from_json(const json& j, VarVisuals::VarChams& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Enabled Players", val.players);
	from_json(j, "Index of Players", val.indexPlayers);
	from_json(j, "Color players", val.colorPlayers);
	from_json(j, "Enabled XQZ Players", val.enabledXQZPlayers);
	from_json(j, "Color XQZ players", val.colorXQZPlayers);
	from_json(j, "Enabled Weapons", val.enabledWeapons);
	from_json(j, "Disable Weapons", val.weaponHide);
	from_json(j, "Index of weapons", val.indexWeapons);
	from_json(j, "Color weapons", val.colorWeapons);
	from_json(j, "Enabled Arms", val.enabledArms);
	from_json(j, "Disable Arms", val.armsHide);
	from_json(j, "Color arms", val.colorArms);
	from_json(j, "Index of arms", val.indexArms);
	from_json(j, "Enable Backtrack Chams", val.enabledBacktrack);
	from_json(j, "Index Backtrack Chams", val.indexBacktrack);
	from_json(j, "Color backtrack", val.colorBacktrack);
	from_json(j, "Backtrack chams mode", val.modeBacktrack);
	from_json(j, "Backtrack chams rainbow", val.rainbowBacktrack);
	from_json(j, "Backtrack chams speed", val.rainbowBacktrackSpeed);
}

void to_json(json& j, const VarVisuals::VarChams& val)
{
	j["Enabled"] = val.enabled;
	j["Enabled Players"] = val.players;
	j["Index of Players"] = val.indexPlayers;
	to_json(j, "Color players", val.colorPlayers);
	j["Enabled XQZ Players"] = val.enabledXQZPlayers;
	to_json(j, "Color XQZ players", val.colorXQZPlayers);
	j["Enabled Weapons"] = val.enabledWeapons;
	j["Disable Weapons"] = val.weaponHide;
	j["Index of weapons"] = val.indexWeapons;
	to_json(j, "Color weapons", val.colorWeapons);
	j["Enabled Arms"] = val.enabledArms;
	j["Disable Arms"] = val.armsHide;
	to_json(j, "Color arms", val.colorArms);
	j["Index of arms"] = val.indexArms;
	j["Enable Backtrack Chams"] = val.enabledBacktrack;
	j["Index Backtrack Chams"] = val.indexBacktrack;
	to_json(j, "Color backtrack", val.colorBacktrack);
	j["Backtrack chams mode"] = val.modeBacktrack;
	j["Backtrack chams rainbow"] = val.rainbowBacktrack;
	j["Backtrack chams speed"] = val.rainbowBacktrackSpeed;
}

void from_json(const json& j, VarVisuals::VarGlow& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Color player", val.colorPlayer);
}

void to_json(json& j, const VarVisuals::VarGlow& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color player", val.colorPlayer);
}

void from_json(const json& j, VarVisuals::VarEsp& val)
{
	from_json(j["Boxes"], *val.boxes);
	from_json(j["HealthBar"], *val.healthBar);
	from_json(j["ArmorBar"], *val.armorBar);
	from_json(j["WeaponBar"], *val.weaponBar);
	from_json(j["Name"], *val.nameBar);
	from_json(j["Skeleton"], *val.skeleton);
	from_json(j["Dlight"], *val.dlight);
	from_json(j["ExtraInfo"], *val.extraInfo);
	from_json(j["Flags"], *val.flags);
	from_json(j["Lasers"], *val.lasers);
	from_json(j["Checks"], *val.checks);
	from_json(j["Dropped"], *val.dropped);
}

void to_json(json& j, const VarVisuals::VarEsp& val)
{
	to_json(j["Boxes"], *val.boxes);
	to_json(j["HealthBar"], *val.healthBar);
	to_json(j["ArmorBar"], *val.armorBar);
	to_json(j["WeaponBar"], *val.weaponBar);
	to_json(j["Name"], *val.nameBar);
	to_json(j["Skeleton"], *val.skeleton);
	to_json(j["Dlight"], *val.dlight);
	to_json(j["ExtraInfo"], *val.extraInfo);
	to_json(j["Flags"], *val.flags);
	to_json(j["Lasers"], *val.lasers);
	to_json(j["Checks"], *val.checks);
	to_json(j["Dropped"], *val.dropped);
}

void from_json(const json& j, VarVisuals::VarEsp::VarBoxes& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Mode", val.mode);
	from_json(j, "Lines", val.color);
	from_json(j, "Fill", val.fill);
	from_json(j, "Outlined", val.outline);
	from_json(j, "Multicolor", val.multiColor);
	from_json(j, "Speed", val.multiColorSpeed);
}

void to_json(json& j, const VarVisuals::VarEsp::VarBoxes& val)
{
	j["Enabled"] = val.enabled;
	j["Mode"] = val.mode;
	to_json(j, "Lines", val.color);
	to_json(j, "Fill", val.fill);
	j["Outlined"] = val.outline;
	j["Multicolor"] = val.multiColor;
	j["Speed"] = val.multiColorSpeed;
}

void from_json(const json& j, VarVisuals::VarEsp::VarHealthBar& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarVisuals::VarEsp::VarHealthBar& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarVisuals::VarEsp::VarArmorBar& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarVisuals::VarEsp::VarArmorBar& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarVisuals::VarEsp::VarWeaponBar& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Enabled Translate", val.translate);
	from_json(j, "Text", val.text);
	from_json(j, "Bar", val.bar);
}

void to_json(json& j, const VarVisuals::VarEsp::VarWeaponBar& val)
{
	j["Enabled"] = val.enabled;
	j["Enabled Translate"] = val.translate;
	to_json(j, "Text", val.text);
	to_json(j, "Bar", val.bar);
}

void from_json(const json& j, VarVisuals::VarEsp::VarName& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarVisuals::VarEsp::VarName& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarVisuals::VarEsp::VarSkeleton& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Lines", val.color);
	from_json(j, "Debug points", val.showDebug);
}

void to_json(json& j, const VarVisuals::VarEsp::VarSkeleton& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Lines", val.color);
	j["Debug points"] = val.showDebug;
}

void from_json(const json& j, VarVisuals::VarEsp::VarDlight& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Color", val.color);
	from_json(j, "Radius", val.radius);
	from_json(j, "Decay", val.decay);
	from_json(j, "Exponent", val.exponent);
}

void to_json(json& j, const VarVisuals::VarEsp::VarDlight& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color", val.color);
	j["Radius"] = val.radius;
	j["Decay"] = val.decay;
	j["Exponent"] = val.exponent;
}

void from_json(const json& j, VarVisuals::VarEsp::VarExtraInfo& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarVisuals::VarEsp::VarExtraInfo& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarVisuals::VarEsp::VarFlags& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Flags", val.flags);
}

void to_json(json& j, const VarVisuals::VarEsp::VarFlags& val)
{
	j["Enabled"] = val.enabled;
	j["Flags"] = val.flags;
}

void from_json(const json& j, VarVisuals::VarEsp::VarLasers& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarVisuals::VarEsp::VarLasers& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarVisuals::VarEsp::VarChecks& val)
{
	from_json(j, "Dead only", val.dead);
	from_json(j, "Smoke check", val.smoke);
	from_json(j, "Visible check", val.visible);
	from_json(j, "Flash limit", val.flashLimit);
}

void to_json(json& j, const VarVisuals::VarEsp::VarChecks& val)
{
	j["Dead only"] = val.dead;
	j["Smoke check"] = val.smoke;
	j["Visible check"] = val.visible;
	j["Flash limit"] = val.flashLimit;
}

void from_json(const json& j, VarVisuals::VarEsp::VarDropped& val)
{
	from_json(j, "Enbaled", val.enabled);
	from_json(j, "Color", val.color);
	from_json(j, "Flags", val.flags);
}

void to_json(json& j, const VarVisuals::VarEsp::VarDropped& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color", val.color);
	j["Flags"] = val.flags;
}

void from_json(const json& j, VarVisuals::VarSound& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Color", val.color);
	from_json(j, "Time", val.time);
	from_json(j, "Max dist", val.maxDist);
	from_json(j, "Max dist line", val.maxDistLine);
	from_json(j, "Line", val.colorLine);
}

void to_json(json& j, const VarVisuals::VarSound& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color", val.color);
	j["Time"] = val.enabled;
	j["Max dist"] = val.enabled;
	j["Max dist line"] = val.enabled;
	to_json(j, "Line", val.colorLine);
}

void from_json(const json& j, VarVisuals::VarDormacy& val)
{
	from_json(j, "Time", val.time);
	from_json(j, "Time limit", val.limit);
}

void to_json(json& j, const VarVisuals::VarDormacy& val)
{
	j["Time"] = val.time;
	j["Time limit"] = val.limit;
}

void from_json(const json& j, VarVisuals::VarWorld& val)
{
	from_json(j["Bomb"], *val.bomb);
	from_json(j["Projectiles"], *val.projectiles);
	from_json(j["Sky"], *val.sky);
	from_json(j["Modulate"], *val.modulate);
	from_json(j["Molotov"], *val.molotov);
	from_json(j["Smoke"], *val.smoke);
	from_json(j["Zeus"], *val.zeus);
	from_json(j["Particles"], *val.particles);
	from_json(j["Tracer"], *val.tracer);
	from_json(j["Impacts"], *val.impacts);
	from_json(j["Fog"], *val.fog);
	from_json(j["ScreenEffect"], *val.screenEffect);
	from_json(j["Tone"], *val.tone);
	from_json(j["Weather"], *val.weather);
	from_json(j["Ambient"], *val.ambient);
}

void to_json(json& j, const VarVisuals::VarWorld& val)
{
	to_json(j["Bomb"], *val.bomb);
	to_json(j["Projectiles"], *val.projectiles);
	to_json(j["Sky"], *val.sky);
	to_json(j["Modulate"], *val.modulate);
	to_json(j["Molotov"], *val.molotov);
	to_json(j["Smoke"], *val.smoke);
	to_json(j["Zeus"], *val.zeus);
	to_json(j["Particles"], *val.particles);
	to_json(j["Tracer"], *val.tracer);
	to_json(j["Impacts"], *val.impacts);
	to_json(j["Fog"], *val.fog);
	to_json(j["ScreenEffect"], *val.screenEffect);
	to_json(j["Tone"], *val.tone);
	to_json(j["Weather"], *val.weather);
	to_json(j["Ambient"], *val.ambient);
}

void from_json(const json& j, VarVisuals::VarWorld::VarBomb& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Color", val.background);
}

void to_json(json& j, const VarVisuals::VarWorld::VarBomb& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color", val.background);
}

void from_json(const json& j, VarVisuals::VarWorld::VarProjectiles& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Flash", val.flash);
	from_json(j, "Nade", val.nade);
	from_json(j, "Molotov", val.molotov);
	from_json(j, "Smoke", val.smoke);
	from_json(j, "Decoy", val.decoy);
}

void to_json(json& j, const VarVisuals::VarWorld::VarProjectiles& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Flash", val.flash);
	to_json(j, "Nade", val.nade);
	to_json(j, "Molotov", val.molotov);
	to_json(j, "Smoke", val.smoke);
	to_json(j, "Decoy", val.decoy);
}

void from_json(const json& j, VarVisuals::VarWorld::VarSky& val)
{
	from_json(j, "Skybox normal", val.indexNormal);
	from_json(j, "Skybox custom", val.indexCustom);
	from_json(j, "Remove sky", val.removeSky);
}

void to_json(json& j, const VarVisuals::VarWorld::VarSky& val)
{
	j["Skybox normal"] = val.indexNormal;
	j["Skybox custom"] = val.indexCustom;
	j["Remove sky"] = val.removeSky;
}

void from_json(const json& j, VarVisuals::VarWorld::VarModulate& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Texture", val.texture);
	from_json(j, "Prop", val.prop);
	from_json(j, "Sky", val.sky);
	from_json(j, "Shader param", val.shader);
}

void to_json(json& j, const VarVisuals::VarWorld::VarModulate& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Texture", val.texture);
	to_json(j, "Prop", val.prop);
	to_json(j, "Sky", val.sky);
	j["Shader param"] = val.shader;
}

void from_json(const json& j, VarVisuals::VarWorld::VarMolotov& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Molly", val.color);
	from_json(j, "Text", val.colorText);
}

void to_json(json& j, const VarVisuals::VarWorld::VarMolotov& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Molly", val.color);
	to_json(j, "Text", val.colorText);
}

void from_json(const json& j, VarVisuals::VarWorld::VarSmoke& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Color", val.color);
}

void to_json(json& j, const VarVisuals::VarWorld::VarSmoke& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color", val.color);
}

void from_json(const json& j, VarVisuals::VarWorld::VarZeus& val)
{
	from_json(j, "Range", val.enabled);
	from_json(j, "Party", val.party);
	from_json(j, "Tracing", val.tracing);
	from_json(j, "Color", val.color);
}

void to_json(json& j, const VarVisuals::VarWorld::VarZeus& val)
{
	j["Range"] = val.enabled;
	j["Party"] = val.party;
	j["Tracing"] = val.tracing;
	to_json(j, "Color", val.color);
}

void from_json(const json& j, VarVisuals::VarWorld::VarParticles& val)
{
	from_json(j, "Enabled Smoke", val.enabledSmoke);
	from_json(j, "Smoke", val.colorSmoke);
	from_json(j, "Enabled Blood", val.enabledBlood);
	from_json(j, "Blood", val.colorBlood);
	from_json(j, "Enabled Molly", val.enabledMolotov);
	from_json(j, "Molly", val.colorMolotov);
}

void to_json(json& j, const VarVisuals::VarWorld::VarParticles& val)
{
	j["Enabled Smoke"] = val.enabledSmoke;
	to_json(j, "Smoke", val.colorSmoke);
	j["Enabled Blood"] = val.enabledBlood;
	to_json(j, "Blood", val.colorBlood);
	j["Enabled Molly"] = val.enabledMolotov;
	to_json(j, "Molly", val.colorMolotov);
}

void from_json(const json& j, VarVisuals::VarWorld::VarTracer& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Beam", val.beamTracer);
}

void to_json(json& j, const VarVisuals::VarWorld::VarTracer& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Beam", val.beamTracer);
}

void from_json(const json& j, VarVisuals::VarWorld::VarImpacts& val)
{
	from_json(j, "Client enabled", val.enabledClient);
	from_json(j, "Client time", val.timeClient);
	from_json(j, "LineClient", val.colorClient);
	from_json(j, "FillClient", val.colorClientFill);
	from_json(j, "Local enabled", val.enabledClient);
	from_json(j, "Local time", val.timeLocal);
	from_json(j, "LineLocal",  val.colorLocal);
	from_json(j, "FillLocal", val.colorLocalFill);
}

void to_json(json& j, const VarVisuals::VarWorld::VarImpacts& val)
{
	j["Client enabled"] = val.enabledClient;
	j["Client time"] = val.timeClient;
	to_json(j, "LineClient", val.colorClient);
	to_json(j, "FillClient", val.colorClientFill);
	j["Local enabled"] = val.enabledClient;
	j["Local time"] = val.timeLocal;
	to_json(j, "LineLocal", val.colorLocal);
	to_json(j, "FillLocal", val.colorLocalFill);
}

void from_json(const json& j, VarVisuals::VarWorld::VarFog& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Distance", val.distance);
	from_json(j, "Color", val.color);
}

void to_json(json& j, const VarVisuals::VarWorld::VarFog& val)
{
	j["Enabled"] = val.enabled;
	j["Distance"] = val.distance;
	to_json(j, "Color", val.color);
}

void from_json(const json& j, VarVisuals::VarWorld::VarScreenEffect& val)
{
	from_json(j, "Index", val.index);
	from_json(j, "Param", val.param);
	from_json(j, "Color", val.color);
}

void to_json(json& j, const VarVisuals::VarWorld::VarScreenEffect& val)
{
	j["Index"] = val.index;
	j["Param"] = val.param;
	to_json(j, "Color", val.color);
}

void from_json(const json& j, VarVisuals::VarWorld::VarTone& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Min", val.min);
	from_json(j, "max", val.max);
	from_json(j, "Bloom", val.bloom);
}

void to_json(json& j, const VarVisuals::VarWorld::VarTone& val)
{
	j["Enabled"] = val.enabled;
	j["Min"] = val.min;
	j["Max"] = val.max;
	j["Bloom"] = val.bloom;
}

void from_json(const json& j, VarVisuals::VarWorld::VarWeather& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Length", val.length);
	from_json(j, "Rain Speed", val.rainSpeed);
	from_json(j, "Width", val.width);
	from_json(j, "Velocity", val.velocity);
	from_json(j, "Alpha", val.alpha);
	from_json(j, "Wind", val.windSpeed);
}

void to_json(json& j, const VarVisuals::VarWorld::VarWeather& val)
{
	j["Enabled"] = val.enabled;
	j["Length"] = val.length;
	j["Rain Speed"] = val.rainSpeed;
	j["Width"] = val.width;
	j["Velocity"] = val.velocity;
	j["Alpha"] = val.alpha;
	j["Wind"] = val.windSpeed;
}

void from_json(const json& j, VarVisuals::VarWorld::VarAmbient& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Color", val.color);
}

void to_json(json& j, const VarVisuals::VarWorld::VarAmbient& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Color", val.color);
}

void from_json(const json& j, VarKeys& val)
{
	from_json(j, "Enabled X88", val.enabledX88Menu);
	from_json(j, "Aimbot", val.aimbot);
	from_json(j, "Menu", val.menu);
	from_json(j, "Panic", val.panic);
	from_json(j, "Logs", val.console);
	from_json(j, "Freelook", val.freeLook);
	from_json(j, "MirrorCam", val.mirrorCam);
	from_json(j, "FreeCam", val.freeCam);
	from_json(j, "FlashLight", val.flashLight);
	from_json(j, "Thirdp", val.thirdP);
}

void to_json(json& j, const VarKeys& val)
{
	j["Enabled X88"] = val.enabledX88Menu;
	to_json(j, "Aimbot", val.aimbot);
	to_json(j, "Menu", val.menu);
	to_json(j, "Panic", val.panic);
	to_json(j, "Logs", val.console);
	to_json(j, "Freelook", val.freeLook);
	to_json(j, "MirrorCam", val.mirrorCam);
	to_json(j, "FreeCam", val.freeCam);
	to_json(j, "FlashLight", val.flashLight);
	to_json(j, "Thirdp", val.thirdP);
}

void from_json(const json& j, VarBacktrack& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Time", val.time);
	from_json(j, "Smoke", val.smoke);
	from_json(j, "Flash limit", val.flashLimit);
}

void to_json(json& j, const VarBacktrack& val)
{
	j["Enabled"] = val.enabled;
	j["Time"] = val.time;
	j["Smoke"] = val.smoke;
	j["Flash limit"] = val.flashLimit;
}

void from_json(const json& j, VarMisc& val)
{
	from_json(j["Logs"], *val.logs);
	from_json(j["Fov"], *val.fov);
	from_json(j["ThridP"], *val.thirdp);
	from_json(j["Crosshair"], *val.crosshair);
	from_json(j["FakeLatency"], *val.fakeLatency);
	from_json(j["BunnyHop"], *val.bunnyHop);
	from_json(j["Hitmarker"], *val.hitmarker);
	from_json(j["Scope"], *val.scope);
	from_json(j["Plots"], *val.plots);
	from_json(j["Info"], *val.info);
	from_json(j["AimWarn"], *val.aimWarn);
	from_json(j["Radar"], *val.radar);
	from_json(j["Trail"], *val.trail);
	from_json(j["Nades"], *val.nade);
	from_json(j["Hat"], *val.hat);
	from_json(j["Discord"], *val.discord);
	from_json(j["MotionBlur"], *val.motionBlur);
	from_json(j["FreeLook"], *val.freeLook);
	from_json(j["MirrorCam"], *val.mirrorCam);
	from_json(j["FreeCam"], *val.freeCam);
	from_json(j["Flashlight"], *val.flashLight);
}

void to_json(json& j, const VarMisc& val)
{
	to_json(j["Logs"], *val.logs);
	to_json(j["Fov"], *val.fov);
	to_json(j["ThridP"], *val.thirdp);
	to_json(j["Crosshair"], *val.crosshair);
	to_json(j["FakeLatency"], *val.fakeLatency);
	to_json(j["BunnyHop"], *val.bunnyHop);
	to_json(j["Hitmarker"], *val.hitmarker);
	to_json(j["Scope"], *val.scope);
	to_json(j["Plots"], *val.plots);
	to_json(j["Info"], *val.info);
	to_json(j["AimWarn"], *val.aimWarn);
	to_json(j["Radar"], *val.radar);
	to_json(j["Trail"], *val.trail);
	to_json(j["Nades"], *val.nade);
	to_json(j["Hat"], *val.hat);
	to_json(j["Discord"], *val.discord);
	to_json(j["MotionBlur"], *val.motionBlur);
	to_json(j["FreeLook"], *val.freeLook);
	to_json(j["MirrorCam"], *val.mirrorCam);
	to_json(j["FreeCam"], *val.freeCam);
	to_json(j["Flashlight"], *val.flashLight);
}

void from_json(const json& j, VarMisc::VarLogs& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Time", val.time);
}

void to_json(json& j, const VarMisc::VarLogs& val)
{
	j["Enabled"] = val.enabled;
	j["Time"] = val.time;
}

void from_json(const json& j, VarMisc::VarFov& val)
{
	from_json(j, "Fov", val.value);
}

void to_json(json& j, const VarMisc::VarFov& val)
{
	j["Fov"] = val.value;
}

void from_json(const json& j, VarMisc::VarThridP& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Distance", val.distance);
	from_json(j, "X", val.x);
	from_json(j, "Y", val.y);
}

void to_json(json& j, const VarMisc::VarThridP& val)
{
	j["Enabled"] = val.enabled;
	j["Distance"] = val.distance;
	j["X"] = val.x;
	j["Y"] = val.y;
}

void from_json(const json& j, VarMisc::VarCrosshair& val)
{
	from_json(j, "Mode", val.index);
}

void to_json(json& j, const VarMisc::VarCrosshair& val)
{
	j["Mode"] = val.index;
}

void from_json(const json& j, VarMisc::VarFakeLatency& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Amount", val.amount);
}

void to_json(json& j, const VarMisc::VarFakeLatency& val)
{
	j["Enabled"] = val.enabled;
	j["Amount"] = val.amount;
}

void from_json(const json& j, VarMisc::VarBunnyHop& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Chance", val.chance);
	from_json(j, "Mode", val.indexStrafe);
}

void to_json(json& j, const VarMisc::VarBunnyHop& val)
{
	j["Enabled"] = val.enabled;
	j["Chance"] = val.chance;
	j["Mode"] = val.indexStrafe;
}

void from_json(const json& j, VarMisc::VarHitmarker& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Play", val.play);
	from_json(j, "3D", val.enabled3D);
	from_json(j, "Resize", val.enabledResize);
	from_json(j, "Normal", val.colorNormal);
	from_json(j, "Head", val.colorHead);
	from_json(j, "Dead", val.colorDead);
	from_json(j, "Time", val.time);
}

void to_json(json& j, const VarMisc::VarHitmarker& val)
{
	j["Enabled"] = val.enabled;
	j["Play"] = val.play;
	j["3D"] = val.enabled3D;
	j["Resize"] = val.enabledResize;
	to_json(j, "Normal", val.colorNormal);
	to_json(j, "Head", val.colorHead);
	to_json(j, "Dead", val.colorDead);
	j["Time"] = val.time;
}

void from_json(const json& j, VarMisc::VarScope& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarMisc::VarScope& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarMisc::VarPlots& val)
{
	from_json(j, "Enabled FPS", val.enabledFps);
	from_json(j, "Enabled Velocity", val.enabledVelocity);
	from_json(j, "Fps", val.colorFPS);
	from_json(j, "Velocity", val.colorVelocity);
	from_json(j, "FPS cap", val.fpsCap);
	from_json(j, "FPS custom", val.fpsCustom);
	from_json(j, "Velocity custom", val.velocityCustom);
	from_json(j, "Velocity cap", val.velocityCap);
}

void to_json(json& j, const VarMisc::VarPlots& val)
{
	j["Enabled FPS"] = val.enabledFps;
	j["Enabled Velocity"] = val.enabledVelocity;
	to_json(j, "Fps", val.colorFPS);
	to_json(j, "Velocity", val.colorVelocity);
	j["FPS cap"] = val.fpsCap;
	j["FPS custom"] = val.fpsCustom;
	j["Velocity custom"] = val.velocityCustom;
	j["Velocity cap"] = val.velocityCap;
}

void from_json(const json& j, VarMisc::VarInfo& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarMisc::VarInfo& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarMisc::VarAimWarn& val)
{
	from_json(j, "Enabled", val.enabled);
}

void to_json(json& j, const VarMisc::VarAimWarn& val)
{
	j["Enabled"] = val.enabled;
}

void from_json(const json& j, VarMisc::VarRadar& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Line", val.colorLine);
	from_json(j, "Player", val.colorPlayer);
	from_json(j, "Thickness", val.thickness);
	from_json(j, "Length", val.length);
	from_json(j, "Scale", val.scale);
	from_json(j, "Ranges", val.ranges);
	from_json(j, "Size", val.size);
}

void to_json(json& j, const VarMisc::VarRadar& val)
{
	j["Enabled"] = val.enabled;
	to_json(j, "Line", val.colorLine);
	to_json(j, "Player", val.colorPlayer);
	j["Thickness"] = val.thickness;
	j["Length"] = val.length;
	j["Scale"] = val.scale;
	j["Ranges"] = val.ranges;
	j["Size"] = val.size;
}

void from_json(const json& j, VarMisc::VarTrail& val)
{
	from_json(j, "Enabled", val.enabled);
	from_json(j, "Mode", val.mode);
	from_json(j, "Color", val.color);
	from_json(j, "Rainbow Col", val.rainbow);
	from_json(j, "Speed", val.rainbowSpeed);
	from_json(j, "Beam Speed", val.beamSpeed);
	from_json(j, "Life", val.time);
}

void to_json(json& j, const VarMisc::VarTrail& val)
{
	j["Enabled"] = val.enabled;
	j["Mode"] = val.mode;
	to_json(j, "Color", val.color);
	j["Rainbow Col"] = val.rainbow;
	j["Speed"] = val.rainbowSpeed;
	j["Beam Speed"] = val.beamSpeed;
	j["Life"] = val.time;
}

void from_json(const json& j, VarMisc::VarNades& val)
{
	from_json(j, "Enable prediction", val.enabledPred);
	from_json(j, "Always", val.predAlways);
	from_json(j, "Pred", val.colorPredLine);
	from_json(j, "Fill", val.colorPredBoxFill);
	from_json(j, "Outline", val.colorPredBox);
	from_json(j, "Enable tracer", val.enabledTracer);
	from_json(j, "TracerLine", val.colorTracer);
	from_json(j, "Tracer distance", val.tracerDist);
	from_json(j, "Tracer warning", val.tracerWarn);
}

void to_json(json& j, const VarMisc::VarNades& val)
{
	j["Enable prediction"] = val.enabledPred;
	j["Always"] = val.predAlways;
	to_json(j, "Pred", val.colorPredLine);
	to_json(j, "Fill", val.colorPredBoxFill);
	to_json(j, "Outline", val.colorPredBox);
	j["Enable tracer"] = val.enabledTracer;
	to_json(j, "TracerLine", val.colorTracer);
	j["Tracer distance"] = val.tracerDist;
	j["Tracer warning"] = val.tracerWarn;
}

void from_json(const json& j, VarMisc::VarHat& val)
{
	from_json(j, "Enable", val.enabled);
	from_json(j, "Enable rainbow", val.rainbow);
	from_json(j, "Speed", val.rainbowSpeed);
	from_json(j, "Size", val.size);
	from_json(j, "Radius", val.radius);
	from_json(j, "Triangle alpha", val.rainbowAlpha);
	from_json(j, "Line alpha", val.rainbowLinesAlpha);
	from_json(j, "Triangle", val.colorTriangle);
	from_json(j, "Line", val.colorLine);
}

void to_json(json& j, const VarMisc::VarHat& val)
{
	j["Enable"] = val.enabled;
	j["Enable rainbow"] = val.rainbow;
	j["Speed"] = val.rainbowSpeed;
	j["Size"] = val.size;
	j["Radius"] = val.radius;
	j["Triangle alpha"] = val.rainbowAlpha;
	j["Line alpha"] = val.rainbowLinesAlpha;
	to_json(j, "Triangle", val.colorTriangle);
	to_json(j, "Line", val.colorLine);
}

void from_json(const json& j, VarMisc::VarDiscord& val)
{
	from_json(j, "Enable", val.enabled);
}

void to_json(json& j, const VarMisc::VarDiscord& val)
{
	j["Enable"] = val.enabled;
}

void from_json(const json& j, VarMisc::VarMotionBlur& val)
{
	from_json(j, "Enable", val.enabled);
	from_json(j, "Forward", val.forward);
	from_json(j, "Falling Intensity", val.fallingIntensity);
	from_json(j, "Falling Min", val.fallingMin);
	from_json(j, "Falling Max", val.fallingMax);
	from_json(j, "Strength", val.strength);
	from_json(j, "Rotation Intensity", val.rotationIntensity);
	from_json(j, "Roll Intensity", val.rollIntensity);
}

void to_json(json& j, const VarMisc::VarMotionBlur& val)
{
	j["Enable"] = val.enabled;
	j["Forward"] = val.forward;
	j["Falling Intensity"] = val.fallingIntensity;
	j["Falling Min"] = val.fallingMin;
	j["Falling Max"] = val.fallingMax;
	j["Strength"] = val.strength;
	j["Rotation Intensity"] = val.rotationIntensity;
	j["Roll Intensity"] = val.rollIntensity;
}

void from_json(const json& j, VarMisc::VarFreeLook& val)
{
	from_json(j, "Enable", val.enabled);
}

void to_json(json& j, const VarMisc::VarFreeLook& val)
{
	j["Enable"] = val.enabled;
}

void from_json(const json& j, VarMisc::VarMirrorCam& val)
{
	from_json(j, "Enable", val.enabled);
	from_json(j, "On key", val.onKey);
}

void to_json(json& j, const VarMisc::VarMirrorCam& val)
{
	j["Enable"] = val.enabled;
	j["On key"] = val.onKey;
}

void from_json(const json& j, VarMisc::VarFreeCam& val)
{
	from_json(j, "Enable", val.enabled);
	from_json(j, "Speed", val.speed);
}

void to_json(json& j, const VarMisc::VarFreeCam& val)
{
	j["Enable"] = val.enabled;
	j["Speed"] = val.speed;
}

void from_json(const json& j, VarMisc::VarFlashlight& val)
{
	from_json(j, "Enable", val.enabled);
	from_json(j, "Big mode", val.bigMode);
	from_json(j, "Fov", val.fov);
}

void to_json(json& j, const VarMisc::VarFlashlight& val)
{
	j["Enable"] = val.enabled;
	j["Big mode"] = val.bigMode;
	j["Fov"] = val.fov;
}

void from_json(const json& j, VarStyling& val)
{
	from_json(j, "Index", val.index);
	from_json(j, "Background", val.background);
	from_json(j, "Background speed", val.speed);
	from_json(j, "Background distance points", val.distance);
	from_json(j, "Background points size", val.size);
	from_json(j, "1", val.color1);
	from_json(j, "2", val.color2);
	from_json(j, "3", val.color3);
}

void to_json(json& j, const VarStyling& val)
{
	j["Index"] = val.index;
	j["Background"] = val.background;
	j["Background speed"] = val.speed;
	j["Background distance points"] = val.distance;
	j["Background points size"] = val.size;
	to_json(j, "1", val.color1);
	to_json(j, "2", val.color2);
	to_json(j, "3", val.color3);
}
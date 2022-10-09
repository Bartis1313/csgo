#pragma once

enum class AimbotWeapon
{
	RIFFLE,
	SMG,
	MACHINE_GUN,
	PISTOL,
	DEAGLE,
	REVOLVER,
	SCOUT,
	AWP,
	AUTO_SNIPER,
	SHOTGUN
};

enum class AimbotHitboxes
{
	NEAREST,
	HEAD,
	CHEST
};

enum class SmoothMode
{
	LINEAR,
	NON_LINEAR
};

enum class CrossHairTypes
{
	OFF,
	STATIC,
	RECOIL,
	ENGINE,
	SPREAD
};

enum class ChamsType
{
	FLAT,
	GENERIC,
	GLOW,
	METALIC,
	PEARLSCENT
};

enum class BTChamsType
{
	STABLE,
	LAST_TICK,
	RAINBOW,
	COLOR_CHANGE
};

enum class BoxTypes
{
	OFF,
	BOX2D,
	FILLED2D,
	BOX3D,
	FILLED3D
};

enum class MovementTrail
{
	BEAM,
	LINE,
	SPLASH
};

enum class EspFlags
{
	BOT,
	MONEY,
	WINS,
	RANK,
	ARMOR,
	ZOOM,
	C4,
};

enum class DroppedFlags
{
	BOX,
	TEXT,
	ICON,
	AMMO,
};

enum class MovementStraferMode
{
	OFF,
	SIDESPEED,
	ROTATE,
	DIRECTIONAL
};

enum class AimbotMethod
{
	CROSSHAIR,
	DYNAMIC
};

enum class MenuTypes
{
	CHERRY,
	DARK,
	CLASSIC,
	WHITE
};
#pragma once

#pragma region enums
enum class AimbotID : size_t
{
	NEAREST,
	HEAD,
	CHEST
};

enum class CrossHairTypes : size_t
{
	OFF,
	STATIC,
	RECOIL,
	ENGINE,
	SPREAD
};

enum class ChamsID : size_t
{
	FLAT,
	GENERIC,
	GLOW,
	METALIC,
	PEARLSCENT
};

enum class BTChamsID : size_t
{
	STABLE,
	LAST_TICK,
	RAINBOW,
	COLOR_CHANGE
};

enum class BoxTypes : size_t
{
	OFF,
	BOX2D,
	FILLED2D,
	BOX3D,
	FILLED3D
};

enum class MovementTrail : size_t
{
	BEAM,
	LINE,
	SPLASH
};

enum class EspFlags : size_t
{
	BOT,
	MONEY,
	WINS,
	RANK,
	ARMOR,
	ZOOM,
	C4,
	FLAGS_SIZE
};

enum class DroppedFlags : size_t
{
	BOX,
	TEXT,
	ICON,
	AMMO,
	FLAGS_SIZE
};

enum class MovementStraferMode : size_t
{
	OFF,
	SIDESPEED,
	ROTATE,
	DIRECTIONAL
};

enum class AimbotMethod : size_t
{
	CROSSHAIR,
	DYNAMIC
};

#pragma endregion
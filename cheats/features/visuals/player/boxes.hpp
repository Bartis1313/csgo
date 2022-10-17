#pragma once

#include <classes/base.hpp>

struct Box;

class BoxesDraw
{
public:
	static void drawBox2D(const Box& box, bool isDormant, float dormacyAlpha);
	static void drawBox2DFilled(const Box& box, bool isDormant, float dormacyAlpha);
	static void drawBox3D(const Box& box, bool isDormant, float dormacyAlpha, float thickness = 1.0f);
	static void drawBox3DFilled(const Box& box, bool isDormant, float dormacyAlpha, float thickness = 1.0f);
	static void drawBox3DFilledMultiColor(const Box& box, bool isDormant, float dormacyAlpha, float thickness = 1.0f);
};

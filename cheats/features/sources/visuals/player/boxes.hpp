#pragma once

struct Box;

class BoxesDraw
{
public:
	void drawBox2D(const Box& box, bool isDormant, float dormacyAlpha);
	void drawBox2DFilled(const Box& box, bool isDormant, float dormacyAlpha);
	void drawBox3D(const Box& box, bool isDormant, float dormacyAlpha, float thickness = 1.0f);
	void drawBox3DFilled(const Box& box, bool isDormant, float dormacyAlpha, float thickness = 1.0f);
	void drawBox3DFilledMultiColor(const Box& box, bool isDormant, float dormacyAlpha, float thickness = 1.0f);
};

[[maybe_unused]] inline auto g_BoxesDraw = BoxesDraw{};

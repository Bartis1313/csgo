#pragma once

struct Box;
struct Box3D;

class BoxesDraw
{
public:
	void drawBox2D(const Box& box);
	void drawBox2DFilled(const Box& box);
	void drawBox3D(const Box3D& box, const float thickness = 1.0f);
	void drawBox3DFilled(const Box3D& box, const float thickness = 1.0f);
	void drawBox3DFilledMultiColor(const Box3D& box, const float thickness = 1.0f);
};

[[maybe_unused]] inline auto g_BoxesDraw = BoxesDraw{};

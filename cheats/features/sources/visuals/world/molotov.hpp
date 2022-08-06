#pragma once

class Inferno_t;

class MolotovDraw
{
public:
	void draw(Inferno_t* ent);
};

[[maybe_unused]] inline auto g_Molotov = MolotovDraw{};

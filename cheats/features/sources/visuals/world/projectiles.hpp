#pragma once

class Entity_t;

class Projectiles
{
public:
	void run(Entity_t* ent, int classID);
};

[[maybe_unused]] inline auto g_Projectiles = Projectiles{};

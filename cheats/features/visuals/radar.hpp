#pragma once

struct Vector2D;
struct Vector;

class Radar final
{
public:
	void run();
private:
	[[nodiscard]]Vector2D entToRadar(const Vector& eye, const Vector& angles, const Vector& entPos, const Vector2D& pos, const Vector2D& size, const float scale);
};

inline Radar radar;
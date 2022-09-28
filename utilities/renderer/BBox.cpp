#include "BBox.hpp"

#include "renderer.hpp"

#include <SDK/ICollideable.hpp>
#include <SDK/structs/Entity.hpp>
#include <utilities/math/math.hpp>

Box::Box(Entity_t* ent)
{
	const auto col = ent->collideable();
	if (!col)
	{
		m_isValid = false;
		return;
	}
	const auto& min = col->OBBMins();
	const auto& max = col->OBBMaxs();
	const auto& matrixWorld = ent->renderableToWorldTransform();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

	const auto points = buildAABB(min, max);

	std::array<ImVec2, 8> screen = {};
	for (size_t i = 0; auto & el : screen)
	{
		if (!imRender.worldToScreen(math::transformVector(points.at(i), matrixWorld), el))
		{
			m_isValid = false;
			return;
		}

		left = std::min(left, el.x);
		top = std::min(top, el.y);
		right = std::max(right, el.x);
		bottom = std::max(bottom, el.y);

		this->points.at(i) = el;

		i++;
	}

	this->x = left;
	this->y = top;
	this->w = right - left;
	this->h = bottom - top;

	// get important points, eg: if you use 3d box, you want to render health by quads, not rects

	this->topleft = screen.at(7);
	this->topright = screen.at(6);
	this->bottomleft = screen.at(3);
	this->bottomright = screen.at(2);

	m_isValid = true;
}

std::array<Vec3, 8> Box::buildAABB(const Vec3& min, const Vec3& max)
{
	std::array points =
	{
		Vec3{ min[Coord::X], min[Coord::Y], min[Coord::Z] },
		Vec3{ min[Coord::X], max[Coord::Y], min[Coord::Z] },
		Vec3{ max[Coord::X], max[Coord::Y], min[Coord::Z] },
		Vec3{ max[Coord::X], min[Coord::Y], min[Coord::Z] },
		Vec3{ min[Coord::X], min[Coord::Y], max[Coord::Z] },
		Vec3{ min[Coord::X], max[Coord::Y], max[Coord::Z] },
		Vec3{ max[Coord::X], max[Coord::Y], max[Coord::Z] },
		Vec3{ max[Coord::X], min[Coord::Y], max[Coord::Z] }
	};

	return points;
}
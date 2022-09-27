#include "BBox.hpp"

#include "renderer.hpp"

#include <SDK/ICollideable.hpp>
#include <SDK/structs/Entity.hpp>
#include <utilities/math/math.hpp>

Box::Box(Entity_t* ent)
{
	transformPoints(ent);
}

Box Box::transformPoints(Entity_t* ent)
{
	const auto col = ent->collideable();
	if (!col)
	{
		m_isValid = false;
		return {};
	}

	const auto& min = col->OBBMins();
	const auto& max = col->OBBMaxs();

	const auto& matrixWorld = ent->renderableToWorldTransform();

	float left = std::numeric_limits<float>::max();
	float top = std::numeric_limits<float>::max();
	float right = -std::numeric_limits<float>::max();
	float bottom = -std::numeric_limits<float>::max();

	auto points = buildAABB(min, max);

	std::array<ImVec2, 8> screen = {};
	Box resBox = {};

	for (size_t i = 0; auto & el : screen)
	{
		if (!imRender.worldToScreen(math::transformVector(points.at(i), matrixWorld), el))
		{
			m_isValid = false;
			return {};
		}

		left = std::min(left, el.x);
		top = std::min(top, el.y);
		right = std::max(right, el.x);
		bottom = std::max(bottom, el.y);

		resBox.points.at(i) = el;

		i++;
	}

	resBox.x = left;
	resBox.y = top;
	resBox.w = right - left;
	resBox.h = bottom - top;

	// get important points, eg: if you use 3d box, you want to render health by quads, not rects

	resBox.topleft = screen.at(7);
	resBox.topright = screen.at(6);
	resBox.bottomleft = screen.at(3);
	resBox.bottomright = screen.at(2);

	m_isValid = true;

	return resBox;
}

std::array<Vector, 8> Box::buildAABB(const Vector& min, const Vector& max)
{
	std::array points =
	{
		Vector{ min.x, min.y, min.z },
		Vector{ min.x, max.y, min.z },
		Vector{ max.x, max.y, min.z },
		Vector{ max.x, min.y, min.z },
		Vector{ min.x, min.y, max.z },
		Vector{ min.x, max.y, max.z },
		Vector{ max.x, max.y, max.z },
		Vector{ max.x, min.y, max.z }
	};

	return points;
}
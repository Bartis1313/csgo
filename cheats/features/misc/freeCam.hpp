#pragma once

#include "../../../SDK/math/Vector.hpp"

class CViewSetup;

class FreeCam
{
public:
	// need to set angles to remove view effect
	void run(CViewSetup* view);
	_NODISCARD constexpr bool isInCam() const { return m_inCam; }
	void drawInfo() const;
private:
	bool m_inCam;
	Vector m_view;
};

inline FreeCam freeCam;
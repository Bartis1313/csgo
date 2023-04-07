#pragma once

struct Rect_t
{
	int x, y;
	int width, height;
};

struct Rect3D_t
{
	int x, y, z;
	int width, height, depth;

	Rect3D_t(int nX, int nY, int nZ, int nWidth, int nHeight, int nDepth)
	{
		x = nX;
		y = nY;
		z = nZ;
		width = nWidth;
		height = nHeight;
		depth = nDepth;
	}

	Rect3D_t(void)
	{
	}

};

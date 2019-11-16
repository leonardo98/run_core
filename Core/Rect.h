#ifndef RECT_H
#define RECT_H

struct Rect
{
	float x1;
	float y1;
	float x2;
	float y2;
	void Init(float X1, float Y1, float X2, float Y2)
	{
		x1 = X1;
		y1 = Y1;
		x2 = X2;
		y2 = Y2;
	}
	bool ContainDot(float x, float y)
	{
		return (x1 <= x && x <= x2 && y1 <= y && y <= y2);
	}
	float Height()
	{
		return y2 - y1;
	}
};

#endif//RECT_H
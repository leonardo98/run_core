// sources token from HGE

#ifndef MYENGINE_FPOINT2D_H
#define MYENGINE_FPOINT2D_H

#include <math.h>
#include "Box2D/Box2D.h"

namespace Point2dMath {
    float InvSqrt(float x);
}

class FPoint2D
{
public:
	float	x,y;

	FPoint2D(float _x, float _y)	{ x=_x; y=_y; }
	FPoint2D(b2Vec2 b2)	{ x=b2.x; y=b2.y; }
	FPoint2D()						{ x=0; y=0; }

	FPoint2D	operator-  ()					const { return FPoint2D(-x, -y);		}
	FPoint2D	operator-  (const FPoint2D &v) const { return FPoint2D(x-v.x, y-v.y); }
	FPoint2D	operator+  (const FPoint2D &v) const { return FPoint2D(x+v.x, y+v.y); }
	FPoint2D&	operator-= (const FPoint2D &v)		  { x-=v.x; y-=v.y; return *this;	}
	FPoint2D&	operator+= (const FPoint2D &v)		  { x+=v.x; y+=v.y; return *this;	}
	bool		operator== (const FPoint2D &v)	const { return (x==v.x && y==v.y);		}
	bool		operator!= (const FPoint2D &v)	const { return (x!=v.x || y!=v.y);		}

	FPoint2D	operator/  (const float scalar)	const { return FPoint2D(x/scalar, y/scalar); }
	FPoint2D	operator*  (const float scalar) const { return FPoint2D(x*scalar, y*scalar); }
	FPoint2D&	operator*= (const float scalar)		  { x*=scalar; y*=scalar; return *this;   }

	float		Dot(const FPoint2D *v) const { return x*v->x + y*v->y; }
	float		Length() const { return sqrtf(Dot(this)); }
	float		Angle(const FPoint2D *v = 0) const;
	
	void		Clamp(const float max) { if(Length() > max)	{ Normalize(); x *= max; y *= max; } }
	FPoint2D*	Normalize() { float rc=Point2dMath::InvSqrt(Dot(this)); x*=rc; y*=rc; return this; }
	FPoint2D*	Rotate(float a) {
		FPoint2D v;
		v.x=x*cosf(a) - y*sinf(a);
		v.y=x*sinf(a) + y*cosf(a);
		x=v.x; y=v.y;
		return this;
	}
	b2Vec2      ConvertTob2Vec2() { return b2Vec2(x, y); }
    void Set(float inX, float inY) { x = inX; y = inY; }
};

inline FPoint2D operator* (const float s, const FPoint2D &v)		{ return v*s; }
inline float	 operator^ (const FPoint2D &v, const FPoint2D &u) { return v.Angle(&u); }
inline float	 operator% (const FPoint2D &v, const FPoint2D &u) { return v.Dot(&u); }

#endif//MYENGINE_FPOINT2D_H

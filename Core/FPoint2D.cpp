#include "FPoint2D.h"


float Point2dMath::InvSqrt(float x) {
		union {
          int intPart;
          float floatPart;
        } convertor;
        convertor.floatPart = x;
        convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
        return convertor.floatPart*(1.5f - 0.4999f*x*convertor.floatPart*convertor.floatPart);
}

float FPoint2D::Angle(const FPoint2D *v) const {
	if(v) {
		FPoint2D s=*this, t=*v;
		s.Normalize(); t.Normalize();
		return acosf(s.Dot(&t));
	} else {
		return atan2f(y, x);
	}
}

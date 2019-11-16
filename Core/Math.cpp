#include "Math.h"

int Math::round(float a) {
	int b = static_cast<int>(a);
	return (a - b) >= 0.5f ? b + 1 : b;
} 

int Math::sign(float a) {
	return (a > 0 ? 1 : (a < 0 ? -1 : 0));
}

float Math::random(float a, float b) {
	return a + (b - a) * rand() / RAND_MAX;
} 

float Math::Read(rapidxml::xml_node<> *xe, const char *name, const float defaultValue) {
	rapidxml::xml_attribute<> *tmp = xe->first_attribute(name);
	return (tmp ? fatof(tmp->value()) : defaultValue);
}

bool Math::Read(rapidxml::xml_node<> *xe, const char *name, const bool defaultValue) {
	rapidxml::xml_attribute<> *tmp = xe->first_attribute(name);
	if (tmp) {
		std::string s(tmp->value());
		return (tmp ? (s == "true" || s == "1") : defaultValue);
	}
	return defaultValue;
}

bool Math::GetCirclesIntersect(float X1, float Y1, float R1, float X2, float Y2, float R2, FPoint2D &P1, FPoint2D &P2)
{

	if (FPoint2D(X1 - X2, Y1 - Y2).Length() >= R1 + R2) {
		return false;
	}
	// Ищем пересечение ближайшее только к Near1.
	float  C1 = R1 * R1 - R2 * R2 + Y2 * Y2 - (X2 - X1) * (X2 - X1) - Y1 * Y1,  // заменяющая константа.
		C2 = X2 - X1,                                                        // заменяющая константа.
		C3 = Y2 - Y1,                                                        // заменяющая константа.
		a  = -4 * C2 * C2 - 4 * C3 * C3,
		b  = (8 * C2 * C2 * Y2 + 4 * C1 * C3) / a,
		c  = (4 * C2 * C2 * R2 * R2 - 4 * C2 * C2 * Y2 * Y2 - C1 * C1) / a,
		X_1_1 = 0, X_1_2 = 0, Y_1 = 0, X_2_1 = 0, X_2_2 = 0, Y_2 = 0;
	a = 1;
	float  sqrtVal = b * b - 4 * a * c;
	if (fabs(sqrtVal) < 0.5f) {
		sqrtVal = 0.f;
	}
	if ( sqrtVal < 0  ||  a == 0 )
	{
		assert(false);
		if ( X1 == X2  &&  Y1 == Y2  &&  R1 == R2 )  // Если окружности совпадают, то будем считать, что точка тут:
		{
			P1.x = X1 - R1;
			P1.y = Y1;
			P2.x = X1 + R1;
			P2.y = Y1;
			return true;
		}
		return false;
	}

	Y_1 = (-b + sqrt(sqrtVal)) / (2 * a);
	Y_2 = (-b - sqrt(sqrtVal)) / (2 * a);
	float sqrtVal2 = R1 * R1 - (Y_1 - Y1) * (Y_1 - Y1);
	if (fabs(sqrtVal2) < 0.5f) {
		sqrtVal2 = 0.f;
	}
	X_1_1 = X1 + sqrt(sqrtVal2);
	X_1_2 = X1 - sqrt(sqrtVal2);
	float sqrtVal3 = R2 * R2 - (Y_2 - Y2) * (Y_2 - Y2);
	if (fabs(sqrtVal3) < 0.5f) {
		sqrtVal3 = 0.f;
	}
	X_2_1 = X2 + sqrt(sqrtVal3);
	X_2_2 = X2 - sqrt(sqrtVal3);

	//P1.y = Y_1;
	//P2.y = Y_2;
	int counter = 0;
	if (fabs(FPoint2D(X2 - X_1_1, Y2 - Y_1).Length() - R2) < 1) {
		P1.x = X_1_1;
		P1.y = Y_1;
		counter = 1;
	}
	if (fabs(FPoint2D(X2 - X_1_2, Y2 - Y_1).Length() - R2) < 1) {
		if (counter == 0) {
			P1.x = X_1_2;
			P1.y = Y_1;
			counter = 1;
		} else {
			P2.x = X_1_2;
			P2.y = Y_1;
			return true;
		}
	}
	if (fabs(FPoint2D(X1 - X_2_1, Y1 - Y_2).Length() - R1) < 1) {
		if (counter == 0) {
			P1.x = X_2_1;
			P1.y = Y_2;
			counter = 1;
		} else {
			P2.x = X_2_1;
			P2.y = Y_2;
			return true;
		}
	}
	if (fabs(FPoint2D(X1 - X_2_2, Y1 - Y_2).Length() - R1) < 1) {
		if (counter == 0) {
			P1.x = X_2_2;
			P1.y = Y_2;
			counter = 1;
		} else {
			P2.x = X_2_2;
			P2.y = Y_2;
			return true;
		}
	}
	assert(false);
	return true;
}

float Math::VMul(const FPoint2D &one, const FPoint2D &two) {
	return (one.x * two.y - one.y * two.x);
}

float Math::SMul(const FPoint2D &one, const FPoint2D &two) {
	return (one.x * two.x + one.y * two.y) / (one.Length() * two.Length());
}

bool Math::STrinagle(float a, float b, float c, float &s) {
	float p = (a + b + c) / 2;
	float underRoot = p * (p - a) * (p - b) * (p - c);
	if (underRoot > 0.f) {
		s = sqrt(underRoot);
		return true;
	}
	s = -1.f;
	return false;
}

float Math::STrinagle(const FPoint2D &one, const FPoint2D &two, const FPoint2D &three) {
	float a = (one - two).Length();
	float b = (two - three).Length();
	float c = (three - one).Length();
	float p = (a + b + c) / 2;
	float underRoot = p * (p - a) * (p - b) * (p - c);
	if (underRoot > 0.f) {
		return sqrt(underRoot);
	}
	return 0.f;
}

bool Math::GetCirclesIntersect2(float x1, float y1, float r1, float x2, float y2, float r2, FPoint2D &q1, FPoint2D &q2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	float d = sqrt(dx * dx + dy * dy);
	float sTriangle;
	if (!STrinagle(d, r1, r2, sTriangle)) {
		return false;
	}
	float h = 2 * sTriangle / d;
	float t = sqrt(r1 * r1 - h * h);
	FPoint2D o(dx / d * t, dy / d * t);
	FPoint2D po(o.y, -o.x);
	o.x += x1;
	o.y += y1;
	po = po / po.Length() * h;
	q1 = o + po;
	q2 = o - po;
	return true;
}

std::string Math::IntToStr(int i) {
	char buff[100];
	sprintf(buff, "%i", i);
	return buff;
}

FPoint2D Math::lerp(const FPoint2D &one, const FPoint2D &two, float p) {
	return FPoint2D((two.x - one.x) * p + one.x, (two.y - one.y) * p + one.y);
}

float Math::lerp(float one, float two, float p) {
	return (two - one) * p + one;
}

int Math::lerp(int one, int two, float p) {
	return Math::round((two - one) * p + one);
}

void Math::FloatToChar(float f, char *s) {
	sprintf(s, "%g", f);
	unsigned int i = 0;
	unsigned int len = strlen(s);
	while (i < len && (s[i] != '.' && s[i] != ',')) {
		++i;
	}
	if (i == len) {
		s[i++] = '.';
		s[i++] = '0';
		s[i] = 0;
	}
}

bool Math::Intersection(const FPoint2D &line1Start, const FPoint2D &line1End, 
							const FPoint2D &line2Start, const FPoint2D &line2End, FPoint2D *result)
{
	float a1 = (line1Start.y - line1End.y);//x coeff
	float b1 = -(line1Start.x - line1End.x);//y coeff
	float c1 = - (a1 * line1Start.x + b1 * line1Start.y);

	float a2 = (line2Start.y - line2End.y);//x coeff
	float b2 = -(line2Start.x - line2End.x);//y coeff
	float c2 = - (a2 * line2Start.x + b2 * line2Start.y);

	// решаем систему 
	// a1 * x + b1 * y + c1 = 0
	// a2 * x + b2 * y + c2 = 0

	float underline = a2 * b1 - a1 * b2;
	if (fabs(underline) < 1e-3) {
		return false;
	}
	FPoint2D r((b2 * c1 - b1 * c2) / underline, (a1 * c2 - a2 * c1) / underline);
	if (result) {
		*result = r;
	}
	float l1 = (line1Start - line1End).Length() - 1e-3f;
	float l2 = (line2Start - line2End).Length() - 1e-3f;
	if ((line1Start - r).Length() < l1 && (line1End - r).Length() < l1
		&& (line2Start - r).Length() < l2 && (line2End - r).Length() < l2) {
		return true;
	}
	return false;
}

bool Math::Intersection(const b2Vec2 &line1Start, const b2Vec2 &line1End, 
							const b2Vec2 &line2Start, const b2Vec2 &line2End, b2Vec2 *result)
{
	float a1 = (line1Start.y - line1End.y);//x coeff
	float b1 = -(line1Start.x - line1End.x);//y coeff
	float c1 = - (a1 * line1Start.x + b1 * line1Start.y);

	float a2 = (line2Start.y - line2End.y);//x coeff
	float b2 = -(line2Start.x - line2End.x);//y coeff
	float c2 = - (a2 * line2Start.x + b2 * line2Start.y);

	// решаем систему 
	// a1 * x + b1 * y + c1 = 0
	// a2 * x + b2 * y + c2 = 0

	float underline = a2 * b1 - a1 * b2;
	if (fabs(underline) < 1e-3) {
		return false;
	}
	b2Vec2 r((b2 * c1 - b1 * c2) / underline, (a1 * c2 - a2 * c1) / underline);
	if (result) {
		*result = r;
	}
	float l1 = (line1Start - line1End).Length() - 1e-3f;
	float l2 = (line2Start - line2End).Length() - 1e-3f;
	if ((line1Start - r).Length() < l1 && (line1End - r).Length() < l1
		&& (line2Start - r).Length() < l2 && (line2End - r).Length() < l2) {
		return true;
	}
	return false;
}

bool Math::Inside(const FPoint2D &m, const std::vector<FPoint2D> &dots) {
	// проверим находиться ли центр треугольника внтури области
	int counter = 0;
	const int n = dots.size();
	for (int j = 0; j < n; ++j) {
		const FPoint2D *a2 = &dots[j];
		const FPoint2D *b2 = &dots[(j + 1) % n];
		if ((a2->x < b2->x && a2->x < m.x && m.x <= b2->x)
			|| (a2->x > b2->x && a2->x >= m.x && m.x > b2->x)
			) {// найти точку пересечения луча из М и отрезка a2b2
			float k = (a2->y - b2->y) / (a2->x - b2->x);
			float b = a2->y - a2->x * k;
			float y = k * m.x + b;
			if (y > m.y) {
				++counter;
			}
		}
	}
	return (counter % 2 == 1);
}

bool Math::Inside(float x, float y, const std::vector<FPoint2D> &dots) {
	// проверим находиться ли центр треугольника внтури области
	int counter = 0;
	int n = dots.size();
	for (int j = 0; j < n; ++j) {
		const FPoint2D *a2 = &dots[j];
		const FPoint2D *b2 = &dots[(j + 1) % dots.size()];
		if ((a2->x < b2->x && a2->x < x && x <= b2->x)
			|| (a2->x > b2->x && a2->x >= x && x > b2->x)
			) {// найти точку пересечения луча из М и отрезка a2b2
			float k = (a2->y - b2->y) / (a2->x - b2->x);
			float b = a2->y - a2->x * k;
			float yy = k * x + b;
			if (yy > y) {
				++counter;
			}
		}
	}
	return (counter % 2 == 1);
}

bool Math::Inside(const FPoint2D &m, const FPoint2D &a, const FPoint2D &b, const FPoint2D &c) {
	std::vector<FPoint2D> d;
	d.push_back(a);
	d.push_back(b);
	d.push_back(c);
	return Inside(m, d);
}


float Math::Distance(const FPoint2D &one, const FPoint2D &two, const FPoint2D &point) {
	float d = 0;
	float ab = (one - two).Length();
	STrinagle(ab, (two - point).Length(), (point - one).Length(), d);
	return d * 2 / ab; 
}

bool Math::DotNearLine(const FPoint2D &one, const FPoint2D &two, const FPoint2D &p) {
	float a = (one - p).Length();
	float b = (p - two).Length();
	float c = (one - two).Length();
	if (c > a && c > b) {
		float s;
		Math::STrinagle(a, b, c, s);
		return (s / c < 4);
	}
	return false;
}

unsigned int Math::ReadColor(std::string color) {
	assert(color.substr(0, 2) == "0x" && color.size() == 10);
	unsigned int result = 0;
	unsigned char c;
	for (unsigned int i = 2; i < color.size(); ++i) {
		if (color[i] >= '0' && color[i] <= '9') {
			c = color[i] - '0';
		} else if (color[i] >= 'A' && color[i] <= 'F') {
			c = 10 + (color[i] - 'A');
		} else if (color[i] >= 'a' && color[i] <= 'f') {
			c = 10 + (color[i] - 'a');
		} else {
			assert(false);
		}
		result = (result << 4) + static_cast<unsigned int>(c);
	}
	return result;
}

std::string Math::ColorToString(unsigned int color) {
	static std::string hex("0123456789ABCDEF");
	std::string result;
	for (;color > 0; color = color >> 4) {
		result = hex[color & 0xF] + result;
	}
	return "0x" + result;
}

//proj_b_to_a = a * dot(a, b)/dot(a,a);
bool Math::ProjectionAtoBC(FPoint2D a, FPoint2D b, const FPoint2D &c, FPoint2D &d)
{
	a -= c;
	b -= c;
	d = (b * b.Dot(&a) / b.Dot(&b));
	float f = (fabs(b.x) > 1e-3 ? d.x / b.x : d.y / b.y );
	d += c;
	return 0 <= f && f <= 1;
}

void Math::Replace(std::string &s)
{
    for (std::string::size_type i = 0; i < s.size(); ++i)
    {
        if (s[i] == '\\')
        {
            s[i] = '/';
        }
    }
}

std::string Math::getRandomUnicName()
{
    static std::string allchars = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
    std::string result("");
    int numberOfChars = 5;
    for (int i = 0; i < numberOfChars; ++i)
    {
        result += allchars[rand() % allchars.size()];
    }
    return result;
}


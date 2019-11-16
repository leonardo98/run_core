#ifndef PHISIK_LINK
#define PHISIK_LINK

#include "../Core/types.h"
#include "../Core/FPoint2D.h"

struct PhisicLink
{
	int type;
	PhisicLink(int Type);
	virtual ~PhisicLink();
	virtual void ProcessContact(b2Fixture *, b2Contact *) {}
};

#endif//

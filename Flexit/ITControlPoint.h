#pragma once

#include <string>
#include "ITPoint.h"

class ITControlPoint : public ITPoint
{
private:

	ITPoint* _W; // The current deflection vector.

public:

	ITControlPoint(float x, float y, float z);
	~ITControlPoint(void);

	ITPoint *get_W();
	void set_W(ITPoint *w);
};